#include "AppDelegate.h"
#include "CCLuaEngine.h"
#include "SimpleAudioEngine.h"
#include "cocos2d.h"
#include "lua_module_register.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_LINUX)
#include "ide-support/CodeIDESupport.h"
#endif

#if (COCOS2D_DEBUG > 0) && (CC_CODE_IDE_DEBUG_SUPPORT > 0)
#include "runtime/Runtime.h"
#include "ide-support/RuntimeLuaImpl.h"
#endif
#include "lua/LuaUtil.h"
#include "../mgr/TDLuaMgr.h"
#include "../lua/LuaNetwork.h"
#include "../mgr/ConfigMgr.h"
#include "../mgr/netmgr/ServiceGate.h"
#include "../net/NetConfig.h"
#include "../lua/LuaRegister.h"
extern "C" {
#include "bit/bit.h"
#include "cjson/lua_cjson.h"
#include "md5/md5.h"

}
using namespace CocosDenshion;

USING_NS_CC;
using namespace std;


AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
    SimpleAudioEngine::end();

#if (COCOS2D_DEBUG > 0) && (CC_CODE_IDE_DEBUG_SUPPORT > 0)
    // NOTE:Please don't remove this call if you want to debug with Cocos Code IDE
    RuntimeEngine::getInstance()->end();
#endif

}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // set default FPS
    Director::getInstance()->setAnimationInterval(1.0f / 60.0f);

    // register lua module
    auto engine = LuaEngine::getInstance();
    ScriptEngineManager::getInstance()->setScriptEngine(engine);
    lua_State* L = engine->getLuaStack()->getLuaState();
    lua_module_register(L);
	LuaRegister::openLibs(L);
	LuaUtil::openLibs(L);
	LuaNetwork::openLibs(L);
	luaopen_bit(L);
	luaopen_cjson(L);
	luaopen_md5_core(L);

	//初始化lua加载函数
	TDLuaMgr::instance();

    register_all_packages();

    LuaStack* stack = engine->getLuaStack();
	stack->setXXTEAKeyAndSign("2dxLua", strlen("2dxLua"), "XXTEA", strlen("XXTEA"));

	{
		Data data = FileUtils::getInstance()->getDataFromFile("src/config/GlobalConfig.conf");
		ConfigMgr::instance()->initGlobalConfig((const char*)data.getBytes(), data.getSize());
	}
	// 设置脚本宏
	LuaMgrIns->registerLuaGlobalVariable("SERVER_TYPE", "client");
	Json::Value macro = GlobalConfig.get("lua_macros", Json::objectValue);
	for (auto mr : macro.getMemberNames()) {
		LuaMgrIns->registerLuaGlobalVariable(mr.c_str(), macro[mr.c_str()].asCString());
	}
	{
		Data data = FileUtils::getInstance()->getDataFromFile("src/config/protocol.txt");
		NetConfig::instance()->updateMessage((const char*)data.getBytes(), data.getSize());
	}

	//CommandLine::instance()->CreateCmdLine();

	Director::getInstance()->getScheduler()->schedule(std::bind(&AppDelegate::onUpdate, this, 0.1f), (void *)this, 0.1f, false, "onUpdate");

	FileUtils::getInstance()->addSearchPath("../../");

    //register custom function
    //LuaStack* stack = engine->getLuaStack();
    //register_custom_function(stack->getLuaState());

#if (COCOS2D_DEBUG > 0) && (CC_CODE_IDE_DEBUG_SUPPORT > 0)
    // NOTE:Please don't remove this call if you want to debug with Cocos Code IDE
    auto runtimeEngine = RuntimeEngine::getInstance();
    runtimeEngine->addRuntime(RuntimeLuaImpl::create(), kRuntimeEngineLua);
    runtimeEngine->start();
#else
    if (engine->executeScriptFile("src/main.lua"))
    {
        return false;
    }
#endif

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

void AppDelegate::onUpdate(float dt) {
	ServiceGate::instance()->onRecvAllMessage();
	LuaMgrIns->executeLua();
}
