//
// Created by weixun on 2017/11/24.
//

#ifndef BHASHTEST8_0_HASHBUCKETS_H
#define BHASHTEST8_0_HASHBUCKETS_H

#include <cstdlib>
#include <vector>
#include <string>
#include "KVBuffer.h"
#include "CountMin.h"
#include "Accumulate.h"

using namespace std;

class HashBuckets{
public:
    base_node * base;
    uint64_t  bucket_num;
    int sub_file_num;
    uint64_t buffer_size_B;
    uint64_t current_size_B;

    HashBuckets(uint64_t bucket_num, int sub_file_num, int accumulate_num, uint64_t buffer_size_B);
    uint64_t get_bucket_id(string k, const unsigned int seed);
    uint64_t get_bucket_id(uint64_t murmur_id);
    int get_file_id(uint64_t bucket_id);
    int fill_hash_buckets(CountMin*countMin, Accumulate*accumulate, string k, int vsize, uint64_t frequency_threshold);
    int transvert_frequency_to_offset(Accumulate*accumulate);
    int grouping(Accumulate*accumulate, string base_path, char split_kv, char split_vv);
    bool buffer_full_or_not(string v);
    int inset(Accumulate*accumulate, string k, string v);
    int flushToGroupSpills(Accumulate*accumulate, string base_path, int file_no, char split_kv, char split_vv);
    ~HashBuckets();
};

#endif //BHASHTEST8_0_HASHBUCKETS_H