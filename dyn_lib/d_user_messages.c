#include "d_user_messages.h"
#include "../common/mes_sort.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define DEF_ARR_SIZE 2

extern size_t SIZE_DATE;
extern size_t SIZE_MESSAGE;
extern size_t SIZE_DICT;

Dict *findMessages(size_t *const message_count, const Message *const messages,
                   char *const user, const Date *const period) {
    size_t cap = DEF_ARR_SIZE, size = 0;
    Dict *dict = calloc(cap, SIZE_DICT);
    if (dict == NULL)
        return NULL;
    for (size_t i = 0; i < *message_count; i++) {
        int8_t flag = inRecievers(user, (messages + i));
        if (flag == 1 && inPeriod(period, (messages + i))) {
            if (size >= cap) {
                Dict *nw_dict = malloc(cap * SIZE_DICT * 2);
                if (nw_dict == NULL)
                    return NULL;
                memcpy(nw_dict, dict, cap * SIZE_DICT);

                cap *= 2;
                free(dict);
                dict = nw_dict;
            }

            toDictElem((messages + i), (dict + size));
            size++;
        } else if (flag == -1) {
            return NULL;
        }
    }

    if (sort(dict, size, cmpDictMen) == false)
        return NULL;

    *message_count = size;
    return dict;
}

struct exportVtable exports = {*findMessages};
