//
// Created by weixun on 2017/11/24.
//

#include <cstdint>
#include <fstream>
#include "HashBuckets.h"
#include "murmurhash.h"
#include "Accumulate.h"

HashBuckets::HashBuckets(uint64_t biggroup_num,  uint64_t buffer_size_B) {
    this->bucket_num = biggroup_num;
    this->buffer_size_B = buffer_size_B;
    this->current_size_B = 0;
    this->base = new base_node[bucket_num];
//    cout<<accumulate_num<<" hashBuckets 初始化完成"<<endl;
}

uint64_t HashBuckets::get_bucket_id(string k, const unsigned int seed) {
    uint64_t murmur_id = MurmurHash64A(k.c_str(), k.size(), seed);
    uint64_t bucket_id = murmur_id % bucket_num;
    return bucket_id;
}
uint64_t HashBuckets::get_bucket_id(uint64_t murmur_id) {
    uint64_t bucket_id = murmur_id % bucket_num;
    return bucket_id;
}


///无需再重新计算countmin中的frequency,相对4.0版本快乐不少，而且更准
int HashBuckets::fill_hash_buckets(CountMin*countMin, Accumulate*accumulate, string k, int vsize, uint64_t frequency_threshold) {
    const unsigned int seed = 59;
    uint64_t frequency = countMin->get_frequency(k,seed);
    uint64_t murmur_id = MurmurHash64A(k.c_str(),k.size(), seed);
    uint64_t bucket_id = get_bucket_id(murmur_id);
    if(frequency < frequency_threshold){
        accumulate->fill_accumulate(k, vsize, murmur_id, bucket_id);
    } else{
        (base+bucket_id) ->fk_sum += (k.size()+1+vsize+1);
        /****************insert process*****************/
        //insert first node
        if((base+bucket_id)->next == NULL){
            struct list_node * current = new list_node;
            current->k = k;
            current->fk_size = (k.size()+1+vsize+1);
            current->next = NULL;
            (base+bucket_id)->next = current;
        } else{
            list_node * pt = base[bucket_id].next;
            list_node * pre = pt;
            while(pt !=NULL && (pt->k.compare(k))!=0){
                pre = pt;
                pt= pt->next;
            }
            if(pt !=NULL && (pt->k.compare(k))==0){
                pt->fk_size += (k.size()+1+vsize+1);
                if(base[bucket_id].next != pt){
                    pre->next = pt->next;
                    pt->next = base[bucket_id].next;
                    base[bucket_id].next = pt;
                }
            } else{
                struct list_node * current = new list_node;
                current->k = k;
                current->fk_size = (k.size()+1+vsize+1);
                //头插法
                current->next = base[bucket_id].next;
                base[bucket_id].next = current;
            }

        }
    }

    return 0;
}

int HashBuckets::transvert_frequency_to_offset(Accumulate*accumulate) {

    uint64_t offset = 0;
    for(int j=0; j<bucket_num; j++){
        list_node * ptr = base[j].next;
        while(ptr != NULL){
            uint64_t current_fk_size = ptr->fk_size;
            ptr->fk_size = offset;
            offset += current_fk_size;
            ptr = ptr->next;
        }
            //用于存放后来v的大小
        base[j].fk_sum = 0;
    }
    accumulate->transvert_accumulate();

    return  0;
}

int HashBuckets::grouping(Accumulate*accumulate, string input_path, string base_path, char split_kv, char split_vv) {

    fstream in;
    in.open(input_path.c_str(), ios::in);
    if(! in){
        cout<<"打开输入文件出错"<<endl;
        exit(0);
    }
    string line;
    getline(in, line);
    while (! in.eof()){
        string::size_type index = line.find_first_of(split_kv, 0);
        string k = line.substr(0, index);
        string v = line.substr(index+1);
        if(buffer_full_or_not(v)){
            flushToGroupSpills(accumulate, base_path, split_kv, split_vv);
            inset(accumulate, k, v);
        } else{
            inset(accumulate, k, v);
        }
        getline(in, line);
    }
    in.close();
    flushToGroupSpills(accumulate, base_path, split_kv, split_vv);

}

bool HashBuckets::buffer_full_or_not(string v) {
    if(current_size_B + v.size() >= buffer_size_B){
        return true;
    }
    return false;
}

int HashBuckets::inset(Accumulate*accumulate, string k, string v) {
    const unsigned int seed = 59;
    uint64_t murmur_id = MurmurHash64A(k.c_str(), k.size(), seed);
    uint64_t bucket_id = get_bucket_id(murmur_id);
    //所在的链为空
    if((base + bucket_id)->next == NULL){
        accumulate->insert_accumulate(k, v, murmur_id, bucket_id);
        current_size_B += (k.size()+v.size());
    } else{
        list_node * ptr = base[bucket_id].next;
        while(ptr != NULL && (ptr->k.compare(k))!=0){
            ptr = ptr->next;
        }
        if(ptr != NULL && (ptr->k.compare(k))==0){
            ptr->listOfv.push_back(v);
            current_size_B += v.size();
            base[bucket_id].fk_sum += v.size();
        } else{
            accumulate->insert_accumulate(k, v, murmur_id, bucket_id);
            current_size_B += (k.size()+v.size());
        }
    }
    return 0;
}

int HashBuckets::flushToGroupSpills(Accumulate*accumulate, string base_path, char split_kv, char split_vv) {
    if(current_size_B == 0)
        return 0;
    ///先创建或打开groupSpills文件
    fstream out;
    out.open((base_path+"/groupSpill").c_str(), ios::out|ios::in);
    if(!out){
        //先创建文件
        ofstream create;
        create.open((base_path+"/groupSpill").c_str());
        create.close();
        //然后再打开
        out.open((base_path+"/groupSpill").c_str(), ios::out|ios::in);
        if(!out){
            cout<<"groupSpill"<<" can not open 1"<<endl;
            exit(0);
        }
    }

    ///将buffer中的v写入文件
    uint64_t current_offset = 0;
    for(int i=0; i<bucket_num; i++){
        if(base[i].fk_sum > 0){
            list_node * pt = base[i].next;
            while (pt != NULL){
                if(!pt->listOfv.empty()){
                    out.seekp(pt->fk_size-current_offset, ios::cur);
                    for(int j=0; j<pt->listOfv.size(); j++){
                        out << pt->k << split_kv<< pt->listOfv[j]<<endl;
//                        out << pt->k << split_kv<< pt->listOfv[j]<<split_vv;
                        out.flush();
                        pt->fk_size += (pt->k.size()+ 1 + pt->listOfv[j].size()+1);
                        base[i].fk_sum -= pt->listOfv[j].size();
                    }
                    current_offset = pt->fk_size;
//                    pt->listOfv.clear();
                    vector<string> temp;
                    temp.swap(pt->listOfv);
                }
                pt = pt->next;

            }
            out.flush();
        }
        if(base[i].fk_sum > 0){
            cout << "这条链清空后，base["<<i<<"].fk_sum仍大于0"<<endl;
            exit(0);
        }
    }
    out.flush();
    out.close();

    accumulate->flush_accumulate(base_path, split_kv, split_vv);
    current_size_B = 0;

    return 0;
}

HashBuckets::~HashBuckets() {
    for(int i=0;i<bucket_num;i++){//遍历每一个桶
        list_node * current = base[i].next;
        while(current!=NULL){//当下一个不是空节点时
            list_node * next = current->next;
            delete  current;
            current = next;
        }
    }
    delete[] base;
    base = NULL;

}
