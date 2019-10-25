#ifndef IZ2_CPP_S_USER_MESSAGES_H
#define IZ2_CPP_S_USER_MESSAGES_H

#include "cpp_mes_sort.h"

extern "C" Dict *run(size_t *const message_count, const Message *const messages,
                     char *const user, const Date *const period);

#endif // IZ2_CPP_S_USER_MESSAGES_H
