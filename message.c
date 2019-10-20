#define DEF_STR_SIZE 32;

typedef struct Date {
    u_int8_t day;
    u_int8_t mounth;
    unsigned short year;
} Date;

typedef struct Message
{
    Date date;
    char user_name[DEF_STR_SIZE];
    char body[10*DEF_STR_SIZE];
    char recievers[10*DEF_STR_SIZE];
    char theme[4*DEF_STR_SIZE];
} Message;

bool get_message( Message* current )
{

}

bool cmo_date( const date* const left, const date* const right )
{

}