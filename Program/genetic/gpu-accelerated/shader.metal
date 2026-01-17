
#include <metal_stdlib>
using namespace metal;

uint rand_xorshift(thread uint &rng_state) {
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

kernel void create_random_portfolios_batched(device float* arr [[buffer(0)]],
                                             uint id [[thread_position_in_grid]]) {
    const uint SIZE = 11; // same as host

    // Each thread is one portfolio
    uint rng_state = id;

    for (uint j = 0; j < SIZE; ++j) {
        // warm up RNG
        rand_xorshift(rng_state);
        rand_xorshift(rng_state);

        float f = float(rand_xorshift(rng_state)) * (1.0 / 4294967296.0);
        arr[id * SIZE + j] = f;
    }
}

