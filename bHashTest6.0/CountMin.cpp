//
// Created by weixun on 11/26/17.
//

#include <climits>
#include "CountMin.h"
#include "murmurhash.h"
using namespace std;

CountMin::CountMin(int count_min_num, uint64_t count_min_length, int sub_file_num, uint64_t bucket_num,
                   int key_buffer_num, int KV_buffer_num) {
    this->count_min_num = count_min_num;
    this->count_min_length = count_min_length;

    count_min_arrays = new uint64_t*[count_min_num];
    for(int i=0; i<count_min_num; i++){
        count_min_arrays[i] = new uint64_t[count_min_length];
    }

    a = new uint64_t[count_min_num];
    b = new uint64_t[count_min_num];
    p = 10000000;
    srand((unsigned)time( NULL ));
    for(int i=0; i<count_min_num; i++){
        a[i] = rand()%p;
        b[i] = rand()%p;

    }

    this->sub_file_num = sub_file_num;
    this->bucket_num = bucket_num;
    keyBuffer = new KeyBuffer(key_buffer_num);
    kvBuffer = new KVBuffer(sub_file_num, KV_buffer_num);

}

int CountMin::add_key_fillSpill(string base_path, string k, string v, char kv_split) {

    const unsigned int seed = 59;
    /****************填充count-min*****************/

    for(int i=0; i<count_min_num; i++){
        count_min_arrays[i][count_min_hash(k, i)]++;
    }

    /******************写入key文件**************************/
    keyBuffer->inset_to_keyBuffer(base_path, k, v);
    /*******************写入spill文件******************************/
    int file_id = get_file_id(k, seed);
    kvBuffer->insertToBuffer(file_id, base_path, k, v, kv_split);
    return 0;
}

int CountMin::get_file_id(string k, const unsigned int seed) {
    uint64_t murmur_id = MurmurHash64A(k.c_str(), k.length(), seed);
    uint64_t bucket_id = murmur_id % bucket_num;
    int file_id;
    file_id = bucket_id/(bucket_num/sub_file_num);
    if(file_id == sub_file_num){
        file_id--;
    }
    return  file_id;
}

int CountMin::flush_reminder_keyBuffer(string base_path) {
    keyBuffer->flushRemainderKeyFile(base_path);
    delete keyBuffer;
}
int CountMin::flush_reminder_kvBuffer(string base_path, char split_kv) {
    for(int i=0; i<sub_file_num; i++){
        kvBuffer->flushReminderToSpills(i, base_path, split_kv);
    }
    delete kvBuffer;
}
uint64_t CountMin::get_frequency(uint32_t k) {
    uint64_t min_f = LONG_LONG_MAX;
    for(int i=0; i<count_min_num; i++){
        if(min_f > count_min_arrays[i][count_min_hash(k, i)]){
            min_f = count_min_arrays[i][count_min_hash(k, i)];
        }
    }
    return min_f;
}
int CountMin::count_min_hash(string k, int index) {
    uint32_t key_int = atoi(k.c_str());
    int i = (a[index]*key_int+b[index])%count_min_length;
    return i;
}
int CountMin::count_min_hash(uint32_t k, int index) {
    int i = (a[index]*k+b[index])%count_min_length;
    return i;
}

CountMin::~CountMin() {
    delete[] a;
    a = NULL;
    delete[] b;
    b = NULL;
    for(int i=0; i<count_min_num; i++){
        delete[] count_min_arrays[i];
        count_min_arrays[i] = NULL;
    }
    delete[] count_min_arrays;
    count_min_arrays = NULL;
}
