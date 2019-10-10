#ifndef IZ_1_STATICTIC_CPP_H
#define IZ_1_STATICTIC_CPP_H
#include "structs.h"

extern "C" void elem_cpy( Element* const dest, const Contract* const src );
extern "C" void elem_swap( Element* const dest, Element* const src );
extern "C" int contract_read( Contract* const cur );
extern "C" size_t part( Element* const arr, size_t low, size_t hight, int (*cmp)( const Element, const Element ) );
extern "C" void quick_sort( Element* const arr, size_t low, size_t hight, int (*cmp)( const Element, const Element ) );
extern "C" void find_top( const Contract* const arr, size_t* cap, Element* const first, Element* const second, Element* const third );

#endif //IZ_1_STATICTIC_CPP_H
