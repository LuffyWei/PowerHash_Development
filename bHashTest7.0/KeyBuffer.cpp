#include <fstream>
#include <iostream>
#include "KeyBuffer.h"

//
// Created by weixun on 11/27/17.
//
KeyBuffer::KeyBuffer(int key_buffer_num) {
    this->key_buffer_num = key_buffer_num;
    kBuffer = new struct key_vsize[key_buffer_num];
    cursor = 0;
}

int KeyBuffer::inset_to_keyBuffer(string base_path, string key, string v) {
    if(cursor == key_buffer_num){
        flushToKeyFile(base_path);
    }
    kBuffer[cursor].k = key;
//    cout<<kBuffer[cursor].k<<endl;
    kBuffer[cursor].v_size = v.size();
    cursor++;
    return 0;
}
int KeyBuffer::flushToKeyFile(string base_path) {

    if(cursor > 0){
        fstream out;
        out.open((base_path+"/key").c_str(), ios::app|ios::ate|ios::binary);
        if(!out){
            cout<<"BUG:key 文件打不开"<<endl;
            exit(0);
        }
//        for(int i=0; i<cursor; i++){
//            cout<<kBuffer[i].k<<endl;
//        }
        out.write((char*)kBuffer, cursor*sizeof(key_vsize));
        out.flush();
        out.close();
        cursor = 0;
    }
    return 0;
}
int KeyBuffer::flushRemainderKeyFile(string base_path) {
    if(cursor > 0){
        flushToKeyFile(base_path);
    }
    return 0;
}
KeyBuffer::~KeyBuffer() {
    delete[] kBuffer;
    kBuffer = NULL;
}
