//
// Created by weixun on 2017/11/24.
//

#ifndef BHASHTEST9_0_KVBUFFER_H
#define BHASHTEST9_0_KVBUFFER_H
#include <string>
#include <vector>

using namespace std;

struct kv{
    vector<string> sub_buffer;
};
class KVBuffer{
public:
    kv* kv_buffer;
    int sub_file_num;
    KVBuffer(int sub_file_num);
    int insertToBuffer(int file_no, string k, string v, char split_kv);
    int flushToSpills(string base_path);
    ~KVBuffer();
};
#endif //BHASHTEST9_0_KVBUFFER_H
