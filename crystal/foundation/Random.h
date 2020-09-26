/*
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstdint>
#include <random>

namespace crystal {

class Random {
 public:
  class Generator {
   public:
    using result_type = uint32_t;

    result_type operator()();

    static constexpr result_type min() {
      return std::numeric_limits<result_type>::min();
    }

    static constexpr result_type max() {
      return std::numeric_limits<result_type>::max();
    }
  };

  static uint32_t rand32() {
    return Generator()();
  }

  static uint32_t rand32(uint32_t max) {
    return rand32(0, max);
  }

  static uint32_t rand32(uint32_t min, uint32_t max) {
    if (min == max) {
      return 0;
    }
    Generator gen;
    return std::uniform_int_distribution<uint32_t>(min, max - 1)(gen);
  }

  static uint64_t rand64() {
    Generator gen;
    return ((uint64_t)gen() << 32) | gen();
  }

  static uint64_t rand64(uint64_t max) {
    return rand64(0, max);
  }

  static uint64_t rand64(uint64_t min, uint64_t max) {
    if (min == max) {
      return 0;
    }
    Generator gen;
    return std::uniform_int_distribution<uint64_t>(min, max - 1)(gen);
  }

  static bool oneIn(uint32_t n) {
    return n != 0 && rand32(0, n) == 0;
  }

  static double randDouble01() {
    Generator gen;
    return std::generate_canonical<double, std::numeric_limits<double>::digits>(
        gen);
  }

  static double randDouble(double min, double max) {
    if (std::fabs(max - min) < std::numeric_limits<double>::epsilon()) {
      return 0;
    }
    Generator gen;
    return std::uniform_real_distribution<double>(min, max)(gen);
  }
};

} // namespace crystal
