#include <cstdlib>
#include "include_cpp/cpp_d_user_messages.h"
#include "include_cpp/cpp_s_user_messages.h"
#include "gtest/gtest.h"
#include <dlfcn.h>
#include <fstream>

#define BASE_DICT "../test_base_dict.dat"
#define SORT_DICT "../test_sort_dict.dat"

using namespace std;

//std::istream& operator>> (std::istream &in, Message &mes)
//{
//    in >> mes.date;
//    in >> mes.type;
//    in >> mes.sum;
//    in >> mes.date.day;
//    in >> mes.date.mounth;
//    in >> mes.date.year;
//    return in;
//}
//
//std::istream& operator>> (std::istream &in, Dict &dict)
//{
//    in >> dict.date.day;
//    in >> dict.date.mounth;
//    in >> dict.date.year;
//    in >> dict.theme;
//
//    return in;
//}
//
//std::ostream& operator<< (std::ostream &in, Dict &dict)
//{
//    in << dict.date.day;
//    in << dict.date.mounth;
//    in << dict.date.year;
//    in << dict.theme;
//
//    return in;
//}

void genMessages( Message *mes, size_t size) {
    char *buf = new char[4];
    for( size_t i = 0; i < size; i++) {
        mes[i].date.year = rand()%10 + 1;
        mes[i].date.mounth = rand()%10 + 1;
        mes[i].date.day = rand()%10 + 1;
        buf[0] = '/';
        buf[1] = 'B';
        buf[2] = '.';
        buf[3] = 0;
        strcpy(mes[i].recievers, buf);
        char *buf_name = new char[DEF_STR_SIZE * 20];
        char *buf_body = buf_name + DEF_STR_SIZE;
        char *buf_theme = buf_name + DEF_STR_SIZE * 16;
        strcpy(buf_theme, buf);
        mes[i].body = buf_body;
        mes[i].theme = buf_theme;
        mes[i].user_name = buf_name;
    }
    delete[] buf;
}

class MyEnvironment : public ::testing::Environment {
public:
  virtual ~MyEnvironment() {}

  // Override this to define how to set up the environment.
  virtual void SetUp() override {
    library = dlopen("libdn_lib.so", RTLD_LAZY);
    imports = (exportVtable *)(dlsym(library, "exports"));
    srand(unsigned(time(0)));
      period = new Date[2];
      period[0].day = 0;
      period[0].mounth = 0;
      period[0].year = 0;
      period[1].day = 100;
      period[1].mounth = 100;
      period[1].year = 100;
  }

  // Override this to define how to tear down the environment.
  virtual void TearDown() override {
    dlclose(library);
    imports = NULL;
    delete[] period;
  }
    static Date *period;
  static void *library;
  static exportVtable *imports;
};

void *MyEnvironment::library;
exportVtable *MyEnvironment::imports;
Date *MyEnvironment::period;

TEST(Message, toDictElem) {
    Message *mes = new Message{{1, 1, 1}};
    mes->body = (char *)"b";
    mes->user_name = (char *)"n";
    mes->theme = (char *)"t";
    strcpy(mes->recievers, "/r.");

    Dict* dict = new Dict;
    toDictElem(mes, dict);

    EXPECT_EQ( dict->date.day, mes->date.day );
    EXPECT_EQ( dict->date.mounth, mes->date.mounth );
    EXPECT_EQ( dict->date.year, mes->date.year );
    EXPECT_EQ( std::string( dict->theme ), std::string( mes->theme ) );
}

TEST(Message, cmpDictMen) {
    Dict *left = new Dict{{1, 2, 1}};
    strcpy(left->theme, "d");
    Dict *right = new Dict{{2, 1, 2}};
    strcpy(right->theme, "f");


    EXPECT_EQ( cmpDictMen(left, right), true);

    delete left;
    delete right;
}

TEST(Message, cmpDateMen) {
    Date left = {1, 2, 1};
    Date right = {2, 1, 2};

    EXPECT_EQ( cmpDateMen(&left, &right), true);
}

TEST(Message, cmpDictEq) {
    Dict *left = new Dict{{1, 1, 1}};
    strcpy(left->theme, "d");
    Dict *right = new Dict{{1, 1, 1}};
    strcpy(right->theme, "f");


    EXPECT_EQ( cmpDictEq(left, right), true);

    delete left;
    delete right;
}

TEST(Message, cmpDateEq) {
    Date left = {1, 1, 1};
    Date right = {1, 1, 1};

    EXPECT_EQ( cmpDateEq(&left, &right), true);
}

