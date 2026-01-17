#include "gpu_util.h"
#include "../portfolio.h"
#include <chrono>
#include <cstring>
#include <iostream>

MTL::Device *create_device() { return MTL::CreateSystemDefaultDevice(); }
MTL::CommandQueue *create_queue(MTL::Device *device) {
  return device->newCommandQueue();
}

MTL::ComputePipelineState *create_state(const std::string &path,
                                        const std::string &func_name,
                                        MTL::Device *device) {
  NS::String *ns_path =
      NS::String::string(path.c_str(), NS::UTF8StringEncoding);
  NS::Error *err;

  MTL::Library *lib = device->newLibrary(ns_path, &err);

  NS::String *ns_func_name =
      NS::String::string(func_name.c_str(), NS::UTF8StringEncoding);

  auto *func = lib->newFunction(ns_func_name);
  return device->newComputePipelineState(func, &err);
}

int main() {
  auto start = std::chrono::system_clock::now();

  const i32 POPULATION = 100'000'000;
  const i16 SIZE = 11;

  auto *device = create_device();
  auto *queue = create_queue(device);
  auto *state = create_state("shader.metallib",
                             "create_random_portfolios_batched", device);

  // Allocate a single buffer to hold all portfolios
  auto bufferSize = sizeof(float) * SIZE * POPULATION;
  auto portfoliosBuffer =
      device->newBuffer(bufferSize, MTL::ResourceStorageModeShared);

  auto *commandBuffer = queue->commandBuffer();
  auto *encoder = commandBuffer->computeCommandEncoder();

  encoder->setComputePipelineState(state);
  encoder->setBuffer(portfoliosBuffer, 0, 0);

  // Launch one thread per portfolio
  uint maxThreads = state->maxTotalThreadsPerThreadgroup();
  auto threadsPerThreadGroup =
      MTL::Size(std::min<size_t>(POPULATION, maxThreads), 1, 1);
  auto threadsPerGrid = MTL::Size(POPULATION, 1, 1);

  encoder->dispatchThreads(threadsPerGrid, threadsPerThreadGroup);

  encoder->endEncoding();
  commandBuffer->commit();
  commandBuffer->waitUntilCompleted();

  // Access results if needed
  float *results = (float *)portfoliosBuffer->contents();
  // e.g., results[i * SIZE + j] is j-th float of i-th portfolio

  portfoliosBuffer->release();
  encoder->release();
  commandBuffer->release();
  state->release();
  queue->release();
  device->release();

  auto end = std::chrono::system_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();
  std::cout << "It took " << ms << " milliseconds\n";

  return 0;
}
