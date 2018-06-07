
#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "HashBuckets.h"
#include "timer.h"
#include "Hyperloglog.h"

/**
 * 适用于所有数据集
 *
 */
using namespace std;


void group_by(string input_path, string base_path, int count_min_num, uint64_t count_min_length, uint64_t buffer_size_B,
              int key_buffer_num, int memory, int alpha, float beta){
    Timer timer;
    uint64_t getPara(uint64_t size_countMin, uint64_t size_keyBuffer, int memory,
                     uint64_t fixedSize, uint64_t totalGroup, int alpha);
    int sample[10] = {19, 23, 29, 31, 37, 41, 43 ,47, 53, 61};
    CountMin * countMin = new CountMin(count_min_num, count_min_length, key_buffer_num, sample);
    /***************读取源文件，同时生成hash表和spills文件*******************/
    ifstream in;
    in.open(input_path.c_str(), ios::in);
    if(!in){
        cout<<"Cannot open input file"<<endl;
        exit(0);
    }
    string line;
    getline(in, line);
    //----------------利用hyperloglog算法估算总的group数量，真特么准----------------------
    int *regp = new int[HLL_REGISTERS];
    for(int i=0; i<HLL_REGISTERS; i++){
        regp[i] = 0;
    }
    //---------------计算kv-pair的平均size---------------------------------------------
    uint64_t totalNum = 0;
    uint64_t totalSize = 0;
    string::size_type index = line.find_first_of('\t', 0);
    uint64_t keySize = line.substr(0, index).size();
    while (!in.eof()){
        totalNum++;
        totalSize += line.size();
        string::size_type index = line.find_first_of('\t', 0);
        keySize = index;
        countMin->add_key_fillSpill(base_path, line.substr(0, index), line.substr(index+1),'\t', regp);
        getline(in, line);
    }
    in.close();
    countMin->flush_reminder_keyBuffer(base_path);

    /***************************参数生成**************************/
    //---------------总的group数目----------------------
    uint64_t totalGroup = reviseHll(regp);
    //---------------kv-pair的平均size------------------
    uint64_t fixedSize = totalSize/totalNum;
    uint64_t offsetSize = keySize + 8;

//    cout<<"fixed size: "<<fixedSize<<endl;
//    cout<<"key size: "<<keySize<<endl;

    //---------------计算各个参数---------------------
    uint64_t size_countMin = count_min_num * count_min_length * 8;
    uint64_t size_keyBuffer = sizeof(key_vsize) * key_buffer_num;
//    cout<<size_countMin<<endl;
//    cout<<size_keyBuffer<<endl;
    uint64_t accumulate_num = getPara(size_countMin, size_keyBuffer, memory, fixedSize, totalGroup, alpha);
    uint64_t biggroup_num_cac = (memory*1000000-size_countMin-size_keyBuffer)/(4*sizeof(base_node)/3+sizeof(list_node)) - accumulate_num;
    uint64_t smallgroup_num;
    uint64_t bucket_num_small;
    uint64_t frequency_threshold;
    if(biggroup_num_cac > totalGroup){
        biggroup_num_cac = totalGroup;
        smallgroup_num = 0;
        bucket_num_small = 0;
        frequency_threshold = 0;
    } else{
        smallgroup_num = totalGroup-biggroup_num_cac;
        bucket_num_small =(smallgroup_num / accumulate_num)*4/3;
        //---------------计算threshold---------------------
        /*
         * 知道了biggroup_num:smallgroup_num的比例beta，就可以对count-min中任意一组进行排序，选择排序后beta这个地方的rough size作为threshold
         */
        double percent = (double)biggroup_num_cac / totalGroup;
        cout<<"percent  "<<percent<<endl;
        int indexOfTh = (int)(count_min_length*percent);
        cout<<"indexOfTh  "<<indexOfTh<<endl;
        uint64_t *sample_countMin = new uint64_t[count_min_length];
        memcpy(sample_countMin, countMin->count_min_arrays[0], count_min_length*sizeof(uint64_t));
        sort(sample_countMin, sample_countMin+count_min_length);
        frequency_threshold = (uint64_t)(sample_countMin[count_min_length -indexOfTh] * beta);
        delete [] sample_countMin;
        sample_countMin = NULL;
    }

    cout<<"group total num: "<<totalGroup<<endl;
    cout<<"biggroup_num: "<<biggroup_num_cac<<endl;
    cout<<"smallgroup_num: "<<smallgroup_num<<endl;
    cout<<"accumulate_num: "<<accumulate_num<<endl;
    cout<<"bucket_num_small "<<bucket_num_small<<endl;
    cout<<"frequency_threshold  "<<frequency_threshold<<endl;
    cout<<"phase 1: "<<timer.elapsed()<<endl;

    /********************读取key文件，然后填充hash表*********************************/
    int64_t  biggroup_num = 4*biggroup_num_cac/3;
    HashBuckets * hashBuckets = new HashBuckets(biggroup_num, buffer_size_B);
    Accumulate * accumulate = new Accumulate(biggroup_num, accumulate_num, bucket_num_small);
    in.open((base_path+"/key").c_str(), ios::in|ios::binary);
    if(!in){
        cout<<"Cannot open key_file"<<endl;
        exit(0);
    }

    key_vsize *kBuffer = new struct key_vsize[key_buffer_num];
    memset(kBuffer, 0, key_buffer_num*sizeof(key_vsize));
    while (!in.eof()){
        in.read((char*)kBuffer, key_buffer_num*sizeof(key_vsize));
        for(int i=0; i<key_buffer_num; i++){
            if(kBuffer[i].v_size == 0){
                break;
            }
            string k = kBuffer[i].k;
            hashBuckets->fill_hash_buckets(countMin, accumulate, k, kBuffer[i].v_size, frequency_threshold);
        }
        memset(kBuffer, 0, key_buffer_num*sizeof(key_vsize));
    }
    in.close();
    delete[] kBuffer;
    kBuffer=NULL;
    delete countMin;
//    cout<<"hashBuckets装填: "<<timer.elapsed()<<" || ";

    /***************将hash表转换成offset表********************************/
    hashBuckets->transvert_frequency_to_offset(accumulate);
    cout<<"phrase 2: "<<timer.elapsed()<<endl;
    /***
     * 只需要记录每个accumulate的起始地址就行，
     * 因为进行offset移动以后，每个offset指针是当前hash accumulate的终止地址，又是下一个hash accumulate的起始地址
     */
    uint64_t old_accumulate ;
    old_accumulate = accumulate->accumulate[0].fk_size;

//    cout<<"记录accumulate起始地址"<<endl;

    /****************大group进行groupBy************************************/
    hashBuckets->grouping(accumulate, input_path, base_path, '\t', '\n');
    cout<<"phase 3.1: "<<timer.elapsed()<<" || ";
    delete hashBuckets;

    /********************小group进行groupBy*****************************/
    for(int j=accumulate_num-1; j>0; j--){
        accumulate->accumulate[j].fk_size = accumulate->accumulate[j-1].fk_size;
    }
    accumulate->accumulate[0].fk_size = old_accumulate;

    accumulate->group_accumulate(base_path, '\t');
    cout<<"phase 3.2: "<<timer.elapsed()<<endl;
    delete accumulate;

    /*****************删除spills文件************************************/
    if(remove((base_path+"/key").c_str())) cout<<"删除key_file文件失败"<<endl;
    if(remove((base_path+"/spill").c_str())) cout<<"删除spill文件失败"<<endl;
    if(remove((base_path+"/groupSpill").c_str()))cout<<"删除groupSpill失败"<<endl;

}

