LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES				:= mkbootimg.c unpackbootimg.c
LOCAL_STATIC_LIBRARIES			:= libmincrypt libcutils libc
LOCAL_MODULE				:= libbootimg
include $(BUILD_STATIC_LIBRARY)
