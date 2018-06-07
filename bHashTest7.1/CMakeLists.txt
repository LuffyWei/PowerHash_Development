cmake_minimum_required(VERSION 3.8)
project(bHashTest7_1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp HashBuckets.h KVBuffer.h HashBuckets.cpp KVBuffer.cpp murmurhash.h murmurhash.cpp CountMin.h CountMin.cpp KeyBuffer.h KeyBuffer.cpp timer.h)
add_executable(bHashTest7_1 ${SOURCE_FILES})