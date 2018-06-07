//
// Created by weixun on 18-1-2.
//

#include "Accumulate.h"
#include "murmurhash.h"
#include <iostream>
#include <fstream>
using namespace std;
string inttostring_acc(int file_no){
    char t[256];
    string s;

    sprintf(t, "%d", file_no);
    s = t;
    return s;
}
Accumulate::Accumulate(int accumulate_num, uint64_t bucket_num_small) {
    this->accumulate_num = accumulate_num;
    this->bucket_num_small = bucket_num_small;
    if(accumulate_num ==0){
        this->kvBuffer = NULL;
    } else{
        this->kvBuffer = new KVBuffer(accumulate_num);
    }
    this->small_group_num = 0;
}

uint64_t Accumulate::get_small_bucket_id(uint64_t murmur_id) {
    uint64_t small_bucket_id = murmur_id % bucket_num_small;
    return small_bucket_id;
}


int Accumulate::insert_accumulate( string k, string v, char split_kv, uint64_t murmur_id) {
    if(kvBuffer == NULL){
        return 0;
    }
    int sub_file_id = murmur_id % accumulate_num;
    kvBuffer->insertToBuffer(sub_file_id, k, v ,split_kv);
    return 0;
}

int Accumulate::flush_accumulate(string base_path, char split_kv, char split_vv) {
//    cout<<"enter the acc"<<endl;
    if(kvBuffer == NULL){
        return 0;
    }
    kvBuffer->flushToSpills(base_path);
    return 0;
}

int Accumulate::group_accumulate(string base_path, char split_kv) {
    if(kvBuffer == NULL){
        return 0;
    }
    for(int j=0; j<accumulate_num; j++){
        ifstream in;
        in.open((base_path+"/"+inttostring_acc(j)).c_str(), ios::in);
        if(!in){
            cout<<"groupSpill can not open 2"<<endl;
            exit(0);
        }
        string line;
        getline(in, line);
        base_node * base_small = new struct base_node[bucket_num_small];
        while (!in.eof()){
            string::size_type index = line.find_first_of(split_kv, 0);
            groupBy_small(base_small, line.substr(0, index), line.substr(index+1));
            getline(in, line);
        }
        in.close();

        fstream out;
        out.open((base_path+"/groupSpill").c_str(), ios::app|ios::out);
        for(int i=0; i<bucket_num_small; i++){
            if(base_small[i].fk_sum > 0){
                list_node * pt = base_small[i].next;
                while(pt != NULL){
                    if(!pt->listOfv.empty()){
                        for(int j=0; j<pt->listOfv.size(); j++){
                            out<<pt->k<<split_kv<<pt->listOfv[j]<<endl;
                            /**
                             * 这里的out.flush()是必要的，因为c++本身写往外存的时候本身就有缓存机制，
                             * 在测量所需内存时，如果不及时向外存刷新，就会将部分数据存往c++自带缓存，
                             * 使测量不准
                             */
                            out.flush();
                            base_small[i].fk_sum -= pt->listOfv[j].size();
                        }
                        vector<string> tmp;
                        tmp.swap(pt->listOfv);
                    }
                    pt = pt->next;
                }
            }
            out.flush();
            if(base_small[i].fk_sum > 0){
                cout << "这条链清空后，small base_small["<<i<<"].fk_sum仍大于0"<<endl;
                exit(0);
            }
        }
        out.flush();
        out.close();
//            cout<<"group一组"<<endl;
        for(int i=0;i<bucket_num_small;i++){//遍历每一个桶
            list_node * current = base_small[i].next;
            while(current!=NULL){//当下一个不是空节点时
                list_node * next = current->next;
                delete  current;
                current = next;
            }
        }
        delete[] base_small;
        base_small = NULL;
    }

    return 0;
}

int Accumulate::groupBy_small(base_node * base_small , string k, string v) {
//    cout<<k<<'\t'<<v<<endl;
    const unsigned int seed = 17;
    uint64_t murmur_id = MurmurHash64A(k.c_str(), k.size(), seed);
    uint64_t small_bucket_id = get_small_bucket_id(murmur_id);
//    cout<<"small_bucket_id: "<<small_bucket_id<<endl;
    base_small[small_bucket_id].fk_sum += v.size();
    if((base_small+small_bucket_id)->next == NULL){
        small_group_num ++;
        struct list_node * current = new list_node;
        current->k = k;
        current->listOfv.push_back(v);
        current->next = NULL;
        (base_small+small_bucket_id)->next = current;
    } else{
        list_node * pt = (base_small+small_bucket_id)->next;;
        while(pt != NULL && (pt->k.compare(k))!= 0){
            pt = pt->next;
        }
        if(pt != NULL && (pt->k.compare(k)) == 0){
            pt->listOfv.push_back(v);

        } else{
            small_group_num++;
            struct list_node * current = new list_node;
            current->next = base_small[small_bucket_id].next;
            base_small[small_bucket_id].next = current;
            current->k = k;
            current->listOfv.push_back(v);
        }
    }
//    cout<<"插入完成"<<endl;
    return 0;
}


Accumulate::~Accumulate() {

    delete kvBuffer;

}