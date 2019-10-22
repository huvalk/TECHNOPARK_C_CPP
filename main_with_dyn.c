#include <dlfcn.h>
#include "mes/message.h"
#include <stdio.h>
#include <stdlib.h>

#define ALLOC_ERR "Allocation error. Restart the program.\n"
#define INPUT_ERR "Input error. Try again\n"

extern size_t SIZE_DATE;
extern size_t SIZE_MESSAGE;
extern size_t SIZE_DICT;

void *library;
struct export_vtable {
    Dict* (*find_messages)(size_t* const message_count, const Message* const messages, char* const user, const Date* const period);
};

int main() {
    // подключение динамической библиотеки
    library = dlopen("libdn_lib.so", RTLD_LAZY);
    struct export_vtable* imports;
    if(!library) { printf("lib_err"); return 0; }

    imports = dlsym(library, "exports");
    if(!imports) { printf("import_err"); return 0; }

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
    Dict* res = imports->find_messages(&n, arr, user, period);

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