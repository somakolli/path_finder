#pragma once
#include "VectorBase.h"
#include <vector>
namespace pathFinder {
template <typename T, typename Allocator = std::allocator<T>>
class RamVector : public VectorBase<T, Allocator> {
private:
  std::vector<T, Allocator> vec;

public:
  explicit RamVector(size_t size);

  T &operator[](size_t index);

  T *begin();

  T *end();

  size_t size();

  void push_back(T element);

  T *data();

  void clear();

  void shrink_to_fit();

  bool empty();
};

template <typename T, typename Allocator>
T &RamVector<T, Allocator>::operator[](size_t index) {
  return vec[index];
}

template <typename T, typename Allocator>
T *RamVector<T, Allocator>::begin() {
  return vec.begin();
}

template <typename T, typename Allocator>
T *RamVector<T, Allocator>::end() {
  return vec.end();
}

template <typename T, typename Allocator>
size_t RamVector<T, Allocator>::size() {
  return vec.size();
}

template <typename T, typename Allocator>
void RamVector<T, Allocator>::push_back(T element) {
  vec.push_back(element);
}

template <typename T, typename Allocator>
RamVector<T, Allocator>::RamVector(size_t size) {
  vec(size);
}

template <typename T, typename Allocator>
T *RamVector<T, Allocator>::data() {
  return vec.data();
}

template <typename T, typename Allocator>
void RamVector<T, Allocator>::clear() {
  vec.clear();
}

template <typename T, typename Allocator>
void RamVector<T, Allocator>::shrink_to_fit() {
  vec.shrink_to_fit();
}

template <typename T, typename Allocator>
bool RamVector<T, Allocator>::empty() {
  return vec.empty();
}
} // namespace pathFinder
