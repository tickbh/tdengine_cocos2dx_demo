#ifndef _MESSAGE_DISPATCH_H_
#define _MESSAGE_DISPATCH_H_

#include <string>
using namespace std;

#include "NetMsg.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#define MSG_DB_ID 0xFFF1

class MessageDispatch
{
public:
	static MessageDispatch* instance();
	MessageDispatch(void);
	~MessageDispatch(void);

	bool lua_read_value(td_proto::Values& value, lua_State* lua, td_proto::Config& config, i32 index, const char* arg);
	bool lua_convert_value(std::vector<td_proto::Values>& values, lua_State* lua, i32 index, std::vector<std::string>& args);
	bool unpackBuffer(lua_State* lua, NetMsg* input);
private:


};

#endif