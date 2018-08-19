LOCAL_PATH:= $(call my-dir)

# libarchive source location
lib_archive := external/libarchive

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
LOCAL_SRC_FILES				:= utils/flashutils.c
LOCAL_STATIC_LIBRARIES			:= libmtdutils libmmcutils libbmlutils
LOCAL_C_INCLUDES			:= bootable/recovery/mtdutils
LOCAL_MODULE				:= libimgutils
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE                            := bootimgtool
LOCAL_MODULE_TAGS                       := optional
LOCAL_SRC_FILES                         := bootimgtool.cpp minarchive.cpp
LOCAL_STATIC_LIBRARIES                  := libc libbootimg libimgutils libmincrypt \
					   libmtdutils libmmcutils libbmlutils \
					   libarchive libz liblz4
LOCAL_C_INCLUDES			:= $(lib_archive)/libarchive $(lib_archive)/contrib/android/include
LOCAL_LDFLAGS                           := -static
LOCAL_FORCE_STATIC_EXECUTABLE           := true
LOCAL_MODULE_CLASS			:= EXECUTABLES
LOCAL_UNSTRIPPED_PATH			:= $(PRODUCT_OUT)/symbols/utilities
LOCAL_MODULE_PATH			:= $(PRODUCT_OUT)/utilities
include $(BUILD_EXECUTABLE)
