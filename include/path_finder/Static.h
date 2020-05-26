//
// Created by sokol on 29.01.20.
//

#ifndef MASTER_ARBEIT_STATIC_H
#define MASTER_ARBEIT_STATIC_H
#include "DataConfig.h"
#include "MmapVector.h"
#include <any>
#include <cstddef>
#include <iostream>
#include <sys/mman.h>
#include <vector>

namespace pathFinder {
class Static {
public:
  template <typename ItA, typename ItB, typename Container, typename Distance, typename ReplacePrevious>
  static inline void merge(ItA aBegin, ItA aEnd, ItB bBegin, ItB bEnd,
                           Distance distanceToLabel, Container &result,
                           ReplacePrevious replacePrevious) {
    auto i = aBegin;
    auto j = bBegin;
    while (i < aEnd && j < bEnd) {
      if (i->id < j->id) {
        result.push_back(replacePrevious(*i));
        ++i;
      } else if (j->id < i->id) {
        result.emplace_back(
            replacePrevious(j->id, j->cost + distanceToLabel, j->previousNode)
            );
        ++j;
      } else {
        if (i->cost < j->cost + distanceToLabel)
          result.emplace_back(replacePrevious(*i));
        else
          result.emplace_back(replacePrevious(j->id, j->cost + distanceToLabel,
                              j->previousNode));
        ++j;
        ++i;
      }
    }
    while (i != aEnd) {
      result.push_back(replacePrevious(*i));
      ++i;
    }
    while (j != bEnd) {
      result.emplace_back(replacePrevious(j->id, j->cost + distanceToLabel, j->previousNode));
      ++j;
    }
  }

  template <typename T>
  static inline void writeVectorToFile(std::vector<T> vector,
                                       const char *filename) {
    FILE *file = fopen64(filename, "w+");
    if (fwrite(vector.data(), sizeof(T), vector.size(), file) == 0)
      std::cerr << "could not write file " << filename << std::endl;
    fflush(file);
  }

  template <typename T>
  static inline std::vector<T>
  getFromFile(pathFinder::BinaryFileDescription fileDescription) {
    FILE *file = fopen64(fileDescription.path.c_str(), "r");
    std::vector<T> buf(fileDescription.size);
    std::fread(&buf[0], sizeof(buf[0]), buf.size(), file);
    return buf;
  }
  template <typename T>
  static inline auto
  getFromFileMMap(pathFinder::BinaryFileDescription fileDescription) {
    return pathFinder::MmapVector<T>(fileDescription.path.c_str(),
                                     fileDescription.size);
  }
};
} // namespace pathFinder

#endif // MASTER_ARBEIT_STATIC_H
