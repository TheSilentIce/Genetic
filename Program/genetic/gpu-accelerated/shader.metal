#include <metal_stdlib>
using namespace metal;

inline uint hash(uint x) {
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

inline float rand01(uint seed) {
    return float(hash(seed)) / float(0xffffffff);
}

kernel void addition_compute_function(constant long *arr1        [[ buffer(0) ]],
                                      constant long *arr2        [[ buffer(1) ]],
                                      device   long *resultArray [[ buffer(2) ]],
                                               uint   index [[ thread_position_in_grid ]]) {
    
    float val = float(arr1[index]) * float(arr2[index]) * rand01(index);
    resultArray[index] = uint64_t(val);
}
