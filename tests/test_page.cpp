#include <gtest/gtest.h>
#include "../lib/node.h"
#include "../lib/page.h"
#include <cstring>
#include <stdexcept>
#include <iostream>

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
    EXPECT_EQ(page.getFirstOpenSlot(), 0 );
    EXPECT_EQ(page.getOpenSlots(), Page<Node>::PAGESIZE/Node::SERIALIZED_SIZE);
}

TEST_F(PageTest, insertGetTestEmptyPage){
    Page<Node> page(0);
    Node node = Node("TestNode");
    page.serializeAndInsertObject(node);
    EXPECT_EQ(page.getFirstOpenSlot(), 1);
    EXPECT_EQ(page.getPageObject(0).getLabel(),"TestNode");
}


TEST_F(PageTest, insertGetTestPartlyFullPage){
    Page<Node> page(0);
    for(int i = 0; i < 10; i++){
       Node node = Node(std::to_string(i));
       page.serializeAndInsertObject(node); 
       EXPECT_EQ(page.getFirstOpenSlot(), i+1);
       EXPECT_EQ(page.getPageObject(i).getLabel(),std::to_string(i));
    }
}

TEST_F(PageTest, insertGetTestFullPage){
    Page<Node> page(0);
    while(page.getOpenSlots() > 0){
        //std::cout << page.getFirstOpenSlot() << std::endl;
        Node node = Node("test");
        page.serializeAndInsertObject(node);
    }
    Node finalNode = Node("FINAL");
    EXPECT_THROW(page.serializeAndInsertObject(finalNode),std::logic_error);
}

TEST_F(PageTest, deleteNodeTest){
    Page<Node> page(0);
    Node node = Node("test");
    page.serializeAndInsertObject(node);
    page.deleteObject(0);
    Node deletedNode = page.getPageObject(0);
    Node notdeletedNode = Node("testr");
    EXPECT_EQ(deletedNode.getUseState(), false);
    EXPECT_EQ(notdeletedNode.getUseState(), true);
}