int main(int argc, char** argv) {
    string default_path = "/home/weixun/KV data/pareto_10000000";
//    string default_path = "/home/weixun/KV data/wiki";
    string data_name = "normal_10000000";
    string input_path = "/home/weixun/KV data/";
    string base_path = "/home/weixun/CLionProjects/bHashTest11.1/data";
    //count-min的三个参数
    int count_min_num = 3;
    uint64_t count_min_length = 250000;

    //将key写入key_file的buffer，单位是个
    int key_buffer_num = 100*1000;

    //从spill写入groupSpill的buffer，单位是byte.
    uint64_t buffer_size_B = 1*1000*1000;
    int memory = 50;
    int alpha = 5;
    float beta = 1;
    if(argc < 9){
        input_path = default_path;
        cout<<"使用默认参数"<<endl;
    } else{
        data_name = argv[1];
        input_path = input_path+data_name;
        base_path = argv[2];
        count_min_num = atoi(argv[3]);
        count_min_length = atoi(argv[4]);
        key_buffer_num = atoi(argv[5])*1000;
        buffer_size_B = atoi(argv[6])*1000;
        memory = atoi(argv[7]);
        alpha = atoi(argv[8]);
        beta = atof(argv[9]);
    }

    cout<<data_name<<" "<<count_min_num << " "<< count_min_length<<" "<<key_buffer_num<<" "
       <<buffer_size_B << " "<<memory<<" "<<alpha<<" "<<beta<<endl;
    group_by(input_path, base_path, count_min_num, count_min_length, buffer_size_B, key_buffer_num, memory, alpha, beta);

    return 0;
}

uint64_t getPara(uint64_t size_countMin, uint64_t size_keyBuffer, int memory,
            uint64_t fixedSize, uint64_t totalGroup, int alpha){

    uint64_t size_phase2 = memory*1000000-size_countMin-size_keyBuffer;
    //-------------------求以P作为未知数的一元二次方程-------------------
    uint64_t accumulate_num;
    long long a = sizeof(list_node);
    long long b = sizeof(list_node) + 4*sizeof(base_node)/3 + alpha * fixedSize - memory*1000000;
    long long c = (sizeof(list_node) + 4*sizeof(base_node)/3 + alpha * fixedSize) * (totalGroup-size_phase2/(4*sizeof(base_node)/3+sizeof(list_node)));
    accumulate_num = (uint64_t)((-b - pow((b*b-4*a*c) , 0.5))/(2*a));
    if(accumulate_num<=0 || accumulate_num>=UINT64_MAX){
        return 1;
    } else{
        return accumulate_num;
    }

    //-------------------求以B作为未知数的一元二次方程-------------------
//    int biggroup_num;
//    long long a = offsetSize;
//    long long b = memory*1000000-alpha*fixedSize-2*size_phase2;
//    long long c = (size_phase2*size_phase2-memory*1000000*size_phase2)/offsetSize+alpha*fixedSize*totalGroup;
////    cout<<a<<" || "<<b<<" || "<<c<<endl;
//    biggroup_num =(int)((-b + pow((b*b-4*a*c) , 0.5))/(2*a));
//    return biggroup_num;
}


