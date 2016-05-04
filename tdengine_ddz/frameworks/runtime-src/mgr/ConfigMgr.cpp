#include "ConfigMgr.h"

ConfigMgr* ConfigMgr::instance()
{
	static ConfigMgr configData;
	return &configData;
}

ConfigMgr::ConfigMgr(void)
	:globalConfig(Json::Value::null)
{
}


ConfigMgr::~ConfigMgr(void)
{
}

void ConfigMgr::initGlobalConfig(const char* data, unsigned long size)
{
	bool success = true;
	Json::Reader reader;
	if (data == NULL) {
		reader.parse("{}", globalConfig, false);
		success = false;
	}
	reader.parse(data, globalConfig, false);
	if (globalConfig.isNull()) {
		reader.parse("{}", globalConfig, false);
		success = false;
	}
}
