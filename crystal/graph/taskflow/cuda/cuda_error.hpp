#pragma once

#include <cuda.h>

#include "crystal/foundation/Exception.h"

#define TF_CUDA_REMOVE_FIRST_HELPER(N, ...) __VA_ARGS__
#define TF_CUDA_REMOVE_FIRST(...) TF_CUDA_REMOVE_FIRST_HELPER(__VA_ARGS__)
#define TF_CUDA_GET_FIRST_HELPER(N, ...) N
#define TF_CUDA_GET_FIRST(...) TF_CUDA_GET_FIRST_HELPER(__VA_ARGS__)

#define TF_CHECK_CUDA(...)                                      \
if(TF_CUDA_GET_FIRST(__VA_ARGS__) != cudaSuccess) {             \
  auto ev = TF_CUDA_GET_FIRST(__VA_ARGS__);                     \
  auto unknown_str  = "unknown error";                          \
  auto unknown_name = "cudaErrorUnknown";                       \
  auto error_str  = ::cudaGetErrorString(ev);                   \
  auto error_name = ::cudaGetErrorName(ev);                     \
  CRYSTAL_THROW(                                                \
      crystal::RuntimeError,                                    \
      (error_str  ? error_str  : unknown_str),                  \
      " (", (error_name ? error_name : unknown_name), ") - ",   \
      TF_CUDA_REMOVE_FIRST(__VA_ARGS__));                       \
}

