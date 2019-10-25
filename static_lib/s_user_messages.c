#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <zconf.h>
#include "../common/mes_sort.h"

#define DEF_ARR_SIZE 2

extern size_t SIZE_DATE;
extern size_t SIZE_MESSAGE;
extern size_t SIZE_DICT;
extern size_t SIZE_SIZE_T;

// поиск сообщений пользователя в промежутке времени и сортировка по дате одним процессом
Dict *findMessages(size_t *const message_count, const Message *const messages,
                   const char *const user, const Date *const period,
                   bool (*cmp)(const Dict *const, const Dict *const)) {
  size_t cap = DEF_ARR_SIZE, size = 0;
  Dict *dict = calloc(cap, SIZE_DICT);
  if (dict == NULL)
    return NULL;
  for (size_t i = 0; i < *message_count; i++) {
    int8_t flag = inRecievers(user, (messages + i));
    if (flag == 1 && inPeriod(period, (messages + i))) {
      if (size >= cap) {
        Dict *nw_dict = calloc(cap * 2, SIZE_DICT);
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

// чтение из pipe в массив
Dict *readFromPipe(int *const fd, const size_t cores, size_t *const start,
                   size_t *const cur_size) {
  // считывание первого непустого Pipe
  do {
        read(fd[2 * (*start)], cur_size, SIZE_SIZE_T);
    (*start)++;
    if ((*cur_size) != 0) {
      break;
    }
  } while ((*start) < cores);

  // если ничего не найдено создается пустой
  if ((*start) == cores && *cur_size == 0) {
    Dict *res = calloc(1, SIZE_DICT);
    if (res == NULL) {
        return NULL;
    }
    *cur_size = 1;
    strcpy(res->theme, "Nothing were found");
    return res;
  }

  Dict *res = calloc((*cur_size), SIZE_DICT);
  if (res == NULL) {
      return NULL;
  }

  read(fd[2 * (*start - 1)], res, SIZE_DICT * (*cur_size));
  return res;
}

// слияние обработаных кусков от дочерних процессов
Dict *mergeFromPipe(int *const fd, const size_t cores, size_t *const chunk_len,
                    size_t *const message_count,
                    bool (*cmp)(const Dict *const, const Dict *const)) {
  // чтение первого массива для слияния
  size_t start = 0, cur_size = 0;
  Dict *res = readFromPipe(fd, cores, &start, &cur_size);
  if (res == NULL) {
      return NULL;
  }
  // последовательное слияение всех массивов в один большой
  for (size_t k = start; k < cores; k++) {
    // считывание длины новго массива. Наличие этих данных в pipe гарантировано
    // процессами-потомками
    size_t nw_size = 0;
    read(fd[2 * k], &nw_size, SIZE_SIZE_T);

    // если массив для слияния не пустой
    if (nw_size) {
      // создание буфера и считывание первого значение из pipe
      Dict *buf = malloc(SIZE_DICT * (nw_size + cur_size));
      if (buf == NULL) {
        free(res);
        return NULL;
      }
      size_t i = 0, j = 0;
      Dict *tmp = calloc(1, SIZE_DICT);
      if (tmp == NULL) {
        free(res);
        free(buf);
        return NULL;
      }
      read(fd[2 * k], tmp, SIZE_DICT);

      // слияние массива и pip
      while (i < cur_size && j < nw_size) {
        if (cmp(res + i, tmp)) {
          memcpy(buf + i + j, res + i, SIZE_DICT);
          i++;
        } else {
          memcpy(buf + i + j, tmp, SIZE_DICT);
          j++;
          // если последний элемент pipe не был считан, читаеся следующий
          if (j != nw_size)
            read(fd[2 * k], tmp, SIZE_DICT);
        }
      }

      // завершение слияния
      if (i == cur_size) {
        while (j < nw_size) {
          memcpy(buf + i + j, tmp, SIZE_DICT);
          j++;
          // если последний элемент pipe не был считан, читаеся следующий
          if (j != nw_size)
            read(fd[2 * k], tmp, SIZE_DICT);
        }
      } else {
        while (i < cur_size) {
          memcpy(buf + i + j, res + i, SIZE_DICT);
          i++;
        }
      }

      // освобожение буферов
      cur_size += nw_size;
      free(res);
      free(tmp);
      res = buf;
    }
  }

  for( size_t i = 0;  i < cores; i++ ) {
      close(fd[2 * i]);
  }

  *message_count = cur_size;
  return res;
}

// разбиение массива на куски
size_t getChunks(size_t *const chunk_len, const size_t cores,
                 const size_t message_count) {
  chunk_len[0] = (message_count) / cores;
  if (chunk_len[0] == 0) {
    chunk_len[0] = message_count;
    return 1;
  }
  for (size_t i = 1; i < cores - 1; i++) {
    chunk_len[i] = chunk_len[0];
  }
  if ((message_count) % cores == 0) {
    chunk_len[cores - 1] = chunk_len[0];
  } else {
    chunk_len[cores - 1] = (message_count) % cores;
  }

  return cores;
}

// запуск дочернего процесса для поиска писем пользователя
bool startChild(int *const fd, size_t *const chunk_len,
                const Message *const messages, const char *const user,
                const Date *const period) {
  // обработка куска массива
  Dict *dict = findMessages(chunk_len, messages, user, period, cmpDictMen);
  if (dict == NULL) {
      return true;
  }
  // возврат размера и значений через Pipe родительскому процессу
  write(*fd, chunk_len, SIZE_SIZE_T);
  if (*chunk_len != 0)
    write(*fd, dict, (*chunk_len) * SIZE_DICT);
  // освобождение памяти и завершение процесса
  free(dict);
}

// поиск сообщений пользователя в промежутке времени и сортировка по дате в несколько процессов
Dict *run(size_t *const message_count, const Message *const messages,
          const char *const user, const Date *const period) {
  if (message_count == 0 || messages == NULL || user == NULL ||
      period == NULL) {
    return NULL;
  }

  // количество доступных ядер
  const size_t cores = get_nprocs_conf();
  if (cores == 0)
    return NULL;

  // разбиение массива на куски
  size_t *chunk_len = calloc(cores, SIZE_SIZE_T);
  if (chunk_len == 0)
    return NULL;
  const size_t chunks = getChunks(chunk_len, cores, *message_count);

    //иницилизация pipe`ов
    int fd[2 * cores];
    for (int i = 0; i < chunks; i++) {
        pipe(&fd[2 * i]);
    }

  size_t offset = 0;
  // разбиение на процессы
  int status = 0;
  pid_t wpid = 0, pid = 0;
  // создание процесса на каждое ядро
  for (size_t i = 0; i < chunks; i++) {
    pid = fork();
    if (pid == 0) { // дочерний процесс
      // запуск поиска
      bool res = startChild(fd + 2 * i + 1, chunk_len + i, messages + offset,
                            user, period);
        for( size_t i = 0;  i < chunks; i++ ) {
            close(fd[i]);
        }
      if (res) {
        exit(EXIT_FAILURE);
      } else {
        exit(EXIT_SUCCESS);
      }
    } else if (pid < 0) {
      return NULL;
    } else { //родительский процесс
      // закрытие неиспользуемого дескриптора и переход к следующему куску
      // массива
      close(fd[2 * i + 1]);
      offset += chunk_len[i];
    }
  }

  *message_count = 0;
  // вызов слияния результатов дочерних процессов.
  // Если какой-то из процессов не успел дописать данные в Pipe, родитеский
  // будет заблокирован (см. документацию)
  Dict *res = mergeFromPipe(fd, chunks, chunk_len, message_count, cmpDictMen);
    for( size_t i = 0;  i < chunks; i++ ) {
        close(fd[2 * i]);
    }
  // освобожение памяти, возврат результата, проверка состояний процессов
  while (wpid = wait(&status) > 0)
    ;
  free(chunk_len);
  if (status != 0)
    return NULL;
  return res;
}