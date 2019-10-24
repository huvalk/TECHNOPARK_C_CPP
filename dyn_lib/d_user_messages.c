#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../mes/message.h"

#define DEF_ARR_SIZE 2

extern size_t SIZE_DATE;
extern size_t SIZE_MESSAGE;
extern size_t SIZE_DICT;


struct export_vtable {
    Dict* (*find_messages)(size_t* const message_count, const Message* const messages, char* const user, const Date* const period);
};

void Merge( const Dict* const left, const size_t left_size, const Dict* const right, const size_t right_size,
            Dict* const buf, bool (*cmp)( const Date* const, const Date* const ) )
{
    size_t i = 0, j = 0;
    // сравение идет по первому значению в кортеже
    while( i < left_size && j < right_size ) {
        if( cmp( &left[i].date, &right[j].date ) ) {
            memcpy( buf + i + j, left + i, SIZE_DICT );
            i++;
        } else {
            memcpy( buf + i + j, right + j, SIZE_DICT );
            j++;
        }
    }

    // если в одном из массивов остались элементы
    if( i == left_size ) {
        while( j < right_size) {
            memcpy( buf + i + j, right + j, SIZE_DICT );
            j++;
        }
    } else {
        while( i < left_size ) {
            memcpy( buf + i + j, left + i, SIZE_DICT );
            i++;
        }
    }
}

void Sort( Dict* const arr, const size_t size, bool (*cmp)( const Date* const, const Date* const ) )
{
    if( size < 2)
        return;

    // выделение длины подмассивов для рекурсивной сортировки
    size_t left_size = size / 2;
    size_t right_size = size - left_size;
    Sort( arr, left_size, cmp );
    Sort( arr + left_size, right_size, cmp );

    // буферный массив для слияния
    Dict* new_arr = malloc(size * SIZE_DICT);

    // слияние
    Merge( arr, left_size, arr + left_size, right_size, new_arr, cmp );

    // копирование буферного массива в основной
    memcpy(arr, new_arr, size * SIZE_DICT);

    free(new_arr);

    return;
}

Dict* find_messages( size_t* const message_count, const Message* const messages,
                     char* const user, const Date* const period )
{
    size_t cap = DEF_ARR_SIZE, size = 0;
    Dict* dict = calloc(cap, SIZE_DICT);
    if(dict == NULL)
        return NULL;
    for(size_t i = 0; i < *message_count; i++) {
        int8_t flag = in_recievers(user, (messages + i));
        if(  flag == 1 && in_period(period, (messages + i)) ) {
            if( size >= cap ) {
                Dict* nw_dict = malloc( cap * SIZE_DICT * 2 );
                if(nw_dict == NULL)
                    return NULL;
                memcpy(nw_dict, dict, cap * SIZE_DICT);

                cap *= 2;
                free(dict);
                dict = nw_dict;
            }

            to_dict_elem((messages + i), (dict + size));
            size++;
        } else if (flag == -1) {
            return NULL;
        }
    }

    Sort(dict, size, cmp_date_men);

    *message_count = size;
    return dict;
}

struct export_vtable exports = {find_messages};
