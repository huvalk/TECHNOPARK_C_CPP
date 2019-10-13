#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <zconf.h>
#include "stddef.h"

#include "statistic.h"

#define ALLOC_ERR printf("Allocation error. Restart the program.\n");
#define INPUT_ERR printf("Input error. Try again\n");
#define DEF_SIZE 128

extern size_t SIZE_DATA;
extern size_t SIZE_ELEMENT;
extern size_t SIZE_CONTRACT;

int main()
{
    // инициализация массива договоров
    size_t n = 0;
    printf( "How many contracts you wold like to insert?\n" );
    while( scanf("%lu", &n) != 1 ) {
        INPUT_ERR
        while ( ( getchar() ) != '\n' );
    }

    if( n <= 1 ) {
        return 0;
    }

    Contract* arr = malloc(n * SIZE_CONTRACT);
    if( arr == NULL ) {
        ALLOC_ERR
        return 0;
    }
    memset( arr, 0, n * SIZE_CONTRACT );

    //чтение массива договоров
    printf( "Type in all contracts. Format: *Name* *Cost* *Type* *dd.mm.yyyy*\n" );
    for( ptrdiff_t i = 0; i < n; i++ ) {
        if( contract_read( arr + i ) ) {
            i--;
            INPUT_ERR
            while ( ( getchar() ) != '\n' );
        }
    }

    // инициализация переменных для сохранения результата и передача управления функции
    Element first = {0, ""}, second = {0, ""}, third = {0, ""};
    find_top( arr, &n, &first, &second, &third );

    char* format_str = malloc( 40 );
    strcpy( format_str, "First - %s\n" );
    if ( n >= 2 ) {
        strcat( format_str, "Second - %s\n" );
        if ( n >= 3 ) {
            strcat( format_str, "Third - %s\n" );
        }
    }

    printf( format_str, first.name, second.name, third.name );
    free( arr );
    return 0;
}
