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

#include "crystal/operator/generic/Slice.h"

#include "crystal/foundation/BitMask.h"
#include "crystal/serializer/DynamicEncoding.h"

namespace crystal {
namespace op {

DataView& Slice::compose(DataView& view) const {
  U32IndexArray index;
  if (reverse_) {
    size_t b = std::min(i_, view.docIndex().size());
    size_t e = view.docIndex().size();
    for (size_t k = 0; k < b; k++) {
      index.push_back(view.docIndex()[k]);
    }
    for (size_t k = std::max(b, j_); k < e; k++) {
      index.push_back(view.docIndex()[k]);
    }
  } else {
    size_t e = std::min(j_, view.docIndex().size());
    for (size_t k = i_; k < e; k++) {
      index.push_back(view.docIndex()[k]);
    }
  }
  view.docIndex().swap(index);
  return view;
}

dynamic Slice::toDynamic() const {
  return dynamic::object
    ("Slice", dynamic::object
     ("i", i_)
     ("j", j_)
     ("reverse", reverse_));
}

DataView& SliceV::compose(DataView& view) const {
  U32IndexArray index;
  size_t e = view.docIndex().size();
  if (reverse_) {
    uint8_t masks[div8(e)];
    BitMask b(masks);
    b.clear(sizeof(masks));
    for (auto& i : v_) {
      if (i < e) {
        b.set(i);
      }
    }
    for (size_t i = 0; i < e; i++) {
      if (!b.isSet(i)) {
        index.push_back(view.docIndex()[i]);
      }
    }
  } else {
    for (auto& i : v_) {
      if (i < e) {
        index.push_back(view.docIndex()[i]);
      }
    }
  }
  view.docIndex().swap(index);
  return view;
}

dynamic SliceV::toDynamic() const {
  return dynamic::object
    ("Slice", dynamic::object
     ("v", encode(v_))
     ("reverse", reverse_));
}

} // namespace op
} // namespace crystal
