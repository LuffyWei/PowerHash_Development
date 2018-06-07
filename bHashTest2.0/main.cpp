
#include <iostream>
#include <fstream>

#include "HashBuckets.h"
#include "CountMin.h"

using namespace std;

string inttostring_main(int file_no){
    char t[256];
    string s;

    sprintf(t, "%d", file_no);
    s = t;
    return s;
}
void group_by(string input_path, string base_path, int count_min_num, uint64_t count_min_length,
              uint64_t bucket_num, uint64_t sub_file_num, uint64_t buffer_size_B,
              int key_buffer_num, int groupBy_buffer_kvnum){

    CountMin * countMin = new CountMin(count_min_num, count_min_length, sub_file_num, bucket_num, key_buffer_num, groupBy_buffer_kvnum);
    /***************读取源文件，同时生成hash表和spills文件*******************/
    ifstream in;
    in.open(input_path.c_str(), ios::in);
    if(!in){
        cout<<"Cannot open input file"<<endl;
        exit(0);
    }
    string line;
    getline(in, line);
    while (!in.eof()){
        string::size_type index = line.find_first_of('\t', 0);
        countMin->add_key_fillSpill(base_path, line.substr(0, index), line.substr(index+1),'\t');
        getline(in, line);
    }
    in.close();
    countMin->flush_reminder_keyBuffer(base_path);
    countMin->flush_reminder_kvBuffer(base_path, '\t');

    /********************读取key文件，然后填充hash表*********************************/
    HashBuckets * hashBuckets = new HashBuckets(bucket_num, sub_file_num, buffer_size_B);
    in.open((base_path+"/key").c_str(), ios::in);
    if(!in){
        cout<<"Cannot open key_file"<<endl;
        exit(0);
    }
    string key;
    getline(in, key);
    while (!in.eof()){
        hashBuckets->fill_hash_buckets(countMin, key);
        getline(in, key);
    }
    in.close();
    delete countMin;
    if(remove((base_path+"/key").c_str())) cout<<"删除key_file文件失败"<<endl;

    /***************将hash表转换成offset表********************************/
    hashBuckets->transvert_frequency_to_offset();

    /****************完成group-by操作************************************/
    hashBuckets->grouping(base_path, '\t', '\n');
    delete hashBuckets;

    /*****************删除spills文件************************************/
    for(int i=0; i<sub_file_num; i++){
        if(remove((base_path+"/spill/"+inttostring_main(i)).c_str()))
            cout<<"删除spill"<<i<<"失败"<<endl;
    }

}

int main(int argc, char** argv) {
    string input_path = "/home/weixun/KV data/pg_message";
    string base_path = "/home/weixun/CLionProjects/bHashTest2.0/data";
    //count-min的三个参数
    int count_min_num = 5;
    uint64_t count_min_length = 100000;
    uint64_t bucket_num = 10000;
    //子文件个数
    uint64_t sub_file_num = 3;
    //将key写入key_file的buffer，单位是个
    int key_buffer_num = 1000;
    //将数据写入spill的buffer，单位是个
    int groupBy_buffer_kvnum = 1000;
    //从spill写入groupSpill的buffer，单位是byte
    uint64_t buffer_size_B = 100000;

    group_by(input_path, base_path, count_min_num, count_min_length, bucket_num, sub_file_num,
             buffer_size_B, key_buffer_num, groupBy_buffer_kvnum);



    return 0;
}



