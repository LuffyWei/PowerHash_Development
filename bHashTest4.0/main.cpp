
#include <iostream>
#include <fstream>
#include <cstring>

#include "HashBuckets.h"
#include "CountMin.h"
#include "timer.h"

/**
 * 最原始的count-min bHash方法
 * 使用count-min数组中频率代表group的大小
 *
 * 中间采用二进制的存储方式存储key文件，但是采用的结构体是整型的
 * 只适用于key是整型的数据集
 */
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
              int key_buffer_num, int groupBy_buffer_kvnum, uint64_t frequency_threshold){
    Timer timer;
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
    cout<<"count-min: "<<timer.elapsed()<<endl;

    /********************读取key文件，然后填充hash表*********************************/
    HashBuckets * hashBuckets = new HashBuckets(bucket_num, sub_file_num, buffer_size_B);
    in.open((base_path+"/key").c_str(), ios::in|ios::binary);
    if(!in){
        cout<<"Cannot open key_file"<<endl;
        exit(0);
    }
    uint32_t *kBuffer = new uint32_t[key_buffer_num];
    while (!in.eof()){
        in.read((char*)kBuffer, key_buffer_num*4);
        for(int i=0; i<key_buffer_num; i++){
            if(kBuffer[i] == '\0'){
                break;
            }
            hashBuckets->fill_hash_buckets(countMin, kBuffer[i],frequency_threshold);
        }
        memset(kBuffer, 0, key_buffer_num*4);
    }
    in.close();
    delete countMin;

    /***************将hash表转换成offset表********************************/
    hashBuckets->transvert_frequency_to_offset();
    cout<<"phrase 1: "<<timer.elapsed()<<endl;

    /****************完成group-by操作************************************/
    hashBuckets->grouping(base_path, '\t', '\n');
    cout<<"total time: "<<timer.elapsed()<<endl;
    delete hashBuckets;

    /*****************删除spills文件************************************/
    if(remove((base_path+"/key").c_str())) cout<<"删除key_file文件失败"<<endl;
    for(int i=0; i<sub_file_num; i++){
        if(remove((base_path+"/spill/"+inttostring_main(i)).c_str()))
            cout<<"删除spill"<<i<<"失败"<<endl;
        if(remove((base_path+"/groupSpill/"+inttostring_main(i)).c_str()))
            cout<<"删除groupSpill"<<i<<"失败"<<endl;
    }

}

int main(int argc, char** argv) {
    string input_path = "/home/weixun/KV data/pareto_10000000";
    string base_path = "/home/weixun/CLionProjects/bHashTest4.0/data";
    //count-min的三个参数
    int count_min_num = 3;
    uint64_t count_min_length = 2500000;
    uint64_t bucket_num = 50000;
    //子文件个数
    uint64_t sub_file_num = 5;
    //将key写入key_file的buffer，单位是个
    int key_buffer_num = 10*10000;
    //将数据写入spill的buffer，单位是个
    int groupBy_buffer_kvnum = 5*10000;
    //从spill写入groupSpill的buffer，单位是byte
    uint64_t buffer_size_B = 1000*10000;
    uint64_t frequency_threshold = 200;

    group_by(input_path, base_path, count_min_num, count_min_length, bucket_num, sub_file_num,
             buffer_size_B, key_buffer_num, groupBy_buffer_kvnum, frequency_threshold);



    return 0;
}



