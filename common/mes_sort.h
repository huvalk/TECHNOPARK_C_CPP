#ifndef IZ2_MES_SORT_H
#define IZ2_MES_SORT_H

#include "message.h"
#include <stdbool.h>

// сортировка кучей
bool sort(Dict *const arr, size_t size,
          bool (*cmp)(const Dict *const, const Dict *const));

#endif //IZ2_MES_SORT_H
