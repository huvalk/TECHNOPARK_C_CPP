#ifndef IZ_1_STATISTIC_H
#define IZ_1_STATISTIC_H
#include <stdlib.h>
#include "structs.h"

void elem_cpy( Element* const dest, const Contract* const src );
void elem_swap( Element* const dest, Element* const src );
int contract_read( Contract* const cur );
size_t part( Element* const arr, size_t low, size_t hight, int (*cmp)( const Element, const Element ) );
void quick_sort( Element* const arr, size_t low, size_t hight, int (*cmp)( const Element, const Element ) );
void find_top( const Contract* const arr, size_t* cap, Element* const first, Element* const second, Element* const third );


#endif //IZ_1_STATISTIC_H
