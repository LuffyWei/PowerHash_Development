//
// Created by weixun on 2017/11/24.
//

/**
 * bHash
 */
#include <cstdint>
#include <fstream>
#include "HashBuckets.h"
#include "murmurhash.h"

HashBuckets::HashBuckets(uint64_t bucket_num, uint64_t sub_file_num, uint64_t buffer_size_B,
                         int groupBy_buffer_kvnum) {
    this->bucket_num = bucket_num;
    this->sub_file_num = sub_file_num;
    this->buffer_size_B = buffer_size_B;
    this->current_size_B = 0;
    this->base = new base_node[bucket_num];
    this->kvBuffer = new KVBuffer(sub_file_num, groupBy_buffer_kvnum);
}

uint64_t HashBuckets::get_bucket_id(string k, const unsigned int seed) {
    uint64_t murmur_id = MurmurHash64A(k.c_str(), k.size(), seed);
    uint64_t bucket_id = murmur_id % bucket_num;
    return bucket_id;
}

uint64_t HashBuckets::get_file_id(uint64_t bucket_id) {
    uint64_t file_id;
    file_id = bucket_id/(bucket_num/sub_file_num);
    if(file_id == sub_file_num){
        file_id--;
    }
    return  file_id;
}

int HashBuckets::hashBuckets_fillSpill(string base_path, string k, string v, char split_kv) {
    unsigned int seed = 59;
    uint64_t bucket_id = get_bucket_id(k, seed);
    (base+bucket_id) ->fk_sum += (v.size()+1+k.size()+1);
//    base[bucket_id].fk_sum += (v.size()+1+k.size()+1);

    /****************insert process*****************/
    //insert first node
    if((base+bucket_id)->next == NULL){
        struct list_node * current = new list_node;
        current->k = k;
        current->fk_size = v.size()+1+k.size()+1;
        current->next = NULL;
        (base+bucket_id)->next = current;
    } else{
        list_node * pt = base[bucket_id].next;
        list_node * prev = pt;
        while(pt !=NULL && (pt->k.compare(k))!=0){
            prev = pt;
            pt= pt->next;
        }
        if(pt!=NULL && (pt->k.compare(k)==0)){
            pt->fk_size += (v.size()+1+k.size()+1);
            ///然后将当前节点从当前位置删除，插入到头节点处
            if(base[bucket_id].next != pt){
                prev->next = pt->next;
                pt->next = base[bucket_id].next;
                base[bucket_id].next = pt;
            }
        }
        else{
            struct list_node * current = new list_node;
            current->k = k;
            current->fk_size = v.size()+1+k.size()+1;
            ///头插法
            current->next = base[bucket_id].next;
            base[bucket_id].next = current;
        }
    }

    /*********************write to spills***********************/
    uint64_t file_no = get_file_id(bucket_id);
    kvBuffer->insertToBuffer(file_no, base_path, k, v, split_kv);

    return 0;
}

int HashBuckets::flush_reminder_kvBuffer(string base_path, char split_kv) {
    for(int i=0; i<sub_file_num; i++){
        kvBuffer->flushReminderToSpills(i, base_path, split_kv);
    }
    delete kvBuffer;
}

