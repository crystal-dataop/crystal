#include "crystal/serializer/record/containers/Serialization.h"

namespace crystal {

size_t bufferSize(const untyped_tuple& value) {
  size_t n = value.fixed_size();
  for (size_t i = 0; i < value.tuple_size(); ++i) {
    n += value.element_buffer_size(i);
  }
  return n;
}

void serialize(const untyped_tuple& from, untyped_tuple& to, uint8_t* buffer) {
  uint8_t* old = from.offset_.get();
  uint8_t* buf = buffer;
  size_t n = from.fixed_size();
  std::memcpy(buf, old, n);
  *buf &= 0x80;
  buffer += n;
  for (size_t i = 0; i < from.tuple_size(); ++i) {
    switch (from.meta_->metas[i].type) {
#define CASE(dt, t) \
      case DataType::dt: serialize(from.get<t>(i), to.get<t>(i), buffer); break;

      CASE(BOOL, bool)
      CASE(INT8, int8_t)
      CASE(INT16, int16_t)
      CASE(INT32, int32_t)
      CASE(INT64, int64_t)
      CASE(UINT8, uint8_t)
      CASE(UINT16, uint16_t)
      CASE(UINT32, uint32_t)
      CASE(UINT64, uint64_t)
      CASE(FLOAT, float)
      CASE(DOUBLE, double)
      CASE(STRING, string)
      CASE(TUPLE, untyped_tuple)
      default: break;

#undef CASE
    }
    buffer += from.element_buffer_size(i);
  }
  to.offset_ = buf;
  to.meta_ = from.meta_;
}

} // namespace crystal
