LOCAL_PATH:= $(call my-dir)

#
# Build libraries
#

# libclipper

include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES += libstlport

BUILD_DIR = build
SRC_DIR = src
LIBS_DIR = libs

LOCAL_C_INCLUDES:= \
	$(KERNEL_HEADERS) \
	$(LOCAL_PATH)/include/

LOCAL_CFLAGS:=-DNO_SHARED_LIBS -c -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -std=c++11 -DVERSION=\"\" -isystem libs -O3 -fomit-frame-pointer -fexceptions

LOCAL_CPPFLAGS += -std=c++11

LOCAL_SRC_FILES:= \
	$(LIBS_DIR)/clipper/clipper.cpp

LOCAL_MODULE_TAGS:=
LOCAL_MODULE:=libclipper

include $(BUILD_STATIC_LIBRARY)

