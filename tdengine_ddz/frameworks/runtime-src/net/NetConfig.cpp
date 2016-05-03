#include "NetConfig.h"
#include "utils/TDMacro.h"
#include <regex>

NetConfig* NetConfig::instance()
{
	static NetConfig netConfig;
	return &netConfig;
}

NetConfig::NetConfig(void)
{
}

NetConfig::~NetConfig(void)
{
}


void NetConfig::updateMessage(Json::Value& messageInfo)
{
	auto field_json = messageInfo["field"];
	auto proto_json = messageInfo["proto"];
	if (field_json.isNull() || proto_json.isNull()) {
		return;
	}
	std::map<std::string, td_proto::Field> field;
	std::map<std::string, td_proto::Proto> proto;

	for (auto iter : field_json.getMemberNames()) {
		auto value = field_json[iter];
		field.insert(make_pair(iter, td_proto::Field(value["index"].asInt(), value["pattern"].asString())));
	}
	for (auto iter : proto_json.getMemberNames()) {
		auto value = field_json[iter];
		std::vector<std::string> args;
		for (auto it_array : value["args"]) {
			args.push_back(it_array.asString());
		}
		proto.insert(make_pair(iter, td_proto::Proto(value["msg_type"].asString(), args)));
	}
	config = td_proto::Config(field, proto);
}
