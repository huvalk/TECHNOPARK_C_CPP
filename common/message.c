#include "message.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

size_t SIZE_DATE = sizeof(Date);
size_t SIZE_MESSAGE = sizeof(Message);
size_t SIZE_DICT = sizeof(Dict);
size_t SIZE_SIZE_T = sizeof(size_t);

// прочитать сообщение из консоли
bool getMessage(Message *current) {
  if (current == NULL)
    return true;

  Date buf_d = {0, 0, 0};
  char *buf_name = calloc(DEF_STR_SIZE * 20, 1);
  if (buf_name == NULL)
    return true;
  char *buf_body = buf_name + DEF_STR_SIZE;
  char *buf_theme = buf_name + DEF_STR_SIZE * 16;
  char *buf_rec = calloc(DEF_STR_SIZE * 15, 1);
  if (buf_rec == NULL) {
    return true;
  }

  size_t result =
      scanf("%21s %329s %329s %43s %hhu.%hhu.%hu", buf_name, buf_body, buf_rec,
            buf_theme, &buf_d.day, &buf_d.mounth, &buf_d.year);
  if (result == 7) {
    current->user_name = buf_name;
    current->body = buf_body;
    current->theme = buf_theme;
    memcpy(&current->date, &buf_d, SIZE_DATE);
    strcpy(current->recievers, buf_rec);
    free(buf_rec);
    return false;
  } else {
    free(buf_name);
    free(buf_rec);
    return true;
  }
}

// прочитать дату из консоли
bool getDate(Date *current) {
  if (current == NULL)
    return true;
  Date buf_d = {0, 0, 0};

  size_t result =
      scanf("%hhu.%hhu.%hu", &buf_d.day, &buf_d.mounth, &buf_d.year);
  if (result == 3) {
    memcpy(current, &buf_d, SIZE_DATE);
    return false;
  } else {
    return true;
  }
}

// преобразовать сообщение к вспомогательной структуре
void toDictElem(const Message *const src, Dict *const dest) {
  if (src == NULL || dest == NULL)
    return;
  memcpy(&dest->date, &src->date, SIZE_DATE);
  strcpy(dest->theme, src->theme);
}

// сравнение структур даты
bool cmpDateMen(const Date *const left, const Date *const right) {
  if (left == NULL)
    return true;
  if (right == NULL)
    return false;

  int d1 = left->year * 365 + left->mounth * 31 + left->day;
  int d2 = right->year * 365 + right->mounth * 31 + right->day;
  if (d1 <= d2) {
    return true;
  } else {
    return false;
  }
}

// сравнение на равенство структур даты
bool cmpDateEq(const Date *const left, const Date *const right) {
  if (right == NULL || left == NULL)
    return false;

  int d1 = left->year * 365 + left->mounth * 31 + left->day;
  int d2 = right->year * 365 + right->mounth * 31 + right->day;
  if (d1 == d2) {
    return true;
  } else {
    return false;
  }
}

// сравнение вспомогательных структур по дате
bool cmpDictMen(const Dict *const left, const Dict *const right) {
  if (left == NULL)
    return true;
  if (right == NULL)
    return false;
  return cmpDateMen(&left->date, &right->date);
}

// сравнение на рпвенство вспомогательных структур по дате
bool cmpDictEq(const Dict *const left, const Dict *const right) {
  if (right == NULL || left == NULL)
    return false;
  return cmpDateEq(&left->date, &right->date);
}

// поменять вспомогательные структуры местами
bool swapDict(Dict *const left, Dict *const right) {
  if (right == NULL || left == NULL)
    return false;
  Dict *buf = malloc(SIZE_DICT);
  if (buf == NULL)
    return false;
  memcpy(buf, left, SIZE_DICT);
  memcpy(left, right, SIZE_DICT);
  memcpy(right, buf, SIZE_DICT);

  free(buf);
  return true;
}

// проверка на попадание сообещния в промежуток времени
bool inPeriod(const Date *const period, const Message *const cur) {
  if (period == NULL || cur == NULL)
    return false;
  return (cmpDateMen(period, &cur->date) && cmpDateMen(&cur->date, period + 1));
}

// проверка на совпадение получателя с именем пользователя
int8_t inRecievers(const char *const user, const Message *const cur) {
  if (user == NULL || cur == NULL)
    return -1;
  size_t buf_size = strlen(user) + 3;
  char *buf = malloc(buf_size * sizeof(char));
  memset(buf, 0, buf_size);
  if (buf == NULL) {
    return -1;
  }
  buf[0] = '/';
  strcat(buf, user);
  strcat(buf, ".");
  bool res = strstr(cur->recievers, buf);
  free(buf);
  if (res) {
    return 1;
  } else {
    return 0;
  }
}


