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

#include <algorithm>

#include "crystal/memory/AllocatorUtil.h"
#include "crystal/memory/Memory.h"

namespace crystal {

template <class T>
class Queue {
 public:
  Queue(size_t capacity, int64_t headOffset)
      : capacity_(std::max(capacity, 2ul)),
        headOffset_(headOffset) {}

  ~Queue() {}

  bool push(Memory* memory, const T& value);
  bool pop();

  T front(Memory* memory) const;

  bool empty() const;
  bool full() const;

  size_t capacity() const;

  void expand(size_t n);

 private:
  T* head(Memory* memory) const {
    return address<T>(memory, headOffset_);
  }

  size_t next(size_t p) const {
    return (p + 1) % capacity_;
  }

  size_t capacity_;
  int64_t headOffset_;
  size_t size_{0};
  size_t front_{0};
  size_t rear_{0};
};

template <class T>
int64_t createQueue(Memory* memory, size_t capacity);

template <class T>
Queue<T>* expandQueue(Memory* memory, int64_t offset, size_t n);

//////////////////////////////////////////////////////////////////////

template <class T>
bool Queue<T>::push(Memory* memory, const T& value) {
  if (full()) {
    return false;
  }
  head(memory)[rear_] = value;
  rear_ = next(rear_);
  ++size_;
  return true;
}

template <class T>
bool Queue<T>::pop() {
  if (empty()) {
    return false;
  }
  front_ = next(front_);
  --size_;
  return true;
}

template <class T>
T Queue<T>::front(Memory* memory) const {
  if (empty()) {
    return T();
  }
  return head(memory)[front_];
}

template <class T>
inline bool Queue<T>::empty() const {
  return size_ == 0;
}

template <class T>
inline bool Queue<T>::full() const {
  return size_ == capacity_;
}

template <class T>
inline size_t Queue<T>::capacity() const {
  return capacity_;
}

template <class T>
int64_t createQueue(Memory* memory, size_t n) {
  int64_t offset = memory->allocate(sizeof(Queue<T>) + sizeof(T) * n);
  if (offset > 0) {
    int64_t headOffset = offset + sizeof(Queue<T>);
    new (memory->address(offset)) Queue<T>(n, headOffset);
  }
  return offset;
}

template <class T>
Queue<T>* expandQueue(Memory* memory, int64_t offset, size_t n) {
  Queue<T>* queue;
  queue = address<Queue<T>>(memory, offset);
  n += queue->capacity();
  int64_t headOffset = memory->allocate(sizeof(T) * n);
  if (headOffset == 0) {
    return nullptr;
  }
  Queue<T> newQueue(n, headOffset);
  queue = address<Queue<T>>(memory, offset);
  while (!queue->empty()) {
    newQueue.push(memory, queue->front(memory));
    queue->pop();
  }
  *queue = newQueue;
  return queue;
}

}  // namespace crystal
