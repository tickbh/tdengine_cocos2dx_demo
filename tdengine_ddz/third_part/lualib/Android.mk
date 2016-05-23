LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := lualib_static

LOCAL_MODULE_FILENAME := liblualib
APP_STL := gnustl_static
LOCAL_CPPFLAGS += -std=c++11 -fexceptions
LOCAL_CFLAGS := -D_GLIBCXX_PERMIT_BACKWARD_HASH

LOCAL_SRC_FILES:= \
bit/bit.c \
cjson/fpconv.c \
cjson/lua_cjson.c \
cjson/strbuf.c \
lua_tinker/lua_tinker.cpp \
md5/des56.c \
md5/ldes56.c \
md5/md5.c \
md5/md5lib.c \

	
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/
						
LOCAL_EXPORT_LDLIBS := -lz \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/

LOCAL_STATIC_LIBRARIES := cocos2d_lua_static
LOCAL_STATIC_LIBRARIES += cocos2d_simulator_static

include $(BUILD_STATIC_LIBRARY)

$(call import-module,scripting/lua-bindings/proj.android)
$(call import-module,tools/simulator/libsimulator/proj.android)