
#include <iostream>
#include <fstream>
#include <cstring>

#include "HashBuckets.h"
#include "CountMin.h"
#include "timer.h"

using namespace std;

string inttostring_main(int file_no){
    char t[256];
    string s;

    sprintf(t, "%d", file_no);
    s = t;
    return s;
}
void group_by(string input_path, string base_path, int count_min_num, uint64_t count_min_length,
              uint64_t bucket_num, int sub_file_num, int accumulate_num, uint64_t buffer_size_B,
              int key_buffer_num, int KV_buffer_num, uint64_t frequency_threshold){
    Timer timer;
    cout<<"start"<<endl;
    CountMin * countMin = new CountMin(count_min_num, count_min_length, sub_file_num, bucket_num, key_buffer_num, KV_buffer_num);
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
    HashBuckets * hashBuckets = new HashBuckets(bucket_num, sub_file_num, accumulate_num, buffer_size_B);
    cout<<"hashBuckets初始化完成！"<<endl;
    in.open((base_path+"/key").c_str(), ios::in|ios::binary);
    if(!in){
        cout<<"Cannot open key_file"<<endl;
        exit(0);
    }
    fstream out_1;
    string out_1_path = "/home/weixun/CLionProjects/bHashTest7.0/data/key_s";
    key_vsize *kBuffer = new struct key_vsize[key_buffer_num];
    memset(kBuffer, 0, key_buffer_num*sizeof(key_vsize));
    while (!in.eof()){
        in.read((char*)kBuffer, key_buffer_num*sizeof(key_vsize));
//        for(int i=0; i<key_buffer_num; i++){
//            cout<<kBuffer[i].k<<'\t'<<kBuffer[i].v_size<<endl;
//        }
        out_1.open(out_1_path.c_str(), ios::app|ios::ate|ios::binary);
        for(int i=0; i<key_buffer_num; i++){
            if(kBuffer[i].v_size == 0){
                break;
            }
            hashBuckets->fill_hash_buckets(countMin, kBuffer[i].k, kBuffer[i].v_size, frequency_threshold);
            out_1<<kBuffer[i].k<<'\t'<<kBuffer[i].v_size<<endl;
        }
        memset(kBuffer, 0, key_buffer_num*sizeof(key_vsize));
        out_1.close();
//        cout<<"完成一批"<<endl;
    }
    in.close();
    delete[] kBuffer;
    kBuffer=NULL;
    delete countMin;
    cout<<"hashBuckets装填完毕"<<endl;

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
//        if(remove((base_path+"/groupSpill/"+inttostring_main(i)).c_str()))
//            cout<<"删除groupSpill"<<i<<"失败"<<endl;
    }

}

int main(int argc, char** argv) {
    string default_path = "/home/weixun/KV data/wiki";
    string input_path = "/home/weixun/KV data/";
    string base_path = "/home/weixun/CLionProjects/bHashTest7.0/data";
    //count-min的三个参数
    int count_min_num = 3;
    uint64_t count_min_length = 10000000;
    uint64_t bucket_num = 1000000;
    //子文件个数
    int sub_file_num = 5;
    //将key写入key_file的buffer，单位是个
    int key_buffer_num = 1000;
    //将数据写入spill的buffer，单位是个
    int KV_buffer_num = 5*10000;
    //从spill写入groupSpill的buffer，单位是byte.
    uint64_t buffer_size_B = 1000*10000;
    uint64_t frequency_threshold = 20;
    int accumulate_num = 1000;
    if(argc < 11){
        input_path = default_path;
        cout<<"使用默认参数"<<endl;
    } else{
        string data_name = argv[1];
        input_path = input_path+data_name;
        base_path = argv[2];
        count_min_num = atoi(argv[3]);
        count_min_length = atoi(argv[4]);
        bucket_num = atoi(argv[5]);
        sub_file_num = atoi(argv[6]);
        key_buffer_num = atoi(argv[7])*10000;
        KV_buffer_num = atoi(argv[8])*10000;
        buffer_size_B = atoi(argv[9])*10000;
        frequency_threshold = atoi(argv[10]);
        accumulate_num = atoi(argv[11]);
    }
    group_by(input_path, base_path, count_min_num, count_min_length, bucket_num, sub_file_num, accumulate_num,
             buffer_size_B, key_buffer_num, KV_buffer_num, frequency_threshold);



    return 0;
}



