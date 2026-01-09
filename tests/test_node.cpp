#include <gtest/gtest.h>
#include "../lib/node.h"
#include <cstring>
#include <stdexcept>
using namespace std;

// Test fixture for Node tests
class NodeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test constructor with valid label
TEST_F(NodeTest, ConstructorWithValidLabel) {
    Node node("TestLabel");
    EXPECT_EQ(node.getLabel(), "TestLabel");
    EXPECT_EQ(node.getFirstRelationshipID(), 0);
    EXPECT_EQ(node.getFirstPropertyID(), 0);
}

// Test constructor with single character label
TEST_F(NodeTest, ConstructorWithSingleCharLabel) {
    Node node("A");
    EXPECT_EQ(node.getLabel(), "A");
}

// Test constructor with maximum length label (16 characters)
TEST_F(NodeTest, ConstructorWithMaxLengthLabel) {
    Node node("1234567890123456");
    EXPECT_EQ(node.getLabel(), "1234567890123456");
}

// Test constructor with empty label (should throw)
TEST_F(NodeTest, ConstructorWithEmptyLabelThrows) {
    EXPECT_THROW({
        Node node("");
    }, std::length_error);
}

// Test constructor with too long label (should throw)
TEST_F(NodeTest, ConstructorWithTooLongLabelThrows) {
    EXPECT_THROW({
        Node node("12345678901234567"); // 17 characters
    }, std::length_error);
}

// Test getLabel
TEST_F(NodeTest, GetLabel) {
    Node node("MyNode");
    std::string label = node.getLabel();
    EXPECT_STREQ(label.c_str(), "MyNode");
}

// Test getFirstRelationshipID returns initial value
TEST_F(NodeTest, GetFirstRelationshipIDInitial) {
    Node node("Node1");
    EXPECT_EQ(node.getFirstRelationshipID(), 0);
}

// Test getFirstPropertyID returns initial value
TEST_F(NodeTest, GetFirstPropertyIDInitial) {
    Node node("Node1");
    EXPECT_EQ(node.getFirstPropertyID(), 0);
}

// Test setFirstRelationship
TEST_F(NodeTest, SetFirstRelationship) {
    Node node("Node1");
    node.setFirstRelationship(12345);
    EXPECT_EQ(node.getFirstRelationshipID(), 12345);
}

// Test setFirstRelationship with zero
TEST_F(NodeTest, SetFirstRelationshipZero) {
    Node node("Node1");
    node.setFirstRelationship(100);
    node.setFirstRelationship(0);
    EXPECT_EQ(node.getFirstRelationshipID(), 0);
}

// Test setFirstRelationship with maximum uint32_t value
TEST_F(NodeTest, SetFirstRelationshipMaxValue) {
    Node node("Node1");
    uint32_t maxVal = 0xFFFFFFFF;
    node.setFirstRelationship(maxVal);
    EXPECT_EQ(node.getFirstRelationshipID(), maxVal);
}

// Test setFirstProperty
TEST_F(NodeTest, SetFirstProperty) {
    Node node("Node1");
    node.setFirstProperty(67890);
    EXPECT_EQ(node.getFirstPropertyID(), 67890);
}

// Test setFirstProperty with zero
TEST_F(NodeTest, SetFirstPropertyZero) {
    Node node("Node1");
    node.setFirstProperty(200);
    node.setFirstProperty(0);
    EXPECT_EQ(node.getFirstPropertyID(), 0);
}

// Test setFirstProperty with maximum uint32_t value
TEST_F(NodeTest, SetFirstPropertyMaxValue) {
    Node node("Node1");
    uint32_t maxVal = 0xFFFFFFFF;
    node.setFirstProperty(maxVal);
    EXPECT_EQ(node.getFirstPropertyID(), maxVal);
}

// Test setLabel with valid label
TEST_F(NodeTest, SetLabelValid) {
    Node node("Initial");
    node.setLabel("Updated");
    EXPECT_STREQ(node.getLabel().c_str(), "Updated");
}

// Test setLabel with single character
TEST_F(NodeTest, SetLabelSingleChar) {
    Node node("Initial");
    node.setLabel("X");
    EXPECT_STREQ(node.getLabel().c_str(), "X");
}

// Test setLabel with maximum length
TEST_F(NodeTest, SetLabelMaxLength) {
    Node node("Initial");
    node.setLabel("ABCDEFGHIJKLMNOP"); // 16 characters
    EXPECT_STREQ(node.getLabel().c_str(), "ABCDEFGHIJKLMNOP");
}

// Test setLabel with empty string (should throw)
TEST_F(NodeTest, SetLabelEmptyThrows) {
    Node node("Initial");
    EXPECT_THROW({
        node.setLabel("");
    }, std::length_error);
}

// Test setLabel with too long string (should throw)
TEST_F(NodeTest, SetLabelTooLongThrows) {
    Node node("Initial");
    EXPECT_THROW({
        node.setLabel("12345678901234567"); // 17 characters
    }, std::length_error);
}

// Test setUseState to true
TEST_F(NodeTest, SetUseStateTrue) {
    Node node("Node1");
    node.setUseState(true);
    // Note: inUse is private, but we can check through serialization
    auto serialized = node.serializeObject();
    EXPECT_EQ(serialized[0], static_cast<std::byte>(1));
}

// Test setUseState to false
TEST_F(NodeTest, SetUseStateFalse) {
    Node node("Node1");
    node.setUseState(false);
    auto serialized = node.serializeObject();
    EXPECT_EQ(serialized[0], static_cast<std::byte>(0));
}

// Test serializeObject basic structure
TEST_F(NodeTest, SerializeObjectBasic) {
    Node node("TestNode");
    auto serialized = node.serializeObject();
    
    // Check size
    EXPECT_EQ(serialized.size(), 16 + 8 + 2);
    
    // Check inUse flag (should be 1 by default)
    EXPECT_EQ(serialized[0], static_cast<std::byte>(1));
}

// Test serializeObject with relationship and property IDs
TEST_F(NodeTest, SerializeObjectWithIDs) {
    Node node("Node1");
    node.setFirstRelationship(0x12345678);
    node.setFirstProperty(0xABCDEF01);
    
    auto serialized = node.serializeObject();
    
    // Check inUse flag
    EXPECT_EQ(serialized[0], static_cast<std::byte>(1));
    
    // Check relationship ID bytes (big-endian)
    EXPECT_EQ(serialized[1], static_cast<std::byte>(0x12));
    EXPECT_EQ(serialized[2], static_cast<std::byte>(0x34));
    EXPECT_EQ(serialized[3], static_cast<std::byte>(0x56));
    EXPECT_EQ(serialized[4], static_cast<std::byte>(0x78));
    
    // Check property ID bytes (big-endian)
    EXPECT_EQ(serialized[5], static_cast<std::byte>(0xAB));
    EXPECT_EQ(serialized[6], static_cast<std::byte>(0xCD));
    EXPECT_EQ(serialized[7], static_cast<std::byte>(0xEF));
    EXPECT_EQ(serialized[8], static_cast<std::byte>(0x01));
}

// Test serializeObject with label
TEST_F(NodeTest, SerializeObjectWithLabel) {
    Node node("TestLabel");
    auto serialized = node.serializeObject();
    
    // Label starts at byte 9
    char extractedLabel[17] = {0};
    for (int i = 0; i < 16; ++i) {
        extractedLabel[i] = static_cast<char>(serialized[9 + i]);
    }
    
    EXPECT_STREQ(extractedLabel, "TestLabel");
}

// Test serializeObject when not in use
TEST_F(NodeTest, SerializeObjectNotInUse) {
    Node node("Node1");
    node.setUseState(false);
    
    auto serialized = node.serializeObject();
    
    // When not in use, only first byte should matter (set to 0)
    EXPECT_EQ(serialized[0], static_cast<std::byte>(0));
}

// Test multiple operations in sequence
TEST_F(NodeTest, MultipleOperationsSequence) {
    Node node("Original");
    
    // Change label
    node.setLabel("Modified");
    EXPECT_EQ(node.getLabel(), "Modified");
    
    // Set IDs
    node.setFirstRelationship(111);
    node.setFirstProperty(222);
    EXPECT_EQ(node.getFirstRelationshipID(), 111);
    EXPECT_EQ(node.getFirstPropertyID(), 222);
    
    // Change label again
    node.setLabel("Final");
    EXPECT_STREQ(node.getLabel().c_str(), "Final");
    
    // Verify IDs are still correct
    EXPECT_EQ(node.getFirstRelationshipID(), 111);
    EXPECT_EQ(node.getFirstPropertyID(), 222);
}

// Test serializeObject with zero IDs
TEST_F(NodeTest, SerializeObjectWithZeroIDs) {
    Node node("ZeroTest");
    
    auto serialized = node.serializeObject();
    
    // All ID bytes should be 0
    for (int i = 1; i <= 8; ++i) {
        EXPECT_EQ(serialized[i], static_cast<std::byte>(0));
    }
}

// Test serializeObject with max IDs
TEST_F(NodeTest, SerializeObjectWithMaxIDs) {
    Node node("MaxTest");
    node.setFirstRelationship(0xFFFFFFFF);
    node.setFirstProperty(0xFFFFFFFF);
    
    auto serialized = node.serializeObject();
    
    // All ID bytes should be 0xFF
    for (int i = 1; i <= 8; ++i) {
        EXPECT_EQ(serialized[i], static_cast<std::byte>(0xFF));
    }
}

// Test constructor from byte chunk
TEST_F(NodeTest, ConstructorFromByteChunk) {
    // Create a byte chunk with known values
    std::vector<uint8_t> chunk(26, 0);
    
    // Set inUse flag (byte 0)
    chunk[0] = 1;
    
    // Set firstRelationshipId = 0x12345678 (bytes 1-4, big-endian)
    chunk[1] = 0x12;
    chunk[2] = 0x34;
    chunk[3] = 0x56;
    chunk[4] = 0x78;
    
    // Set firstpropertyId = 0xABCDEF01 (bytes 5-8, big-endian)
    chunk[5] = 0xAB;
    chunk[6] = 0xCD;
    chunk[7] = 0xEF;
    chunk[8] = 0x01;
    
    // Set label "TestNode" (bytes 9+)
    std::string testLabel = "TestNode";
    std::memcpy(&chunk[9], testLabel.c_str(), testLabel.size());
    
    // Create node from chunk
    Node node(chunk);
    
    // Verify all fields were deserialized correctly
    EXPECT_EQ(node.getLabel(), "TestNode");
    EXPECT_EQ(node.getFirstRelationshipID(), 0x12345678);
    EXPECT_EQ(node.getFirstPropertyID(), 0xABCDEF01);
}

// Test round-trip serialization/deserialization
TEST_F(NodeTest, SerializeDeserializeRoundTrip) {
    // Create original node
    Node original("RoundTrip");
    original.setFirstRelationship(0xDEADBEEF);
    original.setFirstProperty(0xCAFEBABE);
    original.setUseState(true);
    
    // Serialize
    auto serialized = original.serializeObject();
    
    // Convert std::byte array to uint8_t vector
    std::vector<uint8_t> chunk(serialized.size());
    for (size_t i = 0; i < serialized.size(); ++i) {
        chunk[i] = static_cast<uint8_t>(serialized[i]);
    }
    
    // Deserialize into new node
    Node deserialized(chunk);
    
    // Verify all fields match
    EXPECT_EQ(deserialized.getLabel(), original.getLabel());
    EXPECT_EQ(deserialized.getFirstRelationshipID(), original.getFirstRelationshipID());
    EXPECT_EQ(deserialized.getFirstPropertyID(), original.getFirstPropertyID());
}

// Test constructor from byte chunk with null-terminated label
TEST_F(NodeTest, ConstructorFromByteChunkWithNullTerminator) {
    std::vector<uint8_t> chunk(26, 0);
    
    chunk[0] = 1; // inUse
    
    // Set label "Short" followed by null terminator
    std::string testLabel = "Short";
    std::memcpy(&chunk[9], testLabel.c_str(), testLabel.size() + 1); // Include null terminator
    
    Node node(chunk);
    
    // Label should be trimmed at null terminator
    EXPECT_EQ(node.getLabel(), "Short");
}