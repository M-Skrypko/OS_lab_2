#include "test_OS_lab_2.h"

extern "C" __declspec(dllexport) double CalculateAverage(const int* arr, int size) {
    if (size <= 0 || arr == nullptr) {
        return 0.0;
    }

    double sum = 0.0;
    for (int i = 0; i < size; ++i) {
        sum += arr[i];
    }
    return sum / size;
}