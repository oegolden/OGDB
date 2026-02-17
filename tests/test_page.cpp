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
    Page<Node> page;
    EXPECT_EQ(page.getFirstOpenSlot(), 0 );
    EXPECT_EQ(page.getOpenSlots(), Page<Node>::PAGESIZE/Node::SERIALIZED_SIZE);
}

TEST_F(PageTest, insertGetTestEmptyPage){
    Page<Node> page;
    Node node = Node("TestNode");
    page.insertObject(node);
    EXPECT_EQ(page.getFirstOpenSlot(), 1);
    EXPECT_EQ(page.getObject(0).getLabel(),"TestNode");
}


TEST_F(PageTest, insertGetTestPartlyFullPage){
    Page<Node> page;
    for(int i = 0; i < 10; i++){
       Node node = Node(std::to_string(i));
       page.insertObject(node); 
       EXPECT_EQ(page.getFirstOpenSlot(), i+1);
       EXPECT_EQ(page.getObject(i).getLabel(),std::to_string(i));
    }
}

TEST_F(PageTest, insertGetTestFullPage){
    Page<Node> page;
    while(page.getOpenSlots() > 0){
        //std::cout << page.getFirstOpenSlot() << std::endl;
        Node node = Node("test");
        page.insertObject(node);
    }
    Node finalNode = Node("FINAL");
    EXPECT_THROW(page.insertObject(finalNode),std::logic_error);
}

TEST_F(PageTest, removeNodeTest){
    Page<Node> page;
    Node node = Node("test");
    page.insertObject(node);
    page.removeObject(0);
    Node deletedNode = page.getObject(0);
    Node notdeletedNode = Node("testr");
    EXPECT_EQ(deletedNode.getUseState(), false);
    EXPECT_EQ(notdeletedNode.getUseState(), true);
}

TEST_F(PageTest, SerializeDeserializeTest){
    Page<Node> page;
    Node a = Node("Alice");
    Node b = Node("Bob");
    page.insertObject(a);
    page.insertObject(b);

    auto buf = page.serializePage();
    // construct a new page from the serialized buffer
    Page<Node> page2(buf.get());

    // verify objects preserved
    EXPECT_EQ(page2.getObject(0).getLabel(), "Alice");
    EXPECT_EQ(page2.getObject(1).getLabel(), "Bob");

    // verify open slots count matches original
    EXPECT_EQ(page2.getOpenSlots(), page.getOpenSlots());
}

