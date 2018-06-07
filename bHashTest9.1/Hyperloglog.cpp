//
// Created by weixun on 18-4-16.
//

#include <iostream>
#include <cmath>
#include "Hyperloglog.h"

using namespace std;



int hllPatLen(uint64_t hash, int *regp) {
    uint64_t bit, index;
    int count;
    index = hash & HLL_P_MASK; /* Register index. */
    hash >>= HLL_P; /* Remove bits used to address the register. */
    hash |= ((uint64_t)1<<HLL_Q); /* Make sure the loop terminates and count will be <= Q+1. */
    bit = 1;
    count = 1; /* Initialized to 1 since we count the "00000...1" pattern. */
    while((hash & bit) == 0) {
        count++;
        bit <<= 1;
    }
    int old_count = regp[index];
    regp[index] = max(count, old_count);
    return count;
}

int reviseHll(int *repg) {
    double alpha = HLL_ALPHA_INF / (1 + 1.079 / HLL_REGISTERS) * HLL_REGISTERS * HLL_REGISTERS;
    int zero; //register中0的个数，算法伪代码中的V
    double registerSum;
    double estimate;   //初步估计的结果，算法伪代码中的E
    for (int i = 0; i < HLL_REGISTERS; i++) {
        registerSum += 1.0 / (pow(2, repg[i]));
        if (repg[i] == 0) {
            zero++;
        }
    }
    estimate = (uint64_t) (alpha / registerSum);
    uint64_t tmp = pow(2, 32);
    if (estimate <= 2.5 * HLL_REGISTERS) {
//        cout << "case 1" << endl;
        return (uint64_t) (HLL_REGISTERS * log(HLL_REGISTERS / zero));
    } else if (estimate > tmp / 30) {
//        cout << "case 2" << endl;
        return (uint64_t) (-tmp * log(1 - estimate / tmp));
    } else {
//        cout << "case 3" << endl;
        return estimate;
    }
}