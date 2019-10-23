#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include "../mes/message.h"

#define DEF_ARR_SIZE 2

extern size_t SIZE_DATE;
extern size_t SIZE_MESSAGE;
extern size_t SIZE_DICT;

void Merge( const Dict* const left, const size_t left_size, const Dict* const right, const size_t right_size,
            Dict* const buf, bool (*cmp)( const Date* const, const Date* const ) )
{
    size_t i = 0, j = 0;
    // сравение идет по первому значению в кортеже
    while( i < left_size && j < right_size ) {
        if( cmp( &left[i].date, &right[j].date ) ) {
            memcpy( buf + i + j, left + i, SIZE_DICT );
            i++;
        } else {
            memcpy( buf + i + j, right + j, SIZE_DICT );
            j++;
        }
    }

    // если в одном из массивов остались элементы
    if( i == left_size ) {
        while( j < right_size) {
            memcpy( buf + i + j, right + j, SIZE_DICT );
            j++;
        }
    } else {
        while( i < left_size ) {
            memcpy( buf + i + j, left + i, SIZE_DICT );
            i++;
        }
    }
}

void Sort( Dict* const arr, const size_t size, bool (*cmp)( const Date* const, const Date* const ) )
{
    if( size < 2)
        return;

    // выделение длины подмассивов для рекурсивной сортировки
    size_t left_size = size / 2;
    size_t right_size = size - left_size;
    Sort( arr, left_size, cmp );
    Sort( arr + left_size, right_size, cmp );

    // буферный массив для слияния
    Dict* new_arr = malloc(size * SIZE_DICT);

    // слияние
    Merge( arr, left_size, arr + left_size, right_size, new_arr, cmp );

    // копирование буферного массива в основной
    memcpy(arr, new_arr, size * SIZE_DICT);

    free(new_arr);

    return;
}

Dict* FindMessages( size_t* const message_count, const Message* const messages,
                     char* const user, const Date* const period, bool (*cmp)( const Date* const, const Date* const ) )
{
    size_t cap = DEF_ARR_SIZE, size = 0;
    Dict* dict = calloc(cap, SIZE_DICT);
    if(dict == NULL)
        return NULL;
    for(size_t i = 0; i < *message_count; i++) {
        if( in_period(period, (messages + i)) && in_recievers(user, (messages + i)) ) {
            if( size >= cap ) {
                Dict* nw_dict = malloc( cap * SIZE_DICT * 2 );
                if(nw_dict == NULL)
                    return NULL;
                memcpy(nw_dict, dict, cap * SIZE_DICT);

                cap *= 2;
                free(dict);
                dict = nw_dict;
            }

            for( size_t i = 0; i < size; i++ ) {
                printf("%s, ", dict[i].theme);
            }
            puts("\n");

            to_dict_elem((messages + i), (dict + size));
            size++;

            for( size_t i = 0; i < size; i++ ) {
                printf("%s, ", dict[i].theme);
            }
            puts("\n");
        }
    }

    Sort(dict, size, cmp);

    *message_count = size;
    return dict;
}

Dict* MergeFromPipe( int* const fd, const size_t cores, size_t* const chunk_len, size_t* const message_count,
            bool (*cmp)( const Date* const, const Date* const ) )
{
    // считывание первого непустого Pipe
    size_t start = 0, cur_size = 0;
    do {
        read(fd[start], &cur_size, sizeof(size_t));
        start++;
        if (cur_size == 0) {
            close(fd[2 * start]);
        } else {
            break;
        }
    } while ( start < cores );

    Dict* res = calloc( cur_size, SIZE_DICT );
    if( res == NULL )
        return NULL;
    if( start == cores ) {
        strcpy( res->theme, "Nothing were found" );
        *message_count = 1;
        return res;
    }

    read(fd[2 * start], res, SIZE_DICT * cur_size);
    close(fd[2 * start]);

    // если разбиение не было - выход
    if( cores == 1 )
        return  res;

    // последовательное слияение всех массивов в один большой
    for( size_t k = start; k < cores; k++ ) {
        // считывание длины новго массива. Наличие этих данных в pipe гарантировано процессами-потомками
        size_t nw_size  = 0;
        read(fd[2 * k], &nw_size, sizeof(size_t));

        // если массив для слияния не пустой
        if( nw_size ) {
            // создание буфера и считывание первого значение из pipe
            Dict* buf = malloc( SIZE_DICT * ( nw_size + cur_size ) );
            if( buf == NULL )
                return NULL;
            size_t i = 0, j = 0;
            Dict *tmp = calloc(1, SIZE_DICT);
            if( tmp == NULL )
                return NULL;
            read(fd[2 * k], tmp, SIZE_DICT);

            // слияние массива и pip
            while( i < cur_size && j < nw_size ) {
                if( cmp( &res[i].date, &tmp->date ) ) {
                    // TODO копирование
                    memcpy( buf + i + j, res + i, SIZE_DICT );
                    i++;
                } else {
                    // TODO копирование
                    memcpy( buf + i + j, tmp, SIZE_DICT );
                    j++;
                    // если последний элемент pipe не был считан, читаеся следующий
                    if (j != nw_size ) read(fd[2 * k], tmp, SIZE_DICT);
                }
            }

            // завершение слияния
            if( i == cur_size ) {
                while( j < nw_size ) {
                    // TODO копирование
                    memcpy( buf + i + j, tmp, SIZE_DICT );
                    j++;
                    // если последний элемент pipe не был считан, читаеся следующий
                    if (j != nw_size ) read(fd[2 * k], tmp, SIZE_DICT);
                }
            } else {
                while( i < cur_size ) {
                    // TODO копирование
                    memcpy( buf + i + j, res + i, SIZE_DICT );
                    i++;
                }
            }

            // освобожение буферов
            cur_size += nw_size;
            free(res);
            free(tmp);
            res = buf;
        }
        // закрытие использованного дескриптора
        close(fd[2 * k]);
    }

    *message_count = cur_size;
    return res;
}

size_t GetChunks( size_t* const chunk_len, const size_t cores, const size_t message_count)
{
    chunk_len[0] = (message_count)/cores;
    if( chunk_len[0] == 0 ) {
        chunk_len[0] = message_count;
        return 1;
    }
    for( size_t i = 1; i < cores - 1; i++ ) {
        chunk_len[i] = chunk_len[0];
    }
    if( (message_count) % cores == 0 ) {
        chunk_len[cores - 1] = chunk_len[0] ;
    } else {
        chunk_len[cores - 1] = (message_count) % cores;
    }

    return cores;
}

Dict* run( size_t* const message_count, Message* const messages, char* const user, const Date* const period )
{
    if( message_count ==  0 || messages == NULL || user == NULL || period == NULL ) {
        return NULL;
    }

    // количество доступных ядер
    const size_t cores = get_nprocs_conf();
    if(cores == 0)
        return NULL;

    //иницилизация pipe`ов
    int fd[2 * cores];
    for (int i = 0; i < cores; i++) {
        pipe(&fd[2*i]);
    }

    // разбиение массива на куски
    size_t* chunk_len = calloc( cores, sizeof(size_t) );
    if(chunk_len == 0)
        return NULL;
    const size_t chunks = GetChunks( chunk_len, cores, *message_count );


    Message* arr = messages;
    // разбиение на процессы
    int status = 0;
    pid_t wpid = 0, pid = 0;
    // создание процесса на каждое ядро
    for( size_t i = 0; i < chunks; i++ ) {
        pid = fork();
        if( pid == 0 ) {                // дочерний процесс
            // обработка куска массива
            Dict* dict = FindMessages(chunk_len + i, arr, user, period, cmp_date_men);
            if(dict == NULL)
                return NULL;
            // возврат размера и значений через Pipe родительскому процессу
            write(fd[2 * i + 1], chunk_len + i, sizeof(size_t));
            if(chunk_len[i] != 0)
                write(fd[2 * i + 1], dict, chunk_len[i]);
            // закрытие использованного дескриптора
            close(fd[2 * i + 1]);
            // освобождение памяти и завершение процесса
            free(dict);
            exit(0);
        } else if ( pid < 0 ) {
            return NULL;
        } else {                 //родительский процесс
            // закрытие неиспользуемого дескриптора и переход к следующему куску массива
            close(fd[2 * i + 1]);
            arr += chunk_len[i];
        }
    }

    *message_count = 0;
    // вызов слияния результатов дочерних процессов.
    // Если какой-то из процессов не успел дописать данные в Pipe, родитеский будет заблокирован (см. документацию)
    Dict* res = MergeFromPipe(fd, chunks, chunk_len, message_count, cmp_date_men);

    // освобожение памяти, возврат результата, проверка состояний процессов
    while ((wpid = wait(&status)) > 0);
    if (wpid < 0 )
        return NULL;

    free(chunk_len);
    return res;
}