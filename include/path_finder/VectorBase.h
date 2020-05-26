//
// Created by sokol on 01.04.20.
//

#ifndef MASTER_ARBEIT_VECTORBASE_H
#define MASTER_ARBEIT_VECTORBASE_H

#include <vector>

namespace pathFinder {
template <typename T, typename Allocator = std::allocator<T>> class VectorBase {
public:
  virtual T &operator[](size_t index);

  virtual T *begin();

  virtual T *end();

  virtual size_t size();

  virtual void push_back(T element);

  virtual T *data();

  virtual void clear();

  virtual void shrink_to_fit();
};

template <typename T, typename Allocator>
T &VectorBase<T, Allocator>::operator[](size_t index) {
  return nullptr;
}

template <typename T, typename Allocator> T *VectorBase<T, Allocator>::begin() {
  return nullptr;
}

template <typename T, typename Allocator> T *VectorBase<T, Allocator>::end() {
  return nullptr;
}

template <typename T, typename Allocator>
size_t VectorBase<T, Allocator>::size() {
  return 0;
}

template <typename T, typename Allocator>
void VectorBase<T, Allocator>::push_back(T element) {}

template <typename T, typename Allocator> T *VectorBase<T, Allocator>::data() {
  return nullptr;
}

template <typename T, typename Allocator>
void VectorBase<T, Allocator>::clear() {}

template <typename T, typename Allocator>
void VectorBase<T, Allocator>::shrink_to_fit() {}
} // namespace pathFinder
#endif // MASTER_ARBEIT_VECTORBASE_H
