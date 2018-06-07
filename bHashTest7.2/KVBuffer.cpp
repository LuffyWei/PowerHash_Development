//
// Created by weixun on 2017/11/24.
//

#include <cstdint>
#include <iostream>
#include <fstream>
#include "KVBuffer.h"
using namespace std;

string inttostring_kv_buffer_(int file_no){
    char t[256];
    string s;

    sprintf(t, "%d", file_no);
    s = t;
    return s;
}

KVBuffer::KVBuffer(int sub_file_num, int KV_buffer_num) {
    this->sub_file_num = sub_file_num;
    this->KV_buffer_num = KV_buffer_num;
    this->cursor = new int[sub_file_num];
    this->kv_buffer = new struct kv*[sub_file_num];
    for(int i=0; i<sub_file_num; i++){
        kv_buffer[i] = new struct kv[KV_buffer_num];
        cursor[i] = 0;
    }
}



int KVBuffer::insertToBuffer(int file_no, string base_path, string k, string v, char split_kv) {
    if(file_no < 0){
        cout<<"BUG: the file_no is smaller than 0"<<endl;
        return -1;
    }
    int cursor_of_file_no = cursor[file_no];
    if(full_or_not(cursor_of_file_no)){
        flushToSpills(file_no,  base_path, split_kv);
        //如果清空buffer，指针必须还原
        cursor_of_file_no = cursor[file_no];
    }

    kv_buffer[file_no][cursor_of_file_no].k = k;
    kv_buffer[file_no][cursor_of_file_no].v = v;
    cursor[file_no]++;
    return 0;

}

bool KVBuffer::full_or_not(int cursor_of_file_no) {
    if(cursor_of_file_no <0){
        cout<<"BUG:the cursor is smaller than 0"<<endl;
        exit(0);
    } else{
        if(cursor_of_file_no == KV_buffer_num){
            return true;
        } else{
            return false;
        }
    }
}

int KVBuffer::flushToSpills(int file_no, string base_path, char split_kv) {

    ofstream out;
    ///?????
    string spill_path = base_path+"/spill/"+inttostring_kv_buffer_(file_no);
    out.open(spill_path.c_str(), ios::app|ios::ate);
    if(!out){
        cout<<"BUG: KVBuffer::flushToSPills: cannot open the spills"<<endl;
        exit(0);
    }
    for(int i=0; i<KV_buffer_num; i++){
        out << kv_buffer[file_no][i].k<<split_kv<<kv_buffer[file_no][i].v<<endl;
    }
    out.close();
    cursor[file_no] = 0;

    return 0;
}

int KVBuffer::flushReminderToSpills(int file_no, string base_path, char split_kv) {
    if(file_no < 0){
        cout<<"BUG: the file_no is smaller than 0"<<endl;
        return -1;
    }

    int cursor_of_file_no = cursor[file_no];
    if(cursor_of_file_no > 0){
        string spill_path = base_path+"/spill/"+inttostring_kv_buffer_(file_no);
        ofstream out;
        out.open(spill_path.c_str(), ios::app|ios::ate);
        for(int i=0; i<cursor_of_file_no; i++){
            out<< kv_buffer[file_no][i].k<<split_kv<<kv_buffer[file_no][i].v<<endl;
        }
    }
    return 0;
}

KVBuffer::~KVBuffer() {
    //the pointer must be set to NULL
    delete[] cursor;
    cursor = NULL;
    for(int i=0; i<sub_file_num; i++){
        delete[] kv_buffer[i];
        kv_buffer[i] = NULL;
    }
    delete[] kv_buffer;
    kv_buffer = NULL;
}

