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

#
# Build CuraEngine
#

include $(CLEAR_VARS)

LOCAL_C_INCLUDES:= \
	$(LOCAL_PATH)/include/ \
	$(KERNEL_HEADERS)

LOCAL_CFLAGS:=-DNO_SHARED_LIBS -D__STDC_LIMIT_MACROS -D__ANDROID__ -c -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -std=c++11 -DVERSION=\"\" -isystem libs -O3 -fomit-frame-pointer -fexceptions
LOCAL_CPPFLAGS += -std=c++11

LOCAL_SRC_FILES:= \
	src/bridge.cpp \
	src/comb.cpp \
	src/gcodeExport.cpp \
	src/infill.cpp \
	src/inset.cpp \
	src/layerPart.cpp \
	src/main.cpp \
	src/optimizedModel.cpp \
	src/pathOrderOptimizer.cpp \
	src/polygonOptimizer.cpp \
	src/raft.cpp \
	src/settings.cpp \
	src/skin.cpp \
	src/skirt.cpp \
	src/slicer.cpp \
	src/support.cpp \
	src/timeEstimate.cpp \
	src/modelFile/modelFile.cpp \
	src/utils/gettime.cpp \
	src/utils/logoutput.cpp \
	src/utils/socket.cpp


LOCAL_MODULE_TAGS:=
LOCAL_MODULE:=CuraEngine

LOCAL_STATIC_LIBRARIES := \
	libclipper

include $(BUILD_EXECUTABLE)