TEST(Message, swapDict) {
    Dict *left = new Dict{{1, 2, 1}};
    strcpy(left->theme, "d");
    Dict *right = new Dict{{2, 1, 2}};
    strcpy(right->theme, "f");
    swapDict(left, right);

    EXPECT_EQ( string(left->theme), "f"s);
    EXPECT_EQ( string(right->theme), "d"s);

    delete left;
    delete right;
}

TEST(Message, inPeriod) {
    Date *dates = new Date[3];
    dates[0] = {1, 2, 1};
    dates[1] = {1, 5, 2};
    Message *mes = new Message{1, 3, 2};

    EXPECT_EQ( inPeriod(dates, mes), true);

    delete[] dates;
    delete mes;
}

TEST(Message, inRecievers) {
    Message *mes = new Message{1, 3, 2};
    strcpy(mes->recievers, "/af./fa.");
    char *user= new char[4];
    strcpy(user, "fa");
    EXPECT_EQ( inRecievers(user, mes), true);
    strcpy(user, "f");
    EXPECT_EQ( inRecievers(user, mes), false);

    delete[] user;
    delete mes;
}

TEST(sort, heap) {
    size_t size = 10;
    Dict *dict = new Dict[size];
    for( size_t i = 0; i < size; i++) {
        dict[i].date.year = rand()%10;
        dict[i].date.mounth = rand()%10;
        dict[i].date.day = rand()%10;
    }

    sort(dict, size, cmpDictMen);

    bool flag = true;

    for( size_t i = 0; i < size-1 && flag; i++) {
        if(cmpDictMen(dict + i + 1, dict + i) || cmpDictEq(dict + i + 1, dict + i )) {
            flag = false;
            printf("%hhu, %hhu, %hu - ", dict[i].date.day, dict[i].date.mounth, dict[i].date.year);
            printf("%hhu, %hhu, %hu\n", dict[i+1].date.day, dict[i+1].date.mounth, dict[i+1].date.year);
        }
    }

    EXPECT_EQ( flag, true);

    delete[] dict;
}

// данных меньше, чем ядер
TEST(findMessages, heap1) {
    size_t size = 4;
    Message *mes = new Message[size];

    genMessages(mes, size);

    char *user = new char[2];
    strcpy(user, "B");


    Dict *s_res = run(&size, mes, user, MyEnvironment::period);
    Dict *d_res = MyEnvironment::imports->findMessages(&size, mes, user, MyEnvironment::period);

    delete[] user;

    bool flag = true;
    for( size_t i = 0; i < size && flag; i++) {
        if(!cmpDictEq(s_res + i, d_res + i))
            flag = false;
        delete[] mes[i].user_name;
    }

    EXPECT_EQ( flag, true);

    delete[] d_res;
    delete[] s_res;
}

// данных больше, чем ядер
TEST(findMessages, heap2) {
    size_t size = 21;
    Message *mes = new Message[size];

    genMessages(mes, size);

    char *user = new char[2];
    strcpy(user, "B");


    Dict *s_res = run(&size, mes, user, MyEnvironment::period);
    
    Dict *d_res = MyEnvironment::imports->findMessages(&size, mes, user, MyEnvironment::period);
    delete[] user;

    bool flag = true;
    for( size_t i = 0; i < size && flag; i++) {
        if( !cmpDictEq(s_res + i, d_res + i))
            flag = false;
        delete[] mes[i].user_name;
    }

    EXPECT_EQ( flag, true );

    delete[] d_res;
    delete[] s_res;
}

// данных больше, чем ядер
TEST(findMessages, heap3) {
    size_t size = 1000000;
    Message *mes = new Message[size];

    genMessages(mes, size);

    char *user = new char[2];
    strcpy(user, "B");


    Dict *s_res = run(&size, mes, user, MyEnvironment::period);
    
    Dict *d_res = MyEnvironment::imports->findMessages(&size, mes, user, MyEnvironment::period);
    delete[] user;

    cout << size << '\n';

    bool flag = true;
    for( size_t i = 0; i < size && flag; i++) {
        if( !cmpDictEq(s_res + i, d_res + i)) {
            flag = false;
            printf("%hhu, %hhu, %hu - ", s_res[i].date.day, s_res[i].date.mounth, s_res[i].date.year);
            printf("%hhu, %hhu, %hu\n", d_res[i].date.day, d_res[i].date.mounth, d_res[i].date.year);
        }

        delete[] mes[i].user_name;
    }

    EXPECT_EQ( flag, true );

    delete[] d_res;
    delete[] s_res;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  AddGlobalTestEnvironment(new MyEnvironment);
  return RUN_ALL_TESTS();
}