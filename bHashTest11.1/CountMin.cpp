//
// Created by weixun on 11/26/17.
//

#include <climits>
#include "CountMin.h"
#include "murmurhash.h"
#include "Hyperloglog.h"

using namespace std;

CountMin::CountMin(int count_min_num, uint64_t count_min_length, int key_buffer_num, int sample[]) {
    this->count_min_num = count_min_num;
    this->count_min_length = count_min_length;

    count_min_arrays = new uint64_t*[count_min_num];
    for(int i=0; i<count_min_num; i++){
        count_min_arrays[i] = new uint64_t[count_min_length];
    }

    a = new uint64_t[count_min_num];
    for(int i=0; i<count_min_num; i++){
        a[i] = sample[i];
    }

    keyBuffer = new KeyBuffer(key_buffer_num);

}

int CountMin::add_key_fillSpill(string base_path, string k, string v, char kv_split, int *regp) {

    const unsigned int seed = 59;
    /****************Ìî³äcount-min*****************/
    uint64_t murmur_id = MurmurHash64A(k.c_str(), k.size(), seed);
    hllPatLen(murmur_id, regp);
    for(int i=0; i<count_min_num; i++){
        count_min_arrays[i][count_min_hash(k, i)]++;
    }

    /******************Ğ´ÈëkeyÎÄ¼ş**************************/
    keyBuffer->inset_to_keyBuffer(base_path, k, v);

    return 0;
}


int CountMin::flush_reminder_keyBuffer(string base_path) {
    keyBuffer->flushRemainderKeyFile(base_path);
    delete keyBuffer;
}

uint64_t CountMin::get_frequency(string k, const unsigned int seed) {
    uint64_t min_f = LONG_LONG_MAX;
    /****************Ìî³äcount-min*****************/
    for(int i=0; i<count_min_num; i++){
        if(min_f > count_min_arrays[i][count_min_hash(k, i)]){
            min_f = count_min_arrays[i][count_min_hash(k, i)];
        }
    }
    return min_f;
}
int CountMin::count_min_hash(string k, int index) {
    int i;
    i = (int)(MurmurHash64A(k.c_str(), k.size(), a[index])%count_min_length);
    return i;
}

CountMin::~CountMin() {
    delete[] a;
    a = NULL;
    for(int i=0; i<count_min_num; i++){
        delete[] count_min_arrays[i];
        count_min_arrays[i] = NULL;
    }
    delete[] count_min_arrays;
    count_min_arrays = NULL;
}
