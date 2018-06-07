//
// Created by weixun on 18-1-4.
//

#include <iostream>
#include <fstream>
#include "Accumulate.h"
using namespace std;
string inttostring_accu(int file_no){
    char t[256];
    string s;

    sprintf(t, "%d", file_no);
    s = t;
    return s;
}

Accumulate::Accumulate(int sub_file_num, uint64_t bucket_num, int accumulate_num) {
    this->sub_file_num = sub_file_num;
    this->bucket_num = bucket_num;
    this->accumulate_num = accumulate_num;
    this->accumulate = new struct accumulate_node*[sub_file_num];
    for(int i=0; i<sub_file_num; i++){
        accumulate[i] = new struct accumulate_node[accumulate_num];
    }
}
uint64_t Accumulate::get_bucket_id(uint64_t murmur_id) {
    uint64_t bucket_id = murmur_id % bucket_num;
    return bucket_id;
}
int Accumulate::get_file_id(uint64_t bucket_id) {
    int file_id;
    file_id = bucket_id / (bucket_num / sub_file_num);
    if(file_id == sub_file_num){
        file_id --;
    }
    return file_id;
}
int Accumulate::fill_accumulate(string k, int vsize, uint64_t murmur_id, uint64_t bucket_id) {
    int accumulate_id = murmur_id % accumulate_num;
    int file_no = get_file_id(bucket_id);
    if(accumulate_id >= accumulate_num){
        cout<<"accumulate_id越界！"<<endl;
        exit(0);
    }
    accumulate[file_no][accumulate_id].fk_size += (k.size()+1+vsize+1);
    accumulate[file_no][accumulate_id].kv_num += 1;
    return 0;
}
int Accumulate::transvert_accumulate(int file_no, uint64_t offset) {
    for(int s=0; s<accumulate_num; s++){
        uint64_t current_fk_size = accumulate[file_no][s].fk_size;
        accumulate[file_no][s].fk_size = offset;
        offset += current_fk_size;
    }
}
int Accumulate::insert_accumulate(string k, string v, uint64_t murmur_id, uint64_t bucket_id) {
    int file_id = get_file_id(bucket_id);
    int accumulate_id = murmur_id % accumulate_num;
    accumulate[file_id][accumulate_id].listOfv.push_back(k);
    accumulate[file_id][accumulate_id].listOfv.push_back(v);
    return 0;
}
int Accumulate::flush_accumulate(string base_path, int file_no, char split_kv, char split_vv) {
    fstream out;
    out.open((base_path+"/groupSpill/"+inttostring_accu(file_no)).c_str(), ios::out|ios::in);
    if(!out){
        cout<<"groupSpill"<<file_no<<"打不开"<<endl;
        exit(0);
    }

    uint64_t current_offset = 0;
    for(int i=0; i<accumulate_num; i++){
        if(!accumulate[file_no][i].listOfv.empty()){
            out.seekp(accumulate[file_no][i].fk_size - current_offset, ios::cur);
            for(int j=0; j<accumulate[file_no][i].listOfv.size(); j=j+2){
                out << accumulate[file_no][i].listOfv[j] << split_kv<< accumulate[file_no][i].listOfv[j+1]<<endl;
//                out << accumulate[file_no][i].listOfv[j] << split_kv<< accumulate[file_no][i].listOfv[j+1]<<split_vv;
                out.flush();
                accumulate[file_no][i].fk_size += (accumulate[file_no][i].listOfv[j].size()+ 1 + accumulate[file_no][i].listOfv[j+1].size()+1);
            }
            current_offset = accumulate[file_no][i].fk_size;
            vector<string> temp;
            temp.swap(accumulate[file_no][i].listOfv);
        }
        out.flush();
    }
    out.close();
}

Accumulate::~Accumulate() {
    for(int i=0; i<sub_file_num; i++){
        delete[] accumulate[i];
        accumulate[i] = NULL;
    }
    delete [] accumulate;
    accumulate = NULL;
}