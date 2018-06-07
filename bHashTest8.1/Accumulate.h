//
// Created by weixun on 18-1-4.
//

#ifndef BHASHTEST8_1_ACCUMULATE_H
#define BHASHTEST8_1_ACCUMULATE_H

#include <string>
#include <vector>


using namespace std;
struct accumulate_node{
    uint64_t  fk_size;
    uint64_t kv_num;
    vector<string> listOfv;
    accumulate_node(){
        fk_size = 0;
        kv_num = 0;
    }
};
class Accumulate {
public:
    accumulate_node ** accumulate;
    int sub_file_num;
    uint64_t bucket_num;
    int accumulate_num;

    Accumulate(int sub_file_num, uint64_t bucket_num, int accumulate_num);
    uint64_t get_bucket_id(uint64_t murmur_id);
    int get_file_id(uint64_t bucket_id);
    int fill_accumulate(string k, int vsize, uint64_t murmur_id, uint64_t bucket_id);
    int transvert_accumulate(int file_no, uint64_t offset);
    int insert_accumulate(string k, string v, uint64_t murmur_id, uint64_t bucket_id);
    int flush_accumulate(string base_path, int file_no, char split_kv, char split_vv);
    ~Accumulate();
};


#endif //BHASHTEST8_1_ACCUMULATE_H
