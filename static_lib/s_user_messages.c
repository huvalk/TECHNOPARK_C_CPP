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

Dict* find_messages( size_t* const message_count, const Message* const messages,
                     char* const user, const Date* const period, bool (*cmp)( const Date* const, const Date* const ) )
{
    size_t cap = DEF_ARR_SIZE, size = 0;
    Dict* dict = calloc(cap, SIZE_DICT);
    for(size_t i = 0; i < *message_count; i++) {
        if( in_period(period, (messages + i)) && in_recievers(user, (messages + i)) ) {
            if( size >= cap ) {
                Dict* nw_dict = malloc( cap * SIZE_DICT * 2 );
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
    size_t cur_size = 0;
    read(fd[0], &cur_size, sizeof(size_t));
    Dict* res = malloc( SIZE_DICT * cur_size );

    read(fd[0], res, SIZE_DICT * cur_size);
    close(fd[0]);

    if( cores == 1 )
        return  res;

    for( size_t k = 1; k < cores; k++ ) {
        size_t nw_size  = 0;
        read(fd[2 * k], &nw_size, sizeof(size_t));

        if( nw_size ) {
            Dict* buf = malloc( SIZE_DICT * ( nw_size + cur_size ) );
            size_t i = 0, j = 0;
            Dict *tmp = calloc(1, SIZE_DICT);
            read(fd[2 * k], tmp, SIZE_DICT);

            while( i < cur_size && j < nw_size ) {
                if( cmp( &res[i].date, &tmp->date ) ) {
                    memcpy( buf + i + j, res + i, SIZE_DICT );
                    i++;
                } else {
                    memcpy( buf + i + j, tmp, SIZE_DICT );
                    j++;
                    if (j != nw_size ) read(fd[2 * k], tmp, SIZE_DICT);
                }
            }

            if( i == cur_size ) {
                while( j < nw_size ) {
                    memcpy( buf + i + j, tmp, SIZE_DICT );
                    j++;
                    if (j != nw_size ) read(fd[2 * k], tmp, SIZE_DICT);
                }
            } else {
                while( i < cur_size ) {
                    memcpy( buf + i + j, res + i, SIZE_DICT );
                    i++;
                }
            }

            cur_size += nw_size;
            free(res);
            res = buf;
        }
        close(fd[2 * k]);
    }

    *message_count = cur_size;
    return res;
}

Dict* run( size_t* const message_count, const Message* const messages, char* const user, const Date* const period )
{
    const size_t cores = get_nprocs_conf();
    if(cores == 0)
        return NULL;

    //иницилизация pipe`ов
    int fd[2 * cores];
    for (int i = 0; i < cores; i++) {
        pipe(&fd[2*i]);
    }

    // разбиение массива на куски
    size_t* chunk_len = malloc( (cores + 1) * sizeof(size_t) );
    chunk_len[0] = (*message_count)/cores;
    for( size_t i = 1; i < cores - 1; i++ ) {
        chunk_len[i] = chunk_len[0];
    }
    if( (*message_count) % cores == 0 ) {
        chunk_len[cores - 1] = chunk_len[0] ;
    } else {
        chunk_len[cores - 1] = (*message_count) % cores;
    }

    Message* arr = messages;

    int status = 0;
    size_t * ready_to_pass = mmap(NULL, sizeof(size_t), PROT_READ | PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    *ready_to_pass = 0;
    pid_t* pid = calloc( cores + 1, sizeof(pid_t) );
    pid[0] = 1;
    for( size_t i = 1; i <= cores; i++ ) {
        if( pid[i - 1] > 0 ) {
            pid[i] = fork();
            if( pid[i] == 0 ) {
                Dict* dict = find_messages(chunk_len[i - 1], arr, user, period, cmp_date_men);
                write(fd[2*(i - 1) + 1], chunk_len + i - 1, sizeof(size_t));
                ready_to_pass += 1;
                write(fd[2*(i - 1) + 1], dict, chunk_len[i - 1]);
                close(fd[2*(i - 1) + 1]);
                free(dict);
                exit(0);
            } else if ( pid[i] < 0 ) {
                //err
            } else {
                close(fd[2*(i - 1) + 1]);
                arr += chunk_len[i - 1];
            }
        }
    }

    while (ready_to_pass != cores) {};
    *message_count = 0;

    Dict* res = MergeFromPipe(fd, cores, chunk_len, message_count, cmp_date_men);
    free(chunk_len);
    free(pid);
    free(pid);
    return res;
}