//
//  main.cpp
//  gpu-acceleration
//
//  Created by Srikar Yadlapati on 1/15/26.
//

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#include <chrono>
#include <iostream>
#include <vector>

#define COUNT 100'000'000 // scalable

// CPU-side hash RNG
inline uint hash(uint x) {
  x ^= x >> 16;
  x *= 0x7feb352d;
  x ^= x >> 15;
  x *= 0x846ca68b;
  x ^= x >> 16;
  return x;
}

inline float rand01(uint seed) { return float(hash(seed)) / float(0xffffffff); }

// Initialize array
std::vector<uint64_t> init_arr() {
  std::vector<uint64_t> arr(COUNT);
  for (uint64_t i = 0; i < COUNT; ++i)
    arr[i] = i + 1;
  return arr;
}

// CPU computation
void cpu_compute(const std::vector<uint64_t> &arr1,
                 const std::vector<uint64_t> &arr2) {
  auto start = std::chrono::steady_clock::now();
  std::vector<uint64_t> r(COUNT);

  for (uint64_t i = 0; i < COUNT; ++i)
    r[i] = uint64_t(float(arr1[i]) * float(arr2[i]) * rand01(i));

  auto end = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();
  std::cout << "CPU took " << ms << " ms\n";
}

// GPU computation
void gpu_compute(const std::vector<uint64_t> &arr1,
                 const std::vector<uint64_t> &arr2) {
  NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

  auto start = std::chrono::steady_clock::now();

  auto device = MTL::CreateSystemDefaultDevice();
  auto queue = device->newCommandQueue();

  NS::String *libPath =
      NS::String::string("shader.metallib", NS::UTF8StringEncoding);
  NS::Error *err;
  MTL::Library *lib = device->newLibrary(libPath, &err);
  NS::String *funcName =
      NS::String::string("addition_compute_function", NS::UTF8StringEncoding);
  auto *func = lib->newFunction(funcName);

  auto *state = device->newComputePipelineState(func, &err);

  auto buf1 = device->newBuffer(arr1.data(), sizeof(uint64_t) * COUNT,
                                MTL::ResourceStorageModeShared);
  auto buf2 = device->newBuffer(arr2.data(), sizeof(uint64_t) * COUNT,
                                MTL::ResourceStorageModeShared);
  auto resultBuffer = device->newBuffer(sizeof(uint64_t) * COUNT,
                                        MTL::ResourceStorageModeShared);

  auto *commandBuffer = queue->commandBuffer();
  auto *encoder = commandBuffer->computeCommandEncoder();
  encoder->setComputePipelineState(state);
  encoder->setBuffer(buf1, 0, 0);
  encoder->setBuffer(buf2, 0, 1);
  encoder->setBuffer(resultBuffer, 0, 2);

  auto threadsPerThreadgroup =
      MTL::Size(state->maxTotalThreadsPerThreadgroup(), 1, 1);
  auto threadsPerGrid = MTL::Size(COUNT, 1, 1);
  encoder->dispatchThreads(threadsPerGrid, threadsPerThreadgroup);

  encoder->endEncoding();
  commandBuffer->commit();
  commandBuffer->waitUntilCompleted();

  auto *results = reinterpret_cast<uint64_t *>(resultBuffer->contents());

  auto end = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();
  std::cout << "GPU took " << ms << " ms\n";

  pool->release();
}

int main() {
  auto arr1 = init_arr();
  auto arr2 = init_arr();

  gpu_compute(arr1, arr2);
  cpu_compute(arr1, arr2);

  return 0;
}
