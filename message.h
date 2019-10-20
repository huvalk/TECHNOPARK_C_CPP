#ifndef UNTITLED_MESSAGE_H
#define UNTITLED_MESSAGE_H

#include <zconf.h>
#include <stdbool.h>

#define DEF_STR_SIZE 32

typedef struct Date
{
    u_int8_t day;
    u_int8_t mounth;
    unsigned short year;
} Date;

typedef struct Message
{
    Date date;
    char user_name[DEF_STR_SIZE];
    char body[10*DEF_STR_SIZE];
    char recievers[10*DEF_STR_SIZE];
    char theme[4*DEF_STR_SIZE];
} Message;

typedef struct Dict
{
    Date date;
    char theme[4*DEF_STR_SIZE];
} Dict;

bool get_message( Message* current );

void to_dict_elem( const Message* const src, Dict* const dest);

bool cmp_date_men( const Date* const left, const Date* const right );

bool in_period( const Date* const period, const Message* const cur );

bool in_recievers( const char* const period, const Message* const cur );

void resize_dict( Dict* dict, size_t* const size);

#endif //UNTITLED_MESSAGE_H
