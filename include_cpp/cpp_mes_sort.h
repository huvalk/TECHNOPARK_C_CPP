#ifndef IZ2_CPP_MES_SORT_H
#define IZ2_CPP_MES_SORT_H

#include "cpp_message.h"
#include <stdbool.h>

extern "C" bool sort(Dict *const arr, size_t size,
                     bool (*cmp)(const Dict *const, const Dict *const));

#endif // IZ2_CPP_MES_SORT_H
