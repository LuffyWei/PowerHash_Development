//
// Created by weixun on 2017/11/24.
//

#ifndef BHASHTEST1_0_HASHBUCKETS_H
#define BHASHTEST1_0_HASHBUCKETS_H

#include <stdint-gcc.h>
#include <cstdlib>
#include <vector>
#include <string>
#include "KVBuffer.h"
#include "CountMin.h"

using namespace std;
struct list_node{
    string k;
    uint64_t  fk_size;
    vector<string> listOfv;
    list_node * next;
    list_node(){
        fk_size = 0;
        next = NULL;
    }
} ;

struct base_node{
    uint64_t fk_sum;
    list_node * next;
    base_node(){
        fk_sum = 0;
        next = NULL;
    }
};

class HashBuckets{
public:
    base_node * base;
    list_node * accumulate;
    uint64_t  bucket_num;
    uint64_t sub_file_num;
    uint64_t buffer_size_B;
    uint64_t current_size_B;

    HashBuckets(uint64_t bucket_num, uint64_t sub_file_num, uint64_t buffer_size_B);
    uint64_t get_bucket_id(string k, const unsigned int seed);
    int get_file_id(uint64_t bucket_id);
    int fill_hash_buckets(CountMin*countMin, string k, uint64_t frequency_threshold);
    int transvert_frequency_to_offset();
    int grouping(string base_path, char split_kv, char split_vv);
    bool buffer_full_or_not(string v);
    int inset(string k, string v);
    int flushToGroupSpills(string base_path, int file_no, char split_kv, char split_vv);
    ~HashBuckets();
};

#endif //BHASHTEST1_0_HASHBUCKETS_H
