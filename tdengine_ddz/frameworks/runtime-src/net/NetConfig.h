#ifndef _NET_CONFIG_H_
#define _NET_CONFIG_H_
#include <map>
#include <string>
#include <fstream>

#include "json/json.h"
//#include "td_proto_cpp/include/Values.h"
#include "td_proto_cpp/include/Config.h"

class NetConfig
{
public:
	static NetConfig* instance();
	~NetConfig(void);

	td_proto::Config& getConfig() {
		return this->config;
	}

	void updateMessage(Json::Value& messageInfo);
private:

	td_proto::Config config;
	NetConfig(void);

};


#endif