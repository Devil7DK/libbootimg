LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES				:= mkbootimg.c unpackbootimg.c
LOCAL_STATIC_LIBRARIES			:= libmincrypt libcutils libc
LOCAL_MODULE				:= libbootimg
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES				:= mkbootimg.c unpackbootimg.c
LOCAL_STATIC_LIBRARIES			:= libmincrypt
LOCAL_SHARED_LIBRARIES			:= libcutils libc
LOCAL_MODULE				:= libbootimg
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_SRC_FILES				:= utils/dump_image.c utils/erase_image.c utils/flash_image.c
LOCAL_STATIC_LIBRARIES			:= libmtdutils
LOCAL_C_INCLUDES			:= bootable/recovery/mtdutils
LOCAL_MODULE				:= libimgutils
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE                            := bootimgtool
LOCAL_MODULE_TAGS                       := optional
LOCAL_SRC_FILES                         := bootimgtool.cpp
LOCAL_STATIC_LIBRARIES                  := libc libbootimg libmincrypt
LOCAL_LDFLAGS                           := -static
LOCAL_FORCE_STATIC_EXECUTABLE           := true
LOCAL_MODULE_CLASS			:= EXECUTABLES
LOCAL_UNSTRIPPED_PATH			:= $(PRODUCT_OUT)/symbols/utilities
LOCAL_MODULE_PATH			:= $(PRODUCT_OUT)/utilities
include $(BUILD_EXECUTABLE)