int HashBuckets::transvert_frequency_to_offset() {
    int range =(int)(bucket_num/sub_file_num);
    int j;

    for(int i=0; i<sub_file_num; i++){
        int beg_base = i*range;
        int end_base = (i+1)*range;
        uint64_t offset = 0;
        for(j=beg_base; j<end_base; j++){
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
        //说明bucket还没有遍历完，这种情况就是bucket_num/sub_file_num不能整除的情况
        //将剩余的bucket写入最后的一个文件
        if(i == sub_file_num-1 && j<= bucket_num-1){
            for(int s=j; s<bucket_num; s++){
                list_node * ptr = base[s].next;
                while (ptr != NULL){
                    uint64_t current_fk_size = ptr->fk_size;
                    ptr->fk_size = offset;
                    offset += current_fk_size;
                    ptr = ptr->next;
                }
                base[s].fk_sum = 0;
            }
        }
    }
    return  0;

}

int HashBuckets::grouping(string base_path, char split_kv, char split_vv) {

    for(int i=0; i<sub_file_num; i++){
        fstream in;
        in.open(base_path+"/spill/"+ inttostring(i).c_str(), ios::in);
        if(! in){
            cout<<"打开spill"<<i<<"文件出错"<<endl;
            exit(0);
        }
        string line;
        getline(in, line);
        while (! in.eof()){
            string::size_type index = line.find_first_of(split_kv, 0);
            string k = line.substr(0, index);
            string v = line.substr(index+1);
            if(buffer_full_or_not(v)){
                flushToGroupSpills(base_path, i, split_kv, split_vv);
                inset(k, v);
            } else{
                inset(k, v);
            }
            getline(in, line);
        }
        in.close();
        flushToGroupSpills(base_path, i, split_kv, split_vv);

    }
}

bool HashBuckets::buffer_full_or_not(string v) {
    if(current_size_B + v.size() > buffer_size_B){
        return true;
    }
    return false;
}

int HashBuckets::inset(string k, string v) {
    const unsigned int seed = 59;
    uint64_t bucket_id = get_bucket_id(k, seed);
    if((base + bucket_id)->next == NULL){
        cout<< k <<'\t'<< v <<"在offset表中没有对应的链"<<endl;
        exit(0);
    }
    list_node * ptr = base[bucket_id].next;
    while(ptr != NULL && (ptr->k.compare(k))!=0){
        ptr = ptr->next;
    }
    if(ptr != NULL && (ptr->k.compare(k))==0){
        ptr->listOfv.push_back(v);
        current_size_B += v.size();
        base[bucket_id].fk_sum += v.size();
    } else{

    }
    return 0;
}

int HashBuckets::flushToGroupSpills(string base_path, int file_no, char split_kv, char split_vv) {
    if(current_size_B == 0)
        return 0;
    ///先创建或打开groupSpills文件
    fstream out;
    out.open((base_path+"/groupSpill/"+inttostring(file_no)).c_str(), ios::out|ios::in);
    if(!out){
        //先创建文件
        ofstream create;
        create.open((base_path+"/groupSpill/"+inttostring(file_no)).c_str());
        create.close();
        //然后再打开
        out.open((base_path+"/groupSpill/"+inttostring(file_no)).c_str(), ios::out|ios::in);
        if(!out){
            cout<<"groupSpill"<<file_no<<"打不开"<<endl;
            exit(0);
        }
    }

    ///将buffer中的v写入文件
    int range = (int)(bucket_num/sub_file_num);
    int base_beg = file_no * range;
    int base_end;
    if(file_no != sub_file_num-1){
        base_end = (file_no+1)*range;
    } else{
        base_end = bucket_num;
    }
    //这个offset就是current_offset
    uint64_t currest_offset = 0;
    for(int i=base_beg; i<base_end; i++){
        if(base[i].fk_sum > 0){
            list_node * pt = base[i].next;
            while (pt != NULL){
                if(!pt->listOfv.empty()){
                    out.seekp(pt->fk_size-currest_offset, ios::cur);
                    for(int j=0; j<pt->listOfv.size(); j++){
                        out << pt->k << split_kv<< pt->listOfv[j]<<endl;
                        out.flush();
                        pt->fk_size += pt->k.size()+ 1 + pt->listOfv[j].size()+1;
                        base[i].fk_sum -= pt->listOfv[j].size();
                    }
                    currest_offset = pt->fk_size;
//                    pt->listOfv.clear();
                    vector<string> temp;
                    temp.swap(pt->listOfv);
                }
                pt = pt->next;
            }
            out.flush();
        }
        if(base[i].fk_sum > 0){
//            cout<<base[i].fk_sum<<endl;
            cout << "这条链清空后，base["<<i<<"].fk_sum仍大于0"<<endl;
            exit(0);
        }
    }
    out.close();
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
}
