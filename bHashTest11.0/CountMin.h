//
// Created by weixun on 11/26/17.
//
#ifndef BHASHTEST9_0_COUNTMIN_H
#define BHASHTEST9_0_COUNTMIN_H

#include <string>
#include "KeyBuffer.h"

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
    //key buffer
    KeyBuffer *keyBuffer;
    //kv buffer
    int sub_file_num;

    CountMin(int count_min_num, uint64_t count_min_length, int key_buffer_num);
    int add_key_fillSpill(string base_path, string k, string v, char kv_split, int *regp);

    uint64_t get_frequency(string k, const unsigned int seed);
    int count_min_hash(uint64_t murmur_id, int index);

    int flush_reminder_keyBuffer(string base_path);
    ~CountMin();



};
#endif //BHASHTEST9_0_COUNTMIN_H