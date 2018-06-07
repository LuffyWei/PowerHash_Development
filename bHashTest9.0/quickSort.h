//
// Created by weixun on 18-4-17.
//

#ifndef BHASHTEST9_0_QUICKSORT_H
#define BHASHTEST9_0_QUICKSORT_H


#include <cstdint>
#include <algorithm>
#include <vector>

using namespace std;
void swap(uint64_t a, uint64_t b){
    uint64_t tmp = a;
    a = b;
    b = tmp;
}
int partition(uint64_t *data, int low, int up)
{
    int pivot = data[up];
    int i = low-1;
    for (int j = low; j < up; j++)
    {
        if(data[j] <= pivot)
        {
            i++;
            swap(data[i], data[j]);
        }
    }
    swap(data[i+1], data[up]);
    return i+1;
}
void quickSort(uint64_t *data, int low, int up)
{
    if(low < up)
    {
        int mid = partition(data, low, up);
        //Watch out! The mid position is on the place, so we don't need to consider it again.
        //That's why below is mid-1, not mid! Otherwise it will occur overflow error!!!
        quickSort(data, low, mid-1);
        quickSort(data, mid+1, up);
    }
}
#endif //BHASHTEST9_0_QUICKSORT_H
