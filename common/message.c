#include "message.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

size_t SIZE_DATE = sizeof(Date);
size_t SIZE_MESSAGE = sizeof(Message);
size_t SIZE_DICT = sizeof(Dict);

bool getMessage(Message *current) {
  Date buf_d = {0, 0, 0};
  char *buf_name = calloc(DEF_STR_SIZE * 20, 1);
  char *buf_body = buf_name + DEF_STR_SIZE;
  char *buf_theme = buf_name + DEF_STR_SIZE * 16;
  char *buf_rec = calloc(DEF_STR_SIZE * 15, 1);

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
    return 0;
  } else {
    free(buf_name);
    free(buf_rec);
    return 1;
  }
}

bool getDate(Date *current) {
  Date buf_d = {0, 0, 0};

  size_t result =
      scanf("%hhu.%hhu.%hu", &buf_d.day, &buf_d.mounth, &buf_d.year);
  if (result == 3) {
    memcpy(current, &buf_d, SIZE_DATE);
    return 0;
  } else {
    return 1;
  }
}

void toDictElem(const Message *const src, Dict *const dest) {
  memcpy(&dest->date, &src->date, SIZE_DATE);
  strcpy(dest->theme, src->theme);
}

bool cmp_date_men(const Date *const left, const Date *const right) {
  bool res = true;
  if (left->year > right->year) {
    res = false;
  } else if (left->mounth > right->mounth) {
    res = false;
  } else if (left->day > right->day) {
    res = false;
  }

  return res;
}

bool cmpDictMen(const Dict *const left, const Dict *const right) {
  bool res = true;
  if (left->date.year > right->date.year) {
    res = false;
  } else if (left->date.mounth > right->date.mounth) {
    res = false;
  } else if (left->date.day > right->date.day) {
    res = false;
  }

  return res;
}

bool swapDict(Dict *const left, Dict *const right) {
  Dict *buf = malloc(SIZE_DICT);
  if (buf == NULL)
    return false;
  memcpy(buf, left, SIZE_DICT);
  memcpy(left, right, SIZE_DICT);
  memcpy(right, buf, SIZE_DICT);

  free(buf);
  return true;
}

bool inPeriod(const Date *const period, const Message *const cur) {
  return (cmp_date_men(period, &cur->date) &&
          cmp_date_men(&cur->date, period + 1));
}

int8_t inRecievers(const char *const user, const Message *const cur) {
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


