#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "message.h"

bool get_message( Message* current )
{

}

void to_dict_elem( const Message* const src, Dict* const dest)
{
    dest->date.day = src->date.day;
    dest->date.mounth = src->date.mounth;
    dest->date.year = src->date.year;
    strcpy( dest->theme, src->theme );
}

bool cmp_date_men( const Date* const left, const Date* const right )
{
    bool res = true;
    if( left->year > right->year ) {
        res = false;
    } else if ( left->mounth > right->mounth ) {
        res = false;
    } else if ( left->day > right->day ) {
        res = false;
    }

    return res;
}

bool in_period( const Date* const period, const Message* const cur )
{
    return ( cmp_date_men(period, &cur->date) && cmp_date_men(&cur->date, period + 1) );
}

bool in_recievers( const char* const user, const Message* const cur )
{
    return strstr(cur->recievers, user) != 0;
}

void resize_dict( Dict* dict, size_t* const size)
{
    Dict* nw_dict = calloc( *size, sizeof(Dict) );
    memcpy(nw_dict, dict, *size * sizeof(Dict));

    *size *= 2;
    free(dict);
    dict = nw_dict;
}


