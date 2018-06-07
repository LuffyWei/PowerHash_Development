//
// Created by weixun on 11/24/17.
//

#ifndef BHASHTEST6_0_MURMURHASH_H
#define BHASHTEST6_0_MURMURHASH_H
#include <iostream>
#include <vector>
#include <stdio.h>
#include <inttypes.h>
#include "murmurhash.h"
using namespace std;

uint64_t MurmurHash64A ( const void * key, int len, unsigned int seed ) {
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const uint64_t * data = (const uint64_t *)key;
    const uint64_t * end = data + (len/8);

    while(data != end)
    {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7)
    {
        case 7: h ^= uint64_t(data2[6]) << 48;
        case 6: h ^= uint64_t(data2[5]) << 40;
        case 5: h ^= uint64_t(data2[4]) << 32;
        case 4: h ^= uint64_t(data2[3]) << 24;
        case 3: h ^= uint64_t(data2[2]) << 16;
        case 2: h ^= uint64_t(data2[1]) << 8;
        case 1: h ^= uint64_t(data2[0]);
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}
string inttostring(int he){
    char t[256];
    string s;

    sprintf(t, "%d", he);
    s = t;
    return s;
}
vector<string> splitByMyChar_returnVectorWithoutDimension(string line,char mychar) {

    string::size_type index = line.find_first_of(mychar, 0);//找到空格、回车、换行等空白符
    if (index < line.size()) {
        vector<string> res ;//先new一个结果数组
        int i = 0;
        while (index != string::npos) {
            //      cout<<"放入list元素是"<<line.substr(0,index)<<endl;
            res.push_back(line.substr(0,index));
            //res[i] = line.substr(0, index);
            //  cout<<res[i]<<endl;
            //  feature_value.push_back(line.substr(0, index));
            line = line.substr(index + 1);
            //     cout<<"去掉一个dest_id剩余子串是："<<line<<endl;
            index = line.find_first_of(mychar, 0);
            //    cout<<"空格键下标是"<<index<<endl;
            ++i;
        }
        res.push_back(line);
        //cout<<res[i]<<endl;
        return res;
    } else {//给定字符串line中没有要分割的字符mychar,返回长度为1的源字符串
        vector<string> res;//先new一个结果double数组
        res.push_back(line.c_str());
        return res;
    }
}
#endif //BHASHTEST6_0_MURMURHASH_H
