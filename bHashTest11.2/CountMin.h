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
    //key buffer
    KeyBuffer *keyBuffer;


    CountMin(int count_min_num, uint64_t count_min_length, int key_buffer_num, int sample[]);

    int add_key_fillSpill(string base_path, string k, string v, char kv_split, int *regp);

    uint64_t get_frequency(string k, const unsigned int seed);
    int count_min_hash(string k, int index);

    int flush_reminder_keyBuffer(string base_path);
    ~CountMin();



};
#endif //BHASHTEST9_0_COUNTMIN_H