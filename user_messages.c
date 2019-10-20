#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"

#define DEF_ARR_SIZE 4

Dict* find_messages( size_t* const message_count, const Message* const messages,
                    char* const user, const Date* const period )
{
    size_t cap = DEF_ARR_SIZE, size = 0;
    Dict* dict = calloc(cap, sizeof(Dict));
    for(size_t i = 0; i < *message_count; i++) {
        if( in_period(period, (messages + i)) && in_recievers(user, (messages + i)) ) {
            if( size >= cap ) {
                resize_dict(dict, &cap);
            }

            to_dict_elem((messages + i), (dict + size));
            size++;
        }
    }

    *message_count = size;
    return dict;
}

void Merge( const Dict* const left, const size_t left_size, const Dict* const right, const size_t right_size,
            Dict buf[], bool (*cmp)( const Dict* const, const Dict* const ) )
{
    size_t i = 0, j = 0;
    // сравение идет по первому значению в кортеже
    while( i < left_size && j < right_size ) {
        if( cmp( left + i, right + j ) ) {
            memcpy( buf + i + j, left + i, sizeof(Dict) );
            i++;
        } else {
            memcpy( buf + i + j, right + j, sizeof(Dict) );
            j++;
        }
    }

    // если в одном из массивов остались элементы
    if( i == left_size ) {
        while( j < right_size) {
            memcpy( buf + i + j, right + j, sizeof(Dict) );
            j++;
        }
    } else {
        while( i < left_size ) {
            memcpy( buf + i + j, left + i, sizeof(Dict) );
            i++;
        }
    }
}

void Sort( Dict* const arr, const size_t size, bool (*cmp)( const Dict* const, const Dict* const ) )
{
    if( size < 2)
        return;

    // выделение длины подмассивов для рекурсивной сортировки
    size_t left_size = size / 2;
    size_t right_size = size - left_size;
    Sort( arr, left_size, cmp );
    Sort( arr + left_size, right_size, cmp );

    // буферный массив для слияния
    Dict* new_arr = calloc(size, sizeof(Dict));

    // слияние
    Merge( arr, left_size, arr + left_size, right_size, new_arr, cmp );

    // копирование буферного массива в основной
    memcpy(arr, new_arr, size * sizeof(Dict));

    free(new_arr);

    return;
}
