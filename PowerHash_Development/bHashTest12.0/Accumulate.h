//
// Created by weixun on 18-1-2.
//

#ifndef BHASHTEST9_0_ACCUMULATE_H
#define BHASHTEST9_0_ACCUMULATE_H

#include <string>
#include <vector>
#include "KeyBuffer.h"
#include "KVBuffer.h"

using namespace std;



class Accumulate {
public:
    int accumulate_num;
    uint64_t bucket_num_small;
    KVBuffer * kvBuffer;
    uint64_t small_group_num;
    Accumulate(int accumulate_num, uint64_t bucket_num_small);
    uint64_t get_small_bucket_id(uint64_t murmur_id);
    int insert_accumulate(string k, string v, char split_kv, uint64_t murmur_id);
    int flush_accumulate(string base_path, char split_kv, char split_vv);
    int group_accumulate(string base_path, char split_kv);
    int groupBy_small(base_node * base_small, string k, string v);
    ~Accumulate();
};


#endif //BHASHTEST9_0_ACCUMULATE_H
