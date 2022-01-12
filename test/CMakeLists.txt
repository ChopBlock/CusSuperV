
# 查找 GTest 库
find_package(GTest REQUIRED)
# GTest 的头文件
include_directories(${GTEST_INCLUDE_DIRS})
# 测试项目的地址
include_directories(
    testclass/
    )
FILE(GLOB
    testsrc
    testclass/*cpp
    )
add_executable(test_my_class test_my_class.cc ${testsrc})
# 开启测试
enable_testing()

# 链接测试库
target_link_libraries( test_my_class
        ${GTEST_BOTH_LIBRARIES}

         ${project_libs}
        )


# 添加到测试
include(GoogleTest)
gtest_discover_tests(test_my_class)
