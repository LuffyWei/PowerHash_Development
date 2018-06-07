
#include <iostream>
#include <fstream>

#include "HashBuckets.h"
#include "timer.h"

using namespace std;

string inttostring_main(uint64_t file_no){
    char t[256];
    string s;

    sprintf(t, "%d", file_no);
    s = t;
    return s;
}
void group_by(string input_path, string base_path, uint64_t bucket_num, uint64_t sub_file_num,
              uint64_t buffer_size_B, uint64_t groupBy_buffer_kvnum){
    Timer timer;
    HashBuckets * hashBuckets = new HashBuckets(bucket_num, sub_file_num, buffer_size_B, groupBy_buffer_kvnum);
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
        hashBuckets->hashBuckets_fillSpill(base_path, line.substr(0, index), line.substr(index+1), '\t');
        getline(in, line);
    }
    in.close();
    hashBuckets->flush_reminder_kvBuffer(base_path, '\t');

    /***************将hash表转换成offset表********************************/
    hashBuckets->transvert_frequency_to_offset();
    cout<<"phase1:"<<timer.elapsed()<<endl;
    /****************完成group-by操作************************************/
    hashBuckets->grouping(base_path, '\t', '\n');
    cout<<"total time:"<<timer.elapsed()<<endl;
    delete hashBuckets;
    /*****************删除spills文件************************************/
    for(int i=0; i<sub_file_num; i++){
        if(remove((base_path+"/spill/"+inttostring_main(i)).c_str()))
            cout<<"删除spill"<<i<<"失败"<<endl;
    }

}

int main(int argc, char** argv) {
    string input_path = "/home/weixun/KV data/pareto_10000000";
    string base_path = "/home/weixun/CLionProjects/bHashTest1.0/data";
    uint64_t bucket_num = 1000000;
    uint64_t sub_file_num = 5;
    uint64_t buffer_size_B = 1000*1000000;
    int groupBy_buffer_kvnum = 5*10000;
    group_by(input_path, base_path, bucket_num, sub_file_num, buffer_size_B, groupBy_buffer_kvnum);



    return 0;
}



