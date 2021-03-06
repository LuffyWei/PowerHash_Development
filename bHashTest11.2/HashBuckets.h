//
// Created by weixun on 2017/11/24.
//

#ifndef BHASHTEST9_0_HASHBUCKETS_H
#define BHASHTEST9_0_HASHBUCKETS_H

#include <cstdlib>
#include <vector>
#include <string>
#include "CountMin.h"
#include "Accumulate.h"

using namespace std;

class HashBuckets{
public:
    base_node * base;
    uint64_t  bucket_num;
    uint64_t buffer_size_B;
    uint64_t current_size_B;
    uint64_t big_group_num;

    HashBuckets(uint64_t bucket_num,  uint64_t buffer_size_B);
    uint64_t get_bucket_id(string k, const unsigned int seed);
    uint64_t get_bucket_id(uint64_t murmur_id);
    int fill_hash_buckets(CountMin*countMin, Accumulate*accumulate, string k, int vsize, uint64_t frequency_threshold);
    int transvert_frequency_to_offset(Accumulate*accumulate);
    int grouping(Accumulate*accumulate, string input_path, string base_path, char split_kv, char split_vv);
    bool buffer_full_or_not(string v);
    int inset(Accumulate*accumulate, string k, string v);
    int flushToGroupSpills(Accumulate*accumulate, string base_path, char split_kv, char split_vv);
    ~HashBuckets();
};

#endif //BHASHTEST9_0_HASHBUCKETS_H