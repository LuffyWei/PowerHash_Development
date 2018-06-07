//
// Created by weixun on 18-4-16.
//
#include <string>
using namespace std;
#ifndef BHASHTEST9_0_HYPERLOGLOG_H
#define BHASHTEST9_0_HYPERLOGLOG_H


#define HLL_P 14 // The greater is P, the smaller the error.
#define HLL_Q (64-HLL_P) // The number of bits of the hash value used for determining the number of leading zeros.
#define HLL_REGISTERS (1<<HLL_P) // With P=14, 16384 registers.
#define HLL_P_MASK (HLL_REGISTERS-1) // Mask to index register.
#define HLL_ALPHA_INF 0.721347520444481703680

int hllPatLen(uint64_t key_hash, int* regp) ;
uint64_t  reviseHll(int *repg);

#endif //BHASHTEST9_0_HYPERLOGLOG_H
