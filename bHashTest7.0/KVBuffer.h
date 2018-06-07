//
// Created by weixun on 2017/11/24.
//

#ifndef BHASHTEST1_0_KVBUFFER_H
#define BHASHTEST1_0_KVBUFFER_H
#include <string>
using namespace std;
struct kv{
    string k;
    string v;
};
class KVBuffer{
public:
    kv** kv_buffer;
    int* cursor;
    int KV_buffer_num;
    int sub_file_num;
    KVBuffer(int sub_file_num, int KV_buffer_num);
    bool full_or_not(int file_no);
    int insertToBuffer(int file_no, string base_path, string k, string v, char split_kv);
    int flushToSpills(int file_no, string base_path, char split_kv);
    int flushReminderToSpills(int file_no, string base_path, char split_kv);
    ~KVBuffer();
};
#endif //BHASHTEST1_0_KVBUFFER_H
