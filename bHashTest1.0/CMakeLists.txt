cmake_minimum_required(VERSION 3.8)
project(bHashTest1_0)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp HashBuckets.h KVBuffer.h HashBuckets.cpp KVBuffer.cpp murmurhash.h timer.h)
add_executable(bHashTest1_0 ${SOURCE_FILES})