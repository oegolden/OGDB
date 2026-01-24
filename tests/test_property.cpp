#include <gtest/gtest.h>
#include "../lib/property.h"

class PropertyTest : public ::testing::Test {
protected:
    PropertyType testType = PropertyType::STRING;  // Ensure PropertyType enum is defined in property.h with STRING member
};

TEST_F(PropertyTest, ConstructorValidName) {
    Property prop("TestProp", 0, testType);
    EXPECT_EQ(prop.getName(), "TestProp");
    EXPECT_EQ(prop.getPrevPropId(), 0);
    EXPECT_EQ(prop.getNextPropId(), -1);
}

TEST_F(PropertyTest, ConstructorEmptyNameThrows) {
    EXPECT_THROW(Property("", 0, testType), std::invalid_argument);
}

TEST_F(PropertyTest, ConstructorNameTooLongThrows) {
    std::string longName(Property::NAMELENGTH + 1, 'a');
    EXPECT_THROW(Property(longName, 0, testType), std::invalid_argument);
}

TEST_F(PropertyTest, ConstructorMaxLengthName) {
    std::string maxName(Property::NAMELENGTH, 'x');
    Property prop(maxName, 0, testType);
    EXPECT_EQ(prop.getName(), maxName);
}

TEST_F(PropertyTest, SetNameValid) {
    Property prop("Initial", 0, testType);
    prop.setName("Updated");
    EXPECT_EQ(prop.getName(), "Updated");
}

TEST_F(PropertyTest, SetNameEmptyThrows) {
    Property prop("Test", 0, testType);
    EXPECT_THROW(prop.setName(""), std::invalid_argument);
}

TEST_F(PropertyTest, SetNameTooLongThrows) {
    Property prop("Test", 0, testType);
    std::string longName(Property::NAMELENGTH + 1, 'a');
    EXPECT_THROW(prop.setName(longName), std::invalid_argument);
}

TEST_F(PropertyTest, NextPropIdOperations) {
    Property prop("Test", 0, testType);
    prop.setNextPropId(42);
    EXPECT_EQ(prop.getNextPropId(), 42);
}

TEST_F(PropertyTest, PrevPropIdOperations) {
    Property prop("Test", 100, testType);
    EXPECT_EQ(prop.getPrevPropId(), 100);
    prop.setPrevPropId(200);
    EXPECT_EQ(prop.getPrevPropId(), 200);
}