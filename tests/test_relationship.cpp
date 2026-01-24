#include <gtest/gtest.h>
#include "relationship.h"
#include <stdexcept>

class RelationshipTest : public ::testing::Test {
protected:
    Relationship rel;
};

// Getter Tests
TEST_F(RelationshipTest, GetInUse) {
    rel.setInUse(true);
    EXPECT_TRUE(rel.getInUse());
}

TEST_F(RelationshipTest, GetFirstNode) {
    rel.setFirstNode(42);
    EXPECT_EQ(42, rel.getFirstNode());
}

TEST_F(RelationshipTest, GetSecondNode) {
    rel.setSecondNode(99);
    EXPECT_EQ(99, rel.getSecondNode());
}

TEST_F(RelationshipTest, GetRelationshipType) {
    rel.setRelationshipType("OWNS");
    EXPECT_EQ("OWNS", rel.getRelationshipType());
}

TEST_F(RelationshipTest, GetFirstPrevRelId) {
    rel.setFirstPrevRelId(10);
    EXPECT_EQ(10, rel.getFirstPrevRelId());
}

TEST_F(RelationshipTest, GetFirstNextRelId) {
    rel.setFirstNextRelId(20);
    EXPECT_EQ(20, rel.getFirstNextRelId());
}

TEST_F(RelationshipTest, GetSecondPrevRelId) {
    rel.setSecondPrevRelId(30);
    EXPECT_EQ(30, rel.getSecondPrevRelId());
}

TEST_F(RelationshipTest, GetSecondNextRelId) {
    rel.setSecondNextRelId(40);
    EXPECT_EQ(40, rel.getSecondNextRelId());
}

TEST_F(RelationshipTest, GetNextPropId) {
    rel.setNextPropId(50);
    EXPECT_EQ(50, rel.getNextPropId());
}

TEST_F(RelationshipTest, GetFirstInChainMarker) {
    rel.setFirstInChainMarker(true);
    EXPECT_TRUE(rel.getFirstInChainMarker());
}

// Setter Tests
TEST_F(RelationshipTest, SetRelationshipTypeValid) {
    EXPECT_NO_THROW(rel.setRelationshipType("KNOW"));
    EXPECT_EQ("KNOW", rel.getRelationshipType());
}

TEST_F(RelationshipTest, SetRelationshipTypeMaxLength) {
    EXPECT_NO_THROW(rel.setRelationshipType("OWNS"));
    EXPECT_EQ("OWNS", rel.getRelationshipType());
}

TEST_F(RelationshipTest, SetRelationshipTypeTooLong) {
    EXPECT_THROW(rel.setRelationshipType("TOOLONG"), std::length_error);
}

TEST_F(RelationshipTest, SetRelationshipTypeEmpty) {
    EXPECT_THROW(rel.setRelationshipType(""), std::length_error);
}

TEST_F(RelationshipTest, MultipleSettersChain) {
    rel.setFirstNode(1);
    rel.setSecondNode(2);
    rel.setRelationshipType("LINK");
    rel.setInUse(true);
    
    EXPECT_EQ(1, rel.getFirstNode());
    EXPECT_EQ(2, rel.getSecondNode());
    EXPECT_EQ("LINK", rel.getRelationshipType());
    EXPECT_TRUE(rel.getInUse());
}