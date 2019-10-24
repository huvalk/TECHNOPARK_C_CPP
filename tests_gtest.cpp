#include <dlfcn.h>
#include "gtest/gtest.h"
#include "include_cpp/cpp_d_user_messages.h"
#include "include_cpp/cpp_s_user_messages.h"

using namespace std;

class MyEnvironment : public ::testing::Environment {
public:
    virtual ~MyEnvironment() {}

    // Override this to define how to set up the environment.
    virtual void SetUp() override {
        library = dlopen("libdn_lib.so", RTLD_LAZY);
        imports = (exportVtable*) (dlsym(library, "exports"));
    }

    // Override this to define how to tear down the environment.
    virtual void TearDown() override {
        dlclose(library);
        imports = NULL;
    }

    static void *library;
    static exportVtable *imports;
};

void *MyEnvironment::library;
exportVtable *MyEnvironment::imports;

TEST(d, a) {
    MyEnvironment::imports->findMessages(0, 0, 0, 0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    AddGlobalTestEnvironment( new MyEnvironment );
    return RUN_ALL_TESTS();
}