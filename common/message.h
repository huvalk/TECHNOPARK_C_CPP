#ifndef UNTITLED_MESSAGE_H
#define UNTITLED_MESSAGE_H

#include <stdbool.h>
#include <zconf.h>

#define DEF_STR_SIZE 22

// TODO в структуре сообщения хранить тему и дату, остальное хранить через указатель
// TODO добавить метод копирования словаря

typedef struct Date
{
    u_int8_t day;
    u_int8_t mounth;
    unsigned short year;
} Date;

typedef struct Message
{
    Date date;
    char *user_name;
    char *body;
    char *theme;
    // поле recievers находится непосредственно внутри структы для более эффективной работы с кэшем при поиске
    char recievers[DEF_STR_SIZE * 15];
} Message;

typedef struct Dict
{
    Date date;
    // поле theme находится непосредственно внутри структы для более эффективной работы с кэшем при сортировке и выводе
    char theme[4*DEF_STR_SIZE];
} Dict;

bool get_message( Message* current );

bool get_date( Date* current );

void to_dict_elem( const Message* const src, Dict* const dest);

bool cmp_date_men( const Date* const left, const Date* const right );

bool cmp_dict_men( const Dict* const left, const Dict* const right );

bool swap_dict( Dict* const left, Dict* const right );

bool in_period( const Date* const period, const Message* const cur );

int8_t in_recievers( const char* const period, const Message* const cur );

#endif //UNTITLED_MESSAGE_H
