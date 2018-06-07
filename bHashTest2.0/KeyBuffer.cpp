#include <fstream>
#include <iostream>
#include "KeyBuffer.h"

//
// Created by weixun on 11/27/17.
//
KeyBuffer::KeyBuffer(int key_buffer_num) {
    this->key_buffer_num = key_buffer_num;
    kBuffer = new string[key_buffer_num];
    cursor = 0;
}

int KeyBuffer::inset_to_keyBuffer(string base_path, string key) {
    if(cursor == key_buffer_num){
        flushToKeyFile(base_path);
    }
    kBuffer[cursor] = key;
    cursor++;

    return 0;
}
int KeyBuffer::flushToKeyFile(string base_path) {

    if(cursor > 0){
        ofstream out;
        out.open(base_path+"/key", ios::app|ios::ate);
        if(!out){
            cout<<"BUG:key �ļ��򲻿�"<<endl;
            exit(0);
        }
        for(int i=0; i<cursor; i++){
            out<<kBuffer[i]<<endl;
        }
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
