//
// Created by weixun on 11/24/17.
//

#ifndef BHASHTEST9_0_MURMURHASH_H
#define BHASHTEST9_0_MURMURHASH_H
#include <iostream>
#include <vector>
#include <stdio.h>
#include <inttypes.h>
using namespace std;

uint64_t MurmurHash64A ( const void * key, int len, unsigned int seed );
string inttostring(int he);
vector<string> splitByMyChar_returnVectorWithoutDimension(string line,char mychar) ;

#endif //BHASHTEST9_0_MURMURHASH_H