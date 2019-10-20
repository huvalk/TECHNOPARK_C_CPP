#ifndef IZ2_USER_MESSAGES_H
#define IZ2_USER_MESSAGES_H

#include "message.h"

Dict* find_messages(size_t* const message_count, const Message* const messages, char* const user, const Date* const period);

void Merge( const Dict* const left, const size_t left_size, const Dict* const right, const size_t right_size,
            Dict* buf, int (*cmp)( const Dict* const, const Dict* const ) );

void Sort( Dict* const arr, const size_t n, int (*cmp)( const Dict* const, const Dict* const ) );

#endif //IZ2_USER_MESSAGES_H
