#include "FixedSizeLockFreeQueueTestHelper.h"


TEST_F(FixedSizeLockFreeQueueTestFixture, MultipleWriterSingleReaderTestCase) {
    EXPECT_TRUE(TestMPSC(10, 1, 50000));
    EXPECT_TRUE(TestMPSC(10, 2, 50000));
    EXPECT_TRUE(TestMPSC(10, 20, 50000));
    EXPECT_TRUE(TestMPSC(10, 200, 50000));
    EXPECT_TRUE(TestMPSC(10, 2000, 50000));
}


