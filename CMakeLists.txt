cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_NAME TurboBaza)
project(${PROJECT_NAME})

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
  libraries/base64/include
  libraries/minhook/include
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
