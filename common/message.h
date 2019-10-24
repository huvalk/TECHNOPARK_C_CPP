#ifndef UNTITLED_MESSAGE_H
#define UNTITLED_MESSAGE_H

#include <stdbool.h>
#include <zconf.h>

#define DEF_STR_SIZE 22

// TODO в структуре сообщения хранить тему и дату, остальное хранить через указатель
// TODO добавить метод копирования словаря

typedef struct Date {
    u_int8_t day;
    u_int8_t mounth;
    unsigned short year;
} Date;

typedef struct Message {
    Date date;
    char *user_name;
    char *body;
    char *theme;
    // поле recievers находится непосредственно внутри структы для более эффективной работы с кэшем при поиске
    char recievers[DEF_STR_SIZE * 15];
} Message;

typedef struct Dict {
    Date date;
    // поле theme находится непосредственно внутри структы для более эффективной работы с кэшем при сортировке и выводе
    char theme[4 * DEF_STR_SIZE];
} Dict;

bool getMessage(Message *current);

bool getDate(Date *current);

void toDictElem(const Message *const src, Dict *const dest);

bool cmp_date_men(const Date *const left, const Date *const right);

bool cmpDictMen(const Dict *const left, const Dict *const right);

bool swapDict(Dict *const left, Dict *const right);

bool inPeriod(const Date *const period, const Message *const cur);

int8_t inRecievers(const char *const user, const Message *const cur);

#endif //UNTITLED_MESSAGE_H
