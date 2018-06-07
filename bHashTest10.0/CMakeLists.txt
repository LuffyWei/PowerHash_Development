cmake_minimum_required(VERSION 3.8)
project(bHashTest10_0)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp HashBuckets.h KVBuffer.h HashBuckets.cpp KVBuffer.cpp murmurhash.h murmurhash.cpp CountMin.h CountMin.cpp timer.h Accumulate.cpp Accumulate.h Hyperloglog.h Hyperloglog.cpp quickSort.h)
add_executable(bHashTest10_0 ${SOURCE_FILES})