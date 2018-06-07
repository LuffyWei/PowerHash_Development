//
// Created by weixun on 11/26/17.
//
#ifndef BHASHTEST9_0_COUNTMIN_H
#define BHASHTEST9_0_COUNTMIN_H

#include <string>
#include "KeyBuffer.h"
#include "KVBuffer.h"

using namespace std;

class CountMin{
public:
    //count-min ²ÎÊý
    int count_min_num;
    uint64_t  count_min_length;
    uint64_t ** count_min_arrays;
    uint64_t * a;
    //key buffer
    KeyBuffer *keyBuffer;
    //kv buffer
    int sub_file_num;
    uint64_t bucket_num;
    KVBuffer *kvBuffer;

    CountMin(int count_min_num, uint64_t count_min_length, int sub_file_num, uint64_t bucket_num,
             int key_buffer_num, int KV_buffer_num, int sample[]);

    int add_key_fillSpill(string base_path, string k, string v, char kv_split, int *regp);
    int get_file_id(string k, const unsigned int seed);
    uint64_t get_frequency(string k, const unsigned int seed);
    int count_min_hash(string k, int index);
//    int count_min_hash(uint32_t k, int index);
    int flush_reminder_kvBuffer(string base_path, char split_kv);
    int flush_reminder_keyBuffer(string base_path);
    ~CountMin();



};
#endif //BHASHTEST9_0_COUNTMIN_H