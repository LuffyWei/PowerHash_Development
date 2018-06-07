//
// Created by weixun on 11/27/17.
//

#ifndef BHASHTEST2_0_KEYBUFFER_H
#define BHASHTEST2_0_KEYBUFFER_H

#include <string>
#include <vector>

using namespace std;
struct key_vsize{
    double k;
    int v_size;
} ;
class KeyBuffer{
public:
//    vector<string> listOfk;
    key_vsize * kBuffer;
    int key_buffer_num;
    int cursor;

    KeyBuffer(int key_buffer_num);
    int inset_to_keyBuffer(string base_path,string key, string v);
    int flushToKeyFile(string base_path);
    int flushRemainderKeyFile(string base_path);
    ~KeyBuffer();
};

#endif //BHASHTEST2_0_KEYBUFFER_H