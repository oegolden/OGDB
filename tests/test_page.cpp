#include <gtest/gtest.h>
#include "../lib/node.h"
#include "../lib/page.h"
#include <cstring>
#include <stdexcept>

class PageTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(PageTest, ConstructorTest){
    Page<Node> page(0);
    EXPECT_EQ(page.getPageId(), 0);
    EXPECT_EQ(page.getFirstpenSlot(), 0 );
    EXPECT_EQ(page.getOpenSlots(), Page<Node>::PAGESIZE/sizeof(Node));
}

