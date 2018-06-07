//
// Created by weixun on 2017/11/24.
//

#include <cstdint>
#include <iostream>
#include <fstream>
#include "KVBuffer.h"
#include "murmurhash.h"

using namespace std;

string inttostring_kv_buffer_(int file_no){
    char t[256];
    string s;

    sprintf(t, "%d", file_no);
    s = t;
    return s;
}

KVBuffer::KVBuffer(int sub_file_num) {
    this->sub_file_num = sub_file_num;
    this->kv_buffer = new struct kv[sub_file_num];
}


int KVBuffer::insertToBuffer(int file_no, string k, string v, char split_kv) {
    if(file_no < 0){
        cout<<"BUG: the file_no is smaller than 0"<<endl;
        return -1;
    }
    string line = k+ split_kv + v;
    kv_buffer[file_no].sub_buffer.push_back(line);
    return 0;
}


int KVBuffer::flushToSpills(string base_path) {
    for(int i=0; i<sub_file_num; i++){
        ofstream out;
        string spill_path = base_path+"/" +inttostring_kv_buffer_(i);
        out.open(spill_path.c_str(), ios::app|ios::out);
        if(!out){
            ofstream create;
            create.open(spill_path.c_str(), ios::out);
            create.close();
            out.open(spill_path.c_str(), ios::app|ios::out);
            if(!out) cout<<"子文件打不开"<<endl;
        }
        for(int j=0; j<kv_buffer[i].sub_buffer.size(); j++){
            out << kv_buffer[i].sub_buffer[j]<<endl;
        }
        vector<string> tmp;
        tmp.swap(kv_buffer[i].sub_buffer);
        out.close();
    }

    return 0;
}


KVBuffer::~KVBuffer() {
    //the pointer must be set to NULL

    for(int i=0; i<sub_file_num; i++){
        vector<string> tmp;
        tmp.swap(kv_buffer[i].sub_buffer);
    }
    delete[] kv_buffer;
    kv_buffer = NULL;
}

