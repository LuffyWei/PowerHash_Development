//
// Created by weixun on 11/26/17.
//
#ifndef BHASHTEST9_0_COUNTMIN_H
#define BHASHTEST9_0_COUNTMIN_H

#include <string>
#include "KVBuffer.h"

using namespace std;

class CountMin{
public:
    //count-min ²ÎÊý
    int count_min_num;
    uint64_t  count_min_length;
    uint64_t ** count_min_arrays;
    uint64_t * a;
    uint64_t * b;
    uint64_t p;


    CountMin(int count_min_num, uint64_t count_min_length);
    int countMin(string k, string v, char kv_split, int *regp);
    uint64_t get_frequency(string k, const unsigned int seed);
    int count_min_hash(uint64_t murmur_id, int index);

    ~CountMin();



};
#endif //BHASHTEST9_0_COUNTMIN_H