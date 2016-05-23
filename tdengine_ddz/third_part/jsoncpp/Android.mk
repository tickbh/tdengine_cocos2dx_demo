LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := json_static

LOCAL_MODULE_FILENAME := libjson
APP_STL := gnustl_static
LOCAL_CPPFLAGS += -std=c++11 -fexceptions
LOCAL_CFLAGS := -D_GLIBCXX_PERMIT_BACKWARD_HASH

LOCAL_SRC_FILES:= \
src/lib_json/json_reader.cpp \
src/lib_json/json_value.cpp \
src/lib_json/json_writer.cpp \
	
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/
						
LOCAL_EXPORT_LDLIBS := -lz \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

include $(BUILD_STATIC_LIBRARY)
