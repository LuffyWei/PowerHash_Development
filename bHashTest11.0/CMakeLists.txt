cmake_minimum_required(VERSION 3.8)
project(bHashTest11_0)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp HashBuckets.h HashBuckets.cpp murmurhash.h murmurhash.cpp KeyBuffer.h KeyBuffer.cpp timer.h Accumulate.cpp Accumulate.h Hyperloglog.h Hyperloglog.cpp quickSort.h CountMin.h CountMin.cpp)
add_executable(bHashTest11_0 ${SOURCE_FILES})