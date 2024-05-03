#include "FixedSizeLockFreeQueueTestHelper.h"


TEST_F(FixedSizeLockFreeQueueTestFixture, MultipleWriterMultipleReaderTestCase) {
    EXPECT_TRUE(TestMPMC(10, 5, 1, 50000));
    EXPECT_TRUE(TestMPMC(10, 5, 2, 50000));
    EXPECT_TRUE(TestMPMC(10, 5, 20, 50000));
    EXPECT_TRUE(TestMPMC(10, 5, 200, 50000));
    EXPECT_TRUE(TestMPMC(10, 5, 2000, 50000));
}

