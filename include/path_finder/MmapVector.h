//
// Created by sokol on 27.01.20.
//

#ifndef MASTER_ARBEIT_MMAPVECTOR_H
#define MASTER_ARBEIT_MMAPVECTOR_H
#include <cstddef>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <vector>
namespace pathFinder {

template <typename T, typename Allocator = std::allocator<T>> class MmapVector {
private:
  T *_data;
  size_t _size;

public:
  MmapVector();
  MmapVector(const std::vector<T> &ramVector, const char *filename);
  MmapVector(const char *filename, size_t size);

  T &operator[](size_t index);

  T *begin();

  T *end();

  size_t size();

  void push_back(T element);
};

template <typename T, typename Allocator>
MmapVector<T, Allocator>::MmapVector(const std::vector<T> &ramVector,
                                     const char *filename) {
  FILE *file = fopen64(filename, "w+");
  fwrite(ramVector.data(), sizeof(T), ramVector.size(), file);
  fflush(file);

  _data = (T *)mmap64(nullptr, sizeof(T) * ramVector.size(), PROT_READ,
                      MAP_SHARED, fileno(file), 0);
  _size = ramVector.size();
}
template <typename T, typename Allocator>
MmapVector<T, Allocator>::MmapVector(const char *filename, size_t size) {
  FILE *file = fopen64(filename, "r");
  if (file == nullptr)
    throw std::runtime_error("could not open file: " + std::string(filename));

  auto mmapPointer =
      mmap64(nullptr, sizeof(T) * size, PROT_READ, MAP_SHARED, fileno(file), 0);
  if (mmapPointer == MAP_FAILED)
    throw std::runtime_error("could not map file(" + std::string(filename) + ") to memory");
  _data = (T *)mmapPointer;
  _size = size;
}
template <typename T, typename Allocator>
T &MmapVector<T, Allocator>::operator[](size_t index) {
  return _data[index];
}
template <typename T, typename Allocator> T *MmapVector<T, Allocator>::begin() {
  return _data;
}
template <typename T, typename Allocator> T *MmapVector<T, Allocator>::end() {
  return _data + _size;
}
template <typename T, typename Allocator>
size_t MmapVector<T, Allocator>::size() {
  return _size;
}
template <typename T, typename Allocator>
MmapVector<T, Allocator>::MmapVector() {}

template <typename T, typename Allocator>
void MmapVector<T, Allocator>::push_back(T element) {

}
} // namespace pathFinder
#endif // MASTER_ARBEIT_MMAPVECTOR_H
