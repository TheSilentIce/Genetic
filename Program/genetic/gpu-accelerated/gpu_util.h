#ifndef GPU_UTIL_H
#define GPU_UTIL_H

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "../../types.h"
#include <Metal/Metal.hpp>
#include <string>

MTL::Device *create_device();
MTL::CommandQueue *create_queue(MTL::Device *device);

MTL::ComputePipelineState *create_state(const std::string &path,
                                        const std::string &func_name,
                                        MTL::Device *device);

float *fast_random_portfolio(i16 size, MTL::Device *device,
                             MTL::ComputePipelineState *state,
                             MTL::CommandQueue *queue);

#endif
