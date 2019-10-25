#include "message.h"
#include <stddef.h>

bool siftDown(size_t i, size_t size, Dict *const dict,
              bool (*cmp)(const Dict *const, const Dict *const)) {
  if (i >= size)
    return true;

  size_t left_child = 2 * i + 1;
  size_t right_child = 2 * i + 2;

  size_t min = i;
  if (left_child < size && cmp(dict + i, dict + left_child)) { // <
    min = left_child;
  }
  if (right_child < size && cmp(dict + min, dict + right_child)) { // <
    min = right_child;
  }
  if (min != i) {
    if (swapDict(dict + i, dict + min) == false)
      return false;
    if (siftDown(min, size, dict, cmp) == false)
      return false;
  }

  return true;
}

bool buildHeap(size_t size, Dict *const dict,
               bool (*cmp)(const Dict *const, const Dict *const)) {
  for (ptrdiff_t i = size / 2 - 1; i >= 0; i--) {
    if (siftDown(i, size, dict, cmp) == false)
      return false;
  }
  return true;
}

bool sort(Dict *const arr, size_t size,
          bool (*cmp)(const Dict *const, const Dict *const)) {
  if (buildHeap(size, arr, cmp) == false)
    return false;
  while (size > 1) {
    if (swapDict(arr, arr + size - 1) == false)
      return false;
    size--;
    if (siftDown(0, size, arr, cmp) == false)
      return false;
  }

  return true;
}