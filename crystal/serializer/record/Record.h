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

#include "crystal/serializer/record/Accessor.h"
#include "crystal/type/TypeTraits.h"

namespace crystal {

class Record {
 public:
  Record() {}
  Record(const RecordMeta* recordMeta,
         const Accessor* accessor,
         Allocator* alloc,
         void* buf = nullptr) {
    init(recordMeta, accessor, alloc, buf);
  }

  virtual ~Record() {}

  void init(const RecordMeta* recordMeta,
            const Accessor* accessor,
            Allocator* alloc,
            void* buf = nullptr);

  void destroy();

  const RecordMeta* recordMeta() const;
  const Accessor* accessor() const;
  void* buffer() const;

  void setBuffer(void* buf);

  bool isValid() const;
  bool hasData() const;

  bool hasField(const FieldMeta& meta) const;
  bool hasField(const std::string& field) const;
  bool hasField(int tag) const;

  template <class T>
  typename std::enable_if<IsValue<T>::value, T>::type
  get(const FieldMeta& meta) const;
  template <class T>
  typename std::enable_if<IsValue<T>::value, T>::type
  get(const std::string& field) const;
  template <class T>
  typename std::enable_if<IsValue<T>::value, T>::type
  get(int tag) const;

  template <class T>
  typename std::enable_if<IsArray<T>::value, T>::type
  get(const FieldMeta& meta) const;
  template <class T>
  typename std::enable_if<IsArray<T>::value, T>::type
  get(const std::string& field) const;
  template <class T>
  typename std::enable_if<IsArray<T>::value, T>::type
  get(int tag) const;

  template <class T>
  typename std::enable_if<IsValue<T>::value, bool>::type
  set(const FieldMeta& meta, const T& value) const;
  template <class T>
  typename std::enable_if<IsValue<T>::value, bool>::type
  set(const std::string& field, const T& value) const;
  template <class T>
  typename std::enable_if<IsValue<T>::value, bool>::type
  set(int tag, const T& value) const;

  bool reset();
  bool merge(const Record& record);
  bool copy(const Record& record);

  bool buildVarArray(const FieldMeta& meta, size_t size);

  dynamic toDynamic() const;
  std::string toString(const json::serialization_opts& opts
                       = json::serialization_opts()) const;

 private:
  template <class T>
  dynamic toDynamic(const FieldMeta& meta) const;

  const RecordMeta* recordMeta_{nullptr};
  const Accessor* accessor_{nullptr};
  Allocator* alloc_{nullptr};
  void* buf_{nullptr};
};

//////////////////////////////////////////////////////////////////////

inline void Record::init(const RecordMeta* recordMeta,
                         const Accessor* accessor,
                         Allocator* alloc,
                         void* buf) {
  recordMeta_ = recordMeta;
  accessor_ = accessor;
  alloc_ = alloc;
  buf_ = buf;
}

inline void Record::destroy() {
  buf_ = nullptr;
}

inline const RecordMeta* Record::recordMeta() const {
  return recordMeta_;
}

inline const Accessor* Record::accessor() const {
  return accessor_;
}

inline void* Record::buffer() const {
  return buf_;
}

inline void Record::setBuffer(void* buf) {
  buf_ = buf;
}

inline bool Record::isValid() const {
  return recordMeta_ && accessor_ && alloc_;
}

inline bool Record::hasData() const {
  return buf_ != nullptr;
}

inline bool Record::hasField(const FieldMeta& meta) const {
  return accessor_->hasField(buf_, meta);
}

inline bool Record::hasField(const std::string& field) const {
  return hasField(*recordMeta_->getMeta(field));
}

inline bool Record::hasField(int tag) const {
  return hasField(*recordMeta_->getMeta(tag));
}

template <class T>
inline typename std::enable_if<IsValue<T>::value, T>::type
Record::get(const FieldMeta& meta) const {
  return accessor_->get<T>(buf_, alloc_, meta);
}

template <class T>
inline typename std::enable_if<IsValue<T>::value, T>::type
Record::get(const std::string& field) const {
  return get<T>(*recordMeta_->getMeta(field));
}

template <class T>
inline typename std::enable_if<IsValue<T>::value, T>::type
Record::get(int tag) const {
  return get<T>(*recordMeta_->getMeta(tag));
}

template <class T>
inline typename std::enable_if<IsArray<T>::value, T>::type
Record::get(const FieldMeta& meta) const {
  return accessor_->mget<typename T::value_type>(buf_, alloc_, meta);
}

template <class T>
inline typename std::enable_if<IsArray<T>::value, T>::type
Record::get(const std::string& field) const {
  return get<T>(*recordMeta_->getMeta(field));
}

template <class T>
inline typename std::enable_if<IsArray<T>::value, T>::type
Record::get(int tag) const {
  return get<T>(*recordMeta_->getMeta(tag));
}

template <class T>
inline typename std::enable_if<IsValue<T>::value, bool>::type
Record::set(const FieldMeta& meta, const T& value) const {
  if (!checkType<T>(meta.type()) || meta.isArray()) {
    return false;
  }
  return accessor_->set<T>(buf_, alloc_, meta, value);
}

template <class T>
inline typename std::enable_if<IsValue<T>::value, bool>::type
Record::set(const std::string& field, const T& value) const {
  return set(*recordMeta_->getMeta(field), value);
}

template <class T>
inline typename std::enable_if<IsValue<T>::value, bool>::type
Record::set(int tag, const T& value) const {
  return set(*recordMeta_->getMeta(tag), value);
}

inline bool Record::reset() {
  return accessor_->reset(buf_, alloc_, *recordMeta_);
}

inline bool Record::merge(const Record& record) {
  return accessor_->merge(
      buf_, alloc_, record.buf_, record.alloc_, record.accessor_,
      *recordMeta_);
}

inline bool Record::copy(const Record& record) {
  return accessor_->copy(
      buf_, alloc_, record.buf_, record.alloc_, record.accessor_,
      *recordMeta_);
}

inline bool Record::buildVarArray(const FieldMeta& meta, size_t size) {
  return accessor_->buildVarArray(buf_, alloc_, meta, size);
}

template <class T>
dynamic Record::toDynamic(const FieldMeta& meta) const {
  if (!meta.isArray()) {
    return get<T>(meta);
  }
  dynamic j = dynamic::array;
  Array<T> array = get<Array<T>>(meta);
  for (size_t i = 0; i < array.size(); ++i) {
    j.push_back(array.get(i));
  }
  return j;
}

}  // namespace crystal
