#ifndef IZ2_CPP_D_USER_MESSAGES_H
#define IZ2_CPP_D_USER_MESSAGES_H

#include "cpp_mes_sort.h"

// поиск сообщений пользователя в промежутке времени и сортировка по дате
extern "C" struct exportVtable {
  Dict *(*findMessage)(size_t *const message_count,
                       const Message *const messages, char *const user,
                       const Date *const period);
};

#endif // IZ2_CPP_D_USER_MESSAGES_H
