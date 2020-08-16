//
// Created by sokol on 29.01.20.
//
#include <path_finder/helper/Static.h>

void pathFinder::Static::conditionalFree(void *pointer, bool isMMaped, size_t size) {
  if(isMMaped)
    munmap(pointer, size);
  else
    free(pointer);
}
