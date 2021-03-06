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
using namespace std;
struct list_node{
    string k;
    uint64_t fk_size;
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
    uint64_t  bucket_num;
    uint64_t sub_file_num;
    KVBuffer* kvBuffer;
    uint64_t buffer_size_B;
    uint64_t current_size_B;

    HashBuckets(uint64_t bucket_num, uint64_t sub_file_num, uint64_t buffer_size_B, int groupBy_buffer_kvnum);
    uint64_t get_bucket_id(string k, const unsigned int seed);
    uint64_t get_file_id(uint64_t bucket_id);
    int hashBuckets_fillSpill(string base_path, string k, string v, char split_kv);
    int flush_reminder_kvBuffer(string base_path, char split_kv);
    int transvert_frequency_to_offset();
    int grouping(string base_path, char split_kv, char split_vv);
    bool buffer_full_or_not(string v);
    int inset(string k, string v);
    int flushToGroupSpills(string base_path, int file_no, char split_kv, char split_vv);
    int flushRemainderToGroupSpills(string base_path, char split_kv);
    ~HashBuckets();
};

#endif //BHASHTEST1_0_HASHBUCKETS_H
