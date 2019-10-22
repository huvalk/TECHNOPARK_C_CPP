#ifndef IZ2_S_USER_MESSAGES_H
#define IZ2_S_USER_MESSAGES_H

#include "../mes/message.h"

Dict* find_messages(size_t* const message_count, const Message* const messages, char* const user, const Date* const period);

#endif //IZ2_S_USER_MESSAGES_H
