#ifndef IZ2_D_USER_MESSAGES_H
#define IZ2_D_USER_MESSAGES_H

#include "../common/mes_sort.h"

Dict *findMessages(size_t *const message_count, const Message *const messages, char *const user,
                   const Date *const period);

#endif //IZ2_D_USER_MESSAGES_H
