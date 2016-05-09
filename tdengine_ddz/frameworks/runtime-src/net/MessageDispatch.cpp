#include "MessageDispatch.h"
#include "NetConfig.h"
#include "lua/LuaRegister.h"

#if	( LUA_VERSION_NUM < 502 )
#define  lua_rawlen lua_objlen
#endif

MessageDispatch* MessageDispatch::instance()
{
	static MessageDispatch messageDispatch;
	return &messageDispatch;
}

MessageDispatch::MessageDispatch(void)
{
}


MessageDispatch::~MessageDispatch(void)
{
}

bool MessageDispatch::unpackBuffer( lua_State* lua, NetMsg* input)
{
	input->rpos(0);
	input->readHead();
	auto msgName = input->getPackName();
	input->rpos(MIN_LENGTH);
	auto unpackValue = td_proto::decode_proto(*input, NetConfig::instance()->getConfig());
	if (!input->isVaild()) {
		lua_pushnil(lua);
		return false;
	}
	lua_pushstring(lua, msgName.c_str());
	lua_newtable(lua);
	int idx = 1;
	for (auto iter : std::get<1>(unpackValue)) {
		lua_pushnumber(lua, idx++);
		LuaRegister::pushValues(lua, iter);
		lua_settable(lua, -3);

	}
	return true;
}

bool MessageDispatch::lua_convert_value(std::vector<td_proto::Values>& values, lua_State* lua, i32 index, std::vector<std::string>& args)
{
	auto size = lua_gettop(lua) - index + 1;
	if (size != args.size()) {
		return false;
	}
	auto config = NetConfig::instance()->getConfig();
	for (int i = 0; i < size; i++) {
		td_proto::Values sub_value;
		auto success = lua_read_value(sub_value, lua, config, i + index, args.at(i).c_str());
		if (!success) {
			return false;
		}
		values.push_back(std::move(sub_value));
	}
	return true;
}

bool MessageDispatch::lua_read_value(td_proto::Values& value, lua_State* lua, td_proto::Config& config, i32 index, const char* arg)
{
#define CHECK_TYPE_VAILD(fn, index) \
	if (!fn(lua, index)) { \
		return false; \
	}
	auto t = td_proto::get_type_by_name(arg);
	auto success = true;
	switch (t) {
	case td_proto::TYPE_NIL:
		break;
	case td_proto::TYPE_U8:
		CHECK_TYPE_VAILD(lua_isnumber, index);
		value = td_proto::Values((u8)lua_tonumber(lua, index));
		break;
	case td_proto::TYPE_I8:
		CHECK_TYPE_VAILD(lua_isnumber, index);
		value = td_proto::Values((i8)lua_tonumber(lua, index));
		break;
	case td_proto::TYPE_U16:
		CHECK_TYPE_VAILD(lua_isnumber, index);
		value = td_proto::Values((u16)lua_tonumber(lua, index));
		break;
	case td_proto::TYPE_I16:
		CHECK_TYPE_VAILD(lua_isnumber, index);
		value = td_proto::Values((i16)lua_tonumber(lua, index));
		break;
	case td_proto::TYPE_U32:
		CHECK_TYPE_VAILD(lua_isnumber, index);
		value = td_proto::Values((u32)lua_tonumber(lua, index));
		break;
	case td_proto::TYPE_I32:
		CHECK_TYPE_VAILD(lua_isnumber, index);
		value = td_proto::Values((i32)lua_tonumber(lua, index));
		break;
	case td_proto::TYPE_FLOAT:
		CHECK_TYPE_VAILD(lua_isnumber, index);
		value = td_proto::Values((float)lua_tonumber(lua, index));
		break;
	case td_proto::TYPE_STR:
		CHECK_TYPE_VAILD(lua_isstring, index);
		value = td_proto::Values(new std::string(lua_tostring(lua, index)));
		break;
	case td_proto::TYPE_RAW: {
		CHECK_TYPE_VAILD(lua_isstring, index);
		size_t size = 0;
		auto info = lua_tolstring(lua, index, &size);
		auto vecs = new std::vector<char>(size);
		memcpy(&vecs[0], info, size);
		value = td_proto::Values(vecs);
	}
		break;
	case td_proto::TYPE_MAP: {
		auto val = new std::map<std::string, td_proto::Values>();
		value = td_proto::Values(val);
		lua_pushnil(lua);
		auto t = index >= 0 ? index : index - 1;
		while (lua_istable(lua, t) && lua_next(lua, t) != 0) {
			CHECK_TYPE_VAILD(lua_isstring, -2);
			std::string key = lua_tostring(lua, -2);
			auto field = config.get_field_by_name(key);
			if (field != nullptr) {
				td_proto::Values sub_val;
				auto success = lua_read_value(sub_val, lua, config, -1, field->pattern.c_str());
				if (!success) {
					return false;
				}
				val->insert(make_pair(key, std::move(sub_val)));
			}
			lua_pop(lua, 1);
		}
		break;
	}
	case td_proto::TYPE_AU8:
	case td_proto::TYPE_AI8:
	case td_proto::TYPE_AU16:
	case td_proto::TYPE_AU32:
	case td_proto::TYPE_AI32:
	case td_proto::TYPE_AFLOAT:
	case td_proto::TYPE_ASTR:
	case td_proto::TYPE_ARAW:
	case td_proto::TYPE_AMAP: {
		auto val = new std::vector<td_proto::Values>();
		value = td_proto::Values(val, t);
		CHECK_TYPE_VAILD(lua_istable, index);
		auto len = lua_rawlen(lua, index);
		for (unsigned int i = 1; i < len + 1; i++) {
			lua_pushnumber(lua, i);
			auto new_index = index >= 0 ? index : index - 1;
			lua_gettable(lua, new_index);
			td_proto::Values sub_val;
			auto success = lua_read_value(sub_val, lua, config, -1, td_proto::get_name_by_type(t - (td_proto::TYPE_AU8 - td_proto::TYPE_U8)));
			if (!success) {
				return false;
			}
			val->push_back(std::move(sub_val));
			lua_pop(lua, 1);
		}
		break;
	}
	default:
		return false;
	}
	return true;
}
