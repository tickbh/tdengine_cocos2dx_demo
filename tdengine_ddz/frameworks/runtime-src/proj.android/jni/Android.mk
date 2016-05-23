LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dlua_shared

LOCAL_MODULE_FILENAME := libcocos2dlua

LOCAL_SRC_FILES := \
../../Classes/AppDelegate.cpp \
../../Classes/ide-support/SimpleConfigParser.cpp \
../../Classes/ide-support/RuntimeLuaImpl.cpp \
../../Classes/ide-support/lua_debugger.c \
../../lua/LuaNetwork.cpp \
../../lua/LuaRegister.cpp \
../../lua/LuaTimer.cpp \
../../lua/LuaUtil.cpp \
../../mgr/ConfigMgr.cpp \
../../mgr/TDLuaMgr.cpp \
../../mgr/netmgr/ServiceGate.cpp \
../../mgr/netmgr/ServiceMgr.cpp \
../../mgr/netmgr/SocketEvent.cpp \
../../net/MessageDispatch.cpp \
../../net/NetConfig.cpp \
../../net/ReadInfo.cpp \
../../net/TDSocket.cpp \
../../utils/TDThread.cpp \
../../utils/TDThreadPool.cpp \
../../utils/TDUtils.cpp \
hellolua/main.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
$(LOCAL_PATH)/../../ \
$(LOCAL_PATH)/../../../../third_part/jsoncpp/include \
$(LOCAL_PATH)/../../../../third_part/lualib \
$(LOCAL_PATH)/../../../../external

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_STATIC_LIBRARIES := cocos2d_lua_static
LOCAL_STATIC_LIBRARIES += cocos2d_simulator_static
LOCAL_STATIC_LIBRARIES += json_static
LOCAL_STATIC_LIBRARIES += lualib_static


# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,../../../../third_part/jsoncpp)
$(call import-module,../../../../third_part/lualib)
$(call import-module,scripting/lua-bindings/proj.android)
$(call import-module,tools/simulator/libsimulator/proj.android)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
