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
    int groupBy_buffer_kvnum;
    uint64_t sub_file_num;
    KVBuffer(uint64_t sub_file_num, int groupBy_buffer_kvnum);
    bool full_or_not(int file_no);
    int insertToBuffer(uint64_t file_no, string base_path, string k, string v, char split_kv);
    int flushToSpills(uint64_t file_no, string base_path, char split_kv);
    int flushReminderToSpills(uint64_t file_no, string base_path, char split_kv);
    ~KVBuffer();
};
#endif //BHASHTEST1_0_KVBUFFER_H
