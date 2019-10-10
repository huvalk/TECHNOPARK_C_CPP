#ifndef IZ_1_STRUCTS_H
#define IZ_1_STRUCTS_H
#define DEF_SIZE 128

// Дата
typedef struct Date {
    u_int8_t day;
    u_int8_t mounth;
    unsigned short year;
} Date;

// договор
typedef struct Contract {
    float sum;
    Date date;
    char type[DEF_SIZE];
    char name[DEF_SIZE];
} Contract;

// элемент словаря
typedef struct Element {
    float sum;
    char name[DEF_SIZE];
} Element;

#endif //IZ_1_STRUCTS_H
