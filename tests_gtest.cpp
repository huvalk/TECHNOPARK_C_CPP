#include "gtest/gtest.h"
#include <fstream>
#include "statictic_cpp.h"
#define TEST_BASE "../test_base.dat"

std::istream& operator>> (std::istream &in, Contract &contract)
{
    in >> contract.name;
    in >> contract.type;
    in >> contract.sum;
    in >> contract.date.day;
    in >> contract.date.mounth;
    in >> contract.date.year;
    return in;
}

using namespace std;

TEST ( SortTest, SwapNormal ) {
    Element first = { 1, "first" };
    Element second = { 2, "second" };
    elem_swap( &first, &second );
    EXPECT_EQ( first.sum, 2 );
    EXPECT_EQ( second.sum, 1 );
    EXPECT_EQ( std::string( first.name ), "second" );
    EXPECT_EQ( std::string( second.name ), "first");
}

TEST ( SortTest, SwapWrong ) {
    Element first = { 1, "first" };
    try {
        elem_swap( &first, &first );
    } catch ( std::exception& e ) {
        ASSERT_TRUE( false );
    }
}

TEST ( SortTest, CopyNormal ) {
    Element first = { 0, "" };
    Contract second = { 2, { 1, 1, 1 }, "type", "second" };
    elem_cpy( &first, &second );
    EXPECT_EQ( first.sum, 2 );
    EXPECT_EQ( std::string( first.name ), "second" );
}

TEST ( SortTest, CopyWrong ) {
    Element first = { 0, "" };
    try {
        elem_cpy( &first, NULL );
    } catch ( std::exception& e ) {
        ASSERT_TRUE( false );
    }
}

TEST ( FindTest, Empty ) {
    size_t n = 0;
    Contract contracts[1] = {};
    Element first = {0, ""},
    second = {0, ""},
    third = {0, ""};

    find_top( contracts, &n, &first, &second, &third );
    EXPECT_EQ( first.sum, 0 );
    EXPECT_EQ( std::string( first.name ), "" );
    EXPECT_EQ( second.sum, 0 );
    EXPECT_EQ( std::string( second.name ), "" );
    EXPECT_EQ( third.sum, 0 );
    EXPECT_EQ( std::string( third.name ), "" );
}

TEST ( FindTest, OnlyOne ) {
    size_t n = 1;
    Contract contracts[n];
    ifstream mfile ( TEST_BASE );
    for( size_t i = 0; i < n; i++ ) {
        mfile >> contracts[i];
    }
    Element first = {0, ""}, second = {0, ""}, third = {0, ""};
    find_top( contracts, &n, &first, &second, &third );
    EXPECT_EQ( first.sum, 5.5 );
    EXPECT_EQ( std::string( first.name ), "awdse" );
    EXPECT_EQ( second.sum, 0 );
    EXPECT_EQ( std::string( second.name ), "" );
    EXPECT_EQ( third.sum, 0 );
    EXPECT_EQ( std::string( third.name ), "" );
    mfile.close();
}

TEST ( FindTest, OnlyTwo ) {
    size_t n = 2;
    Contract contracts[n];
    ifstream mfile ( TEST_BASE );
    for( size_t i = 0; i < n; i++ ) {
        mfile >> contracts[i];
    }
    Element first = {0, ""}, second = {0, ""}, third = {0, ""};
    find_top( contracts, &n, &first, &second, &third );
    EXPECT_EQ( first.sum, 7 );
    EXPECT_EQ( std::string( first.name ), "dfsef" );
    EXPECT_EQ( second.sum, 5.5 );
    EXPECT_EQ( std::string( second.name ), "awdse" );
    EXPECT_EQ( third.sum, 0 );
    EXPECT_EQ( std::string( third.name ), "" );
    mfile.close();
}

TEST ( FindTest, FullAnswer ) {
    size_t n = 10;
    Contract contracts[n];
    ifstream mfile ( TEST_BASE );
    for( size_t i = 0; i < n; i++ ) {
        mfile >> contracts[i];
    }
    Element first = {0, ""}, second = {0, ""}, third = {0, ""};
    find_top( contracts, &n, &first, &second, &third );
    EXPECT_EQ( first.sum, 75.5 );
    EXPECT_EQ( std::string( first.name ), "awdse" );
    EXPECT_EQ( second.sum, 26 );
    EXPECT_EQ( std::string( second.name ), "secf" );
    EXPECT_EQ( third.sum, 7 );
    EXPECT_EQ( std::string( third.name ), "dfef" );
    mfile.close();
}

TEST ( FindTest, WrongParams ) {
    size_t n = 4;
    Element first = {0, ""}, second = {0, ""}, third = {0, ""};
    try {
        find_top( NULL, &n, &first, &second, &third );
    } catch ( std::exception& e ) {
        ASSERT_TRUE( false );
    }


}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}