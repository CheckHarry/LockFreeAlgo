


include_directories(../include)

add_executable(FixedSizeLockFreeQueueTest
        main.cpp
        FixedSizeLockFreeQueueTest_MPSC.cpp
        FixedSizeLockFreeQueueTest_MPMC.cpp)
target_link_libraries(FixedSizeLockFreeQueueTest GTest::gtest_main)