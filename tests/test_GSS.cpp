#include <gtest/gtest.h>
#include "../lib/globalStringStorage.h"
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstring>
#include <span>
#include <cstdlib>

std::filesystem::path f = "../files/gss.bin";

class GloabalStringStorageTest: public ::testing::Test {
protected:
    void SetUp() override {
        // Clean up any existing file before each test
        if(std::filesystem::exists(f)){
            std::filesystem::remove(f);
        }
    }

    void TearDown() override {
        // Clean up after each test
        if(std::filesystem::exists(f)){
            std::filesystem::remove(f);
        }
    }
};

TEST_F(GloabalStringStorageTest, ConstructorTest){
    GlobalStringStorage GSS;
    EXPECT_TRUE(std::filesystem::exists(f));
}


TEST_F(GloabalStringStorageTest, InsertStringIntoEmptyFileTest){
    GlobalStringStorage GSS;
    int header = GSS.putString("Test");
    EXPECT_EQ(GSS.getString(header),"Test");
}

TEST_F(GloabalStringStorageTest,StringsPastFilledChunk){
    GlobalStringStorage GSS;
    for(int i = 0; i < 66; i++){
        GSS.putString("Test " + std::to_string(i));
    }
    EXPECT_EQ(GSS.getString(65),"Test 65");
    GSS.putString("Test " + std::to_string(66));
    EXPECT_EQ(GSS.getString(66),"Test 66");
    //test to make sure that we can go back and get the right string
    EXPECT_EQ(GSS.getString(0),"Test 0");
}

TEST_F(GloabalStringStorageTest,ExistingFileTest){
    {
        GlobalStringStorage GSS;
        GSS.putString("Test 1");
    }
    {
        GlobalStringStorage GSS2;
        EXPECT_EQ(GSS2.getString(0),"Test 1");
    }
}

// 1. Edge Cases for String Operations
TEST_F(GloabalStringStorageTest, EmptyStringTest){
    GlobalStringStorage GSS;
    EXPECT_THROW(GSS.putString(""), std::length_error);
}

TEST_F(GloabalStringStorageTest, LargeStringTest){
    GlobalStringStorage GSS;
    std::string largeStr(10000, 'a');
    int header = GSS.putString(largeStr);
    EXPECT_EQ(GSS.getString(header), largeStr);
}

TEST_F(GloabalStringStorageTest, SpecialCharactersTest){
    GlobalStringStorage GSS;
    std::string special = "Test\n\t\r!@#$%^&*()";
    int header = GSS.putString(special);
    EXPECT_EQ(GSS.getString(header), special);
}

// 2. Error Handling
TEST_F(GloabalStringStorageTest, RetrieveNonexistentStringTest){
    GlobalStringStorage GSS;
    EXPECT_THROW(GSS.getString(999), std::out_of_range);
}


// 3. Concurrent/Sequential Operations
TEST_F(GloabalStringStorageTest, DuplicateStringsTest){
    GlobalStringStorage GSS;
    int h1 = GSS.putString("Duplicate");
    int h2 = GSS.putString("Duplicate");
    EXPECT_EQ(GSS.getString(h1), GSS.getString(h2));
    EXPECT_EQ(GSS.getString(h1), "Duplicate");
}

TEST_F(GloabalStringStorageTest, SequentialRetrievalAfterMultipleInsertsTest){
    GlobalStringStorage GSS;
    std::vector<int> headers;
    for(int i = 0; i < 10; i++){
        headers.push_back(GSS.putString("String " + std::to_string(i)));
    }
    for(int i = 0; i < 10; i++){
        EXPECT_EQ(GSS.getString(headers[i]), "String " + std::to_string(i));
    }
}

// 4. File Persistence
TEST_F(GloabalStringStorageTest, FileIntegrityAfterMultipleOperationsTest){
    {
        GlobalStringStorage GSS;
        for(int i = 0; i < 20; i++){
            GSS.putString("Persist " + std::to_string(i));
        }
    }

    {
        GlobalStringStorage GSS2;
        for(int i = 0; i < 20; i++){
            std::cout<<i<<std::endl;
            EXPECT_EQ(GSS2.getString(i), "Persist " + std::to_string(i));
        }
    }
}

// 5. Boundary Conditions
TEST_F(GloabalStringStorageTest, StringAtChunkBoundaryTest){
    GlobalStringStorage GSS;
    // Assuming chunk size is standard, fill up to near boundary
    std::string boundaryStr(500, 'x');
    int header = GSS.putString(boundaryStr);
    EXPECT_EQ(GSS.getString(header), boundaryStr);
}

// 6. Memory/Resource Management
TEST_F(GloabalStringStorageTest, MultipleInstancesIndependenceTest){
    GlobalStringStorage GSS1;
    int h1 = GSS1.putString("Instance1");
    GlobalStringStorage GSS2;
    int h2 = GSS2.putString("Instance2");
    EXPECT_EQ(GSS1.getString(h1), "Instance1");
    EXPECT_EQ(GSS2.getString(h2), "Instance2");
}

// 7. Delete Functionality
TEST_F(GloabalStringStorageTest, DeleteStringTest){
    GlobalStringStorage GSS;
    int h1 = GSS.putString("ToDelete");
    int h2 = GSS.putString("ToKeep");
    GSS.deleteString(h1);
    EXPECT_THROW(GSS.getString(h1), std::out_of_range);
    EXPECT_EQ(GSS.getString(h2), "ToKeep");
}
