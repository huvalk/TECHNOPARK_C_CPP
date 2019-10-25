#ifndef IZ2_CPP_MESSAGE_H
#define IZ2_CPP_MESSAGE_H

#include <stdbool.h>
#include <zconf.h>

#define DEF_STR_SIZE 22

// Дата
typedef struct Date {
    u_int8_t day;
    u_int8_t mounth;
    unsigned short year;
} Date;

// сообщение
typedef struct Message {
    Date date;
    char *user_name;
    char *body;
    char *theme;
    // поле recievers находится непосредственно внутри структы для более
    // эффективной работы с кэшем при поиске
    char recievers[DEF_STR_SIZE * 15];
} Message;

// вспомогательная струтура
typedef struct Dict {
    Date date;
    // поле theme находится непосредственно внутри структы для более эффективной
    // работы с кэшем при сортировке и выводе
    char theme[4 * DEF_STR_SIZE];
} Dict;

extern "C" {

// прочитать сообщение из консоли
bool getMessage(Message *current);

// прочитать дату из консоли
bool getDate(Date *current);

// преобразовать сообщение к вспомогательной структуре
void toDictElem(const Message *const src, Dict *const dest);

// сравнение структур даты
bool cmpDateMen(const Date *const left, const Date *const right);

// сравнение на равенство структур даты
bool cmpDictMen(const Dict *const left, const Dict *const right);

// сравнение вспомогательных структур по дате
bool cmpDateEq(const Date *const left, const Date *const right);

// сравнение на рпвенство вспомогательных структур по дате
bool cmpDictEq(const Dict *const left, const Dict *const right);

// поменять вспомогательные структуры местами
bool swapDict(Dict *const left, Dict *const right);

// проверка на попадание сообещния в промежуток времени
bool inPeriod(const Date *const period, const Message *const cur);

// проверка на совпадение получателя с именем пользователя
int8_t inRecievers(const char *const user, const Message *const cur);

};
#endif // IZ2_CPP_MESSAGE_H
