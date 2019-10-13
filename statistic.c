#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

#define ALLOC_ERR printf("Allocation error. Restart the program.\n");
#define INPUT_ERR printf("Input error. Try again\n");
#define DEF_SIZE 128

const size_t SIZE_DATE = sizeof( Date );
const size_t SIZE_CONTRACT = sizeof( Contract );
const size_t SIZE_ELEMENT = sizeof( Element );

int compare_sum( const Element dest, const Element src )
{
    return dest.sum - src.sum;
}



// копирование части договора в элемент словаря
void elem_cpy( Element* const dest, const Contract* const src )
{
    if( dest == src || dest == NULL || src == NULL )
        return;
    dest->sum = src->sum;
    memcpy( dest->name, src->name, DEF_SIZE );
}

// смена элементов словаря местами
void elem_swap( Element* const dest, Element* const src )
{
    if( dest == src || dest == NULL || src == NULL )
        return;
    Element buf = {0, ""};
    memcpy(&buf, src, SIZE_ELEMENT);
    memcpy(src, dest, SIZE_ELEMENT);
    memcpy(dest, &buf, SIZE_ELEMENT);
}

// чтение договора
int contract_read( Contract* const cur )
{
    char* buf_n = malloc( DEF_SIZE * 2 );
    char* buf_t = buf_n + DEF_SIZE;
    Date buf_d = {0, 0, 0};
    float buf_s = 0;

    size_t result = scanf( "%127s %f %127s %hhu.%hhu.%hu",
            buf_n, &buf_s, buf_t, &buf_d.day, &buf_d.mounth, &buf_d.year );
    if( result == 6 ){
        memcpy( cur->name, buf_n, DEF_SIZE );
        memcpy( cur->type, buf_t, DEF_SIZE );
        memcpy( &cur->date, &buf_d, SIZE_DATE );
        cur->sum = buf_s;
        free( buf_n );
        return 0;
    } else {
        free( buf_n );
        return 1;
    }
}

// функция разбиения для сортировка словаря
size_t part(Element* const arr, size_t low, size_t hight, int (*cmp)( const Element, const Element))
{

    size_t i = low, j = hight;

    while( 1 ) {
        while( cmp( arr[i], arr[low] ) > 0 || i == low ) { // >
            i++;
            if ( i == hight )
                break;
        }
        while( cmp( arr[i], arr[low] ) < 0 ) { // <
            j--;
            if( j == low )
                break;
        }

        if( i >= j)
            break;

        elem_swap(arr + i, arr + j);
    }
    elem_swap(arr + low, arr + j);
    return j;
}

// быстрая сортировка по сумме
void quick_sort(Element* const arr, size_t low, size_t hight, int (*cmp)( const Element, const Element))
{
    if( hight < 0 || hight <= low)
        return;
    size_t j = part(arr, low, hight, cmp);
    if( j != 0 )
        quick_sort(arr, low, j - 1, cmp);
    quick_sort(arr, j + 1, hight, cmp);
}

// поиск топ-3 компаний
void find_top(const Contract* const arr, size_t* const cap, Element* const first, Element* const second, Element* const third)
{
    if( arr == NULL || cap == NULL || first == NULL || second == NULL || third == NULL ) {
        return;
    }

    if( *cap <= 0 ) {
        return;
    }

    size_t n = *cap;
    // создания словаря для поиска сумм договоров
    Element* dict = malloc(n * SIZE_ELEMENT);
    if( dict == NULL ) {
        ALLOC_ERR
        return;
    }
    memset(dict, 0, n * SIZE_ELEMENT);

    // поиск сумм договоров
    elem_cpy(dict, arr);
    size_t cur_cap = 1;
    for( size_t i = 1; i < n; i++ ) {
        u_int8_t flag = 1;
        for( size_t j = 0; j < cur_cap && flag; j++ ) {
            if( !strcmp(arr[i].name, dict[j].name) ) {
                dict[j].sum += arr[i].sum;
                flag = 0;
            }
        }
        if( flag ) {
            elem_cpy(dict + cur_cap, arr + i);
            cur_cap++;
        }
    }

    // сортировка словаря по сумме договоров
    quick_sort(dict, 0, cur_cap - 1, compare_sum);

    // результат
    memcpy(first, dict, SIZE_ELEMENT);
    if( cur_cap > 1 ) {
        memcpy(second, dict + 1, SIZE_ELEMENT);
        if( cur_cap > 2) {
            memcpy(third, dict + 2, SIZE_ELEMENT);
        }
    }
    *cap = cur_cap;
    free(dict);
    return;
}