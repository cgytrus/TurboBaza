cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_NAME TurboBaza)
project(${PROJECT_NAME})

find_file(WINDOWS_HEADER windows.h)
if(NOT WINDOWS_HEADER)
  message(FATAL_ERROR "Can't find windows.h!")
endif()

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
  libraries/base64/include
  libraries/minhook/include
  libraries/gd.h/include
  libraries/gd.h
  libraries/cocos-headers/
  libraries/cocos-headers/extensions
  libraries/cocos-headers/cocos2dx
  libraries/cocos-headers/cocos2dx/include
  libraries/cocos-headers/cocos2dx/kazmath/include
  libraries/cocos-headers/cocos2dx/platform/win32
  libraries/cocos-headers/cocos2dx/platform/third_party/win32
  libraries/cocos-headers/cocos2dx/platform/third_party/win32/OGLES
)

set(BASE64_WITH_OpenMP ON)
set(OpenMP_C_FLAGS /openmp:experimental)

#set(AVX_CFLAGS /arch:AVX)

# not sure if these 3 work
set(SSSE3_CFLAGS /d2archSSSE3)
set(SSE41_CFLAGS /d2archSSE41)
set(SSE42_CFLAGS /d2archSSE42)

#set(AVX2_CFLAGS /arch:AVX2)

#set(BASE64_REGENERATE_TABLES ON)

add_subdirectory(libraries/minhook)
add_subdirectory(libraries/base64)
target_link_libraries(${PROJECT_NAME} minhook base64)

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/cocos2dx/libcocos2d.lib)
#target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/libraries/cocos-headers/extensions/libExtensions.lib)
