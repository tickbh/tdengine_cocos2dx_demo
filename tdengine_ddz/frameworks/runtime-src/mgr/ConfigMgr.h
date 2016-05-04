#ifndef _CONFIG_DATA_H_
#define _CONFIG_DATA_H_
#include "json/json.h"
#include <assert.h>

class ConfigMgr
{
public:
	static ConfigMgr* instance();

	ConfigMgr(void);
	~ConfigMgr(void);

	void initGlobalConfig(const char* data, unsigned long size);
	Json::Value globalConfig;
};

#define GlobalConfig ConfigMgr::instance()->globalConfig

#endif