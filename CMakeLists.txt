cmake_minimum_required(VERSION 2.8.0)

project(CuraEngine CXX)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(DARWIN_FLAGS "-force_cpusubtype_ALL -mmacosx-version-min=10.6 -arch x86_64 -arch i386")
	add_definitions(${DARWIN_FLAGS})
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${DARWIN_FLAGS}")
elseif(MINGW)
	add_definitions(-march=pentium4)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--large-address-aware")
else(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")

add_subdirectory(clipper)

include_directories(
	${CMAKE_SOURCE_DIR}
)

add_definitions(-Wall -Wextra -O3 -fomit-frame-pointer)

###############################################################################
# CuraEngine
###############################################################################
set(SOURCES
	main.cpp
	modelFile/modelFile.cpp
	settings.cpp
)

set(HEADERS
	bridge.h
	comb.h
	gcodeExport.h
	infill.h
	inset.h
	layerPart.h
	modelFile/modelFile.h
	multiVolumes.h
	pathOptimizer.h
	polygonOptimizer.h
	raft.h
	settings.h
	skin.h
	skirt.h
	sliceDataStorage.h
	slicer.h
	support.h
	utils/floatpoint.h
	utils/gettime.h
	utils/intpoint.h
	utils/logoutput.h
)

add_executable(CuraEngine
	${SOURCES}
	${HEADERS}
)

target_link_libraries(CuraEngine clipper)

