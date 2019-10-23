#include <dlfcn.h>
#include "static_lib/s_user_messages.h"
#include <stdio.h>
#include <stdlib.h>

#define ALLOC_ERR "Allocation error. Restart the program.\n"
#define INPUT_ERR "Input error. Try again\n"

extern size_t SIZE_DATE;
extern size_t SIZE_MESSAGE;
extern size_t SIZE_DICT;

int main() {
    // чтение входных данных
    size_t n = 0;
    printf( "How many messages you wold like to insert?\n" );
    while( scanf("%lu", &n) != 1 ) {
        printf(INPUT_ERR);
        while ( ( getchar() ) != '\n' );
    }

    if( n <= 1 ) {
        return 0;
    }

    Message* arr = calloc(n, sizeof(Message));
    if( arr == NULL ) {
        printf(ALLOC_ERR);
        return 0;
    }

    // чтение писем
    printf( "Type in all messages. Format: *Name* *body* *rec* *theme* *dd.mm.yyyy*\n" );
    for( ptrdiff_t i = 0; i < n; i++ ) {
        if( get_message( arr + i ) ) {
            i--;
            printf(INPUT_ERR);
            while ( ( getchar() ) != '\n' );
        }
    }

    // чтение имени пользователя
    char* user = malloc(32);
    if( user == NULL ) {
        printf(ALLOC_ERR);
        return 0;
    }
    while( scanf("%31s", user) != 1 ) {
        printf(INPUT_ERR);
        while ( ( getchar() ) != '\n' );
    }

    // чтение периода для поиска
    Date* period = malloc( SIZE_DATE * 2 );
    if( period == NULL ) {
        printf(ALLOC_ERR);
        return 0;
    }
    for( ptrdiff_t i = 0; i < 2; i++ ) {
        if( get_date( period + i ) ) {
            i--;
            printf(INPUT_ERR);
            while ( ( getchar() ) != '\n' );
        }
    }

    // вызов библиотечной функции
    Dict* res = run(&n, arr, user, period);
    if( res == NULL ) {
        printf(ALLOC_ERR);
        return 1;
    }
    // вывод результата
    for( size_t i = 0; i < n; i++ ) {
        printf("%s\n", res[i].theme);
    }

    // завершение программы
    free(res);
    free(period);
    free(user);
    free(arr);
    return 0;
}