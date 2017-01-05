# what flags you want to pass to the C compiler & linker
CPPFLAGS += -c -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -std=c++0x -DVERSION=\"$(VERSION)\" -isystem libs
#LDFLAGS = -L../build -lclipper

BUILD_DIR = ../build

CPPFLAGS += -I../libs

SOURCES_RAW = bridge.cpp \
comb.cpp \
gcodeExport.cpp \
infill.cpp \
inset.cpp \
layerPart.cpp \
main.cpp \
modelFile/modelFile.cpp \
optimizedModel.cpp \
pathOrderOptimizer.cpp \
polygonOptimizer.cpp \
raft.cpp \
settings.cpp \
skin.cpp \
skirt.cpp \
slicer.cpp \
support.cpp \
timeEstimate.cpp \
utils/gettime.cpp \
utils/logoutput.cpp \
utils/socket.cpp

HEADERS_RAW = bridge.h \
comb.h \
fffProcessor.h \
gcodeExport.h \
infill.h \
inset.h \
layerPart.h \
modelFile/modelFile.h \
multiVolumes.h \
optimizedModel.h \
pathOrderOptimizer.h \
polygonOptimizer.h \
raft.h \
settings.h \
skin.h \
skirt.h \
sliceDataStorage.h \
slicer.h \
support.h \
timeEstimate.h \
utils/curastring.h \
utils/floatpoint.h \
utils/gettime.h \
utils/intpoint.h \
utils/logoutput.h \
utils/polygon.h \
utils/polygondebug.h \
utils/socket.h

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = $(BUILD_DIR)/CuraEngine
noinst_LIBRARIES = $(BUILD_DIR)/libCuraEngine.a

__BUILD_DIR__CuraEngine_LDADD = -L../build -lclipper
#__BUILD_DIR__CuraEngine_LDFLAGS = -lclipper 
__BUILD_DIR__CuraEngine_SOURCES = $(SOURCES_RAW) $(HEADERS_RAW)
__BUILD_DIR__libCuraEngine_a_SOURCES = $(SOURCES_RAW) $(HEADERS_RAW)
