#ifndef GASIOTEST_H
#define GASIOTEST_H


#include <gtest/gtest.h>
#include"asiotest.h"
class GAsioTest:public ::testing::Test
{
public:
    GAsioTest();

    // Test interface
protected:
    void SetUp() override;

private:
    void TestBody() override;
};

#endif // GASIOTEST_H
