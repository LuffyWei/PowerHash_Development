//
// Created by weixun on 11/26/17.
//

#include <climits>
#include "CountMin.h"
#include "murmurhash.h"
#include "Hyperloglog.h"

using namespace std;

CountMin::CountMin(int count_min_num, uint64_t count_min_length) {
    this->count_min_num = count_min_num;
    this->count_min_length = count_min_length;

    count_min_arrays = new uint64_t*[count_min_num];
    for(int i=0; i<count_min_num; i++){
        count_min_arrays[i] = new uint64_t[count_min_length];
    }

    a = new uint64_t[count_min_num];
    b = new uint64_t[count_min_num];
    p = 1000;
    srand((unsigned)time( NULL ));
    for(int i=0; i<count_min_num; i++){
        a[i] = rand()%p;
        b[i] = rand()%(p*1000);
    }
}

int CountMin::countMin(string k, string v, char kv_split, int *regp) {
    const unsigned int seed = 59;
    /****************野割count-min*****************/
    uint64_t murmur_id = MurmurHash64A(k.c_str(), k.size(), seed);
    hllPatLen(murmur_id, regp);
    for(int i=0; i<count_min_num; i++){
        count_min_arrays[i][count_min_hash(murmur_id, i)]++;
    }

    return 0;
}

uint64_t CountMin::get_frequency(string k, const unsigned int seed) {
    uint64_t min_f = LONG_LONG_MAX;
    /****************野割count-min*****************/
    uint64_t murmur_id = MurmurHash64A(k.c_str(), k.size(), seed);
    for(int i=0; i<count_min_num; i++){
        if(min_f > count_min_arrays[i][count_min_hash(murmur_id, i)]){
            min_f = count_min_arrays[i][count_min_hash(murmur_id, i)];
        }
    }
    return min_f;
}
int CountMin::count_min_hash(uint64_t murmur_id, int index) {
    int i = (int)((a[index]*murmur_id+b[index])%count_min_length);
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
