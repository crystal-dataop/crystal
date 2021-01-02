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

#include <string>

namespace crystal {

class TermColorWrapper {
 public:
  enum Color { kBlack, kRed, kGreen, kYellow, kBlue, kMagenta, kCyan, kWhite };

  TermColorWrapper() {}

  TermColorWrapper& color(Color c);
  TermColorWrapper& backgroundColor(Color c);
  TermColorWrapper& bold();
  TermColorWrapper& underline();

  std::string operator()(const std::string& s, bool end = true) const;

  static constexpr const char* end = "\x1b[0m";

 private:
  int color_{-1};
  int backgroundColor_{-1};
  bool bold_{false};
  bool underline_{false};
};

std::ostream& operator<<(std::ostream& out, const TermColorWrapper& c);

template <TermColorWrapper::Color c>
class TermColor : public TermColorWrapper {
 public:
  TermColor() {
    color(c);
  }
};

typedef TermColor<TermColorWrapper::kRed> Red;
typedef TermColor<TermColorWrapper::kGreen> Green;
typedef TermColor<TermColorWrapper::kYellow> Yellow;
typedef TermColor<TermColorWrapper::kBlue> Blue;
typedef TermColor<TermColorWrapper::kMagenta> Magenta;
typedef TermColor<TermColorWrapper::kCyan> Cyan;
typedef TermColor<TermColorWrapper::kWhite> White;

//////////////////////////////////////////////////////////////////////

inline TermColorWrapper& TermColorWrapper::color(Color c) {
  color_ = 30 + c;
  return *this;
}

inline TermColorWrapper& TermColorWrapper::backgroundColor(Color c) {
  backgroundColor_ = 40 + c;
  return *this;
}

inline TermColorWrapper& TermColorWrapper::bold() {
  bold_ = true;
  return *this;
}

inline TermColorWrapper& TermColorWrapper::underline() {
  underline_ = true;
  return *this;
}

inline std::ostream& operator<<(std::ostream& out, const TermColorWrapper& c) {
  out << c("", false);
  return out;
}

} // namespace crystal
