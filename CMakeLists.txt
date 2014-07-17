CMAKE_MINIMUM_REQUIRED (VERSION 2.6) 

# project/binary name:
PROJECT(Jedi)

message("Jedi - Occulus Rift - Starting Project Build")

#include "findASSIMP.cmake"...
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extern/acgl/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extern/wiic/src/cmake_modules/")

# ACGL setup:
ADD_DEFINITIONS(-DACGL_OPENGL_VERSION_32)
ADD_DEFINITIONS(-DACGL_OPENGL_PROFILE_CORE)
INCLUDE(${CMAKE_SOURCE_DIR}/extern/acgl/CMakeListsStaticInclude.txt)

# Wiiuse setup
INCLUDE(${CMAKE_SOURCE_DIR}/extern/wiic/CMakeListsStaticInclude.txt)
include_directories(extern/wiic/src/wiicpp)
include_directories(extern/wiic/src/wiic)
include_directories(extern/wiic/src/log)
SET(LIBRARIES ${LIBRARIES} "-lbluetooth")

###############################################################################
#
# Compiler settings, can be simpler if only one compiler should be used.
#

# enable warnings
ADD_DEFINITIONS(-Wall)
ADD_DEFINITIONS(-DNO_SPACE_NAVIGATOR_SUPPORT)

# C++11 support for gcc:
SET(CMAKE_CXX_FLAGS "-std=c++0x")
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")
SET(CMAKE_C_COMPILER "/usr/bin/g++")

# overwrite for clang++
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  UNSET(CMAKE_CXX_FLAGS)
  SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
  SET(CMAKE_C_COMPILER "/usr/bin/clang++")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#
#
###############################################################################

###############################################################################
#
# Settings for the app.
#
#
# Will set the run directory for QtCreator:
set(dir ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

# source and header files
add_subdirectory(src)

FILE(GLOB_RECURSE SOURCE_FILES_CC "${CMAKE_SOURCE_DIR}/*.cc")

SET(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_FILES_CC} ${cgengine_source} ${glg_source})

FILE(GLOB_RECURSE HEADER_FILES_HH "${CMAKE_SOURCE_DIR}/*.hh")

SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_HH} ${cgengine_headers} ${glg_headers})

include_directories(${CGE_HEADERS})
include_directories(${GLG_HEADERS})

# shader files
FILE(GLOB_RECURSE SHADER_FILES "${CMAKE_SOURCE_DIR}/bin/shader/*.*")

#
#
###############################################################################

###############################################################################
#
# Oculus Rift:
#
INCLUDE(${CMAKE_SOURCE_DIR}/extern/LibOVR/CMakeListsStaticInclude.txt)
#
#
###############################################################################


###############################################################################
#
# OpenAL:
#

#
# Windows:
#
IF(WIN32)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/extern/OpenAL/include)
  SET(LIBRARIES ${LIBRARIES} )
  # yes, libOpenAL_32_ is also correct for the 64bit version:
  SET(LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/OpenAL/lib/Win64/libOpenAL32.dll.a)
ENDIF(WIN32)

#
# MacOS X: System provided OpenAL is done by Apple, to get more features like binaural audio
#          provide your own OpenAL Soft and change the linker flags (additional features
#          are implemented in software anyway).
#
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET(LIBRARIES ${LIBRARIES} -Wl,-framework,OpenAL)
  #SET(LIBRARIES ${LIBRARIES} -L. -lopenal) # if there is a local self-build OpenAL Soft dylib in the same folder
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#
# Linux: Use system provided OpenAL, should be OpenAL Soft.
#
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET(LIBRARIES ${LIBRARIES} -lopenal)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#
#
###############################################################################


###############################################################################
#
# GLFW (and some other linker flags)
#
ADD_DEFINITIONS(-DACGL_COMPILE_WITH_GLFW)

FILE(GLOB_RECURSE HEADER_FILES_GLFW "${CMAKE_SOURCE_DIR}/extern/glfw/include/*.h")
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extern/glfw/include)
SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_GLFW})

#
# Windows:
#
IF(WIN32)
  SET(LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/glfw/lib/glfw3.lib OpenGL32.lib)
ENDIF(WIN32)

#
# MacOS X:
#
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET(LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/glfw/src/libglfw3.a -Wl,-framework,Cocoa -Wl,-framework,OpenGL,-framework,IOKit)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#
# Linux:
#
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET(LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/glfw/src/libglfw3.a -lXrandr -lGL -lXi  -pthread -lm -lX11 -lXxf86vm)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#
#
#
###############################################################################
#Additional Libraries and configurations for our project
#
#

#Qt
find_package(Qt4 REQUIRED)
set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
set(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})

#Assimp Library
find_package(Assimp REQUIRED)
if(NOT ASSIMP_FOUND)
  message(FATAL_ERROR "ASSIMP not found!")
endif ()

include_directories(${ASSIMP_INCLUDE_DIR})
set(LIBRARIES ${LIBRARIES} ${ASSIMP_LIBRARY})

#for checking...
message(STATUS "Assimp " ${ASSIMP_INCLUDE_DIR})
message(STATUS "Assimp Lib " ${ASSIMP_LIBRARY})

#Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "BOOST not found!")
endif ()
message(STATUS "Boost libraries are ${Boost_LIBRARIES}")
include_directories(${Boost_INCLUDE_DIRS})
link_directories( ${Boost_LIBRARY_DIRS} )
set(CXX_FLAGS "-lboost_system")
set(LIBRARIES ${LIBRARIES} ${Boost_LIBRARIES})
set(LIBRARIES ${LIBRARIES} -lboost_system)

# just print out the variables to see their values...
MESSAGE(STATUS "Boost information:")
MESSAGE(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
MESSAGE(STATUS "Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
MESSAGE(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
MESSAGE(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")

#GLEW
find_package(GLEW)
if(NOT GLEW_FOUND)
  message(FATAL_ERROR "GLEW not found!")
endif ()
include_directories(${GLEW_INCLUDE_DIR})
set(LIBRARIES ${LIBRARIES} ${OPENGL_LIBRARIES})
set(LIBRARIES ${LIBRARIES} ${GLEW_LIBRARIES})

###############################################################################
#
# Bullet engine
#
find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})
set(LIBRARIES ${LIBRARIES} ${BULLET_LIBRARIES})

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} ${README_FILES})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${LIBRARIES})
