#ifndef IZ2_S_USER_MESSAGES_H
#define IZ2_S_USER_MESSAGES_H

#include "../common/mes_sort.h"

// поиск сообщений пользователя в промежутке времени и сортировка по дате в несколько процессов
Dict *run(size_t *const message_count, const Message *const messages,
          char *const user, const Date *const period);

#endif //IZ2_S_USER_MESSAGES_H
