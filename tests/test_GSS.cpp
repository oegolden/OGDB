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

class GloabalStringStorageTest: public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

TEST_F(GloabalStringStorageTest, ConstructorTest){
    GlobalStringStorage GSS;
    std::filesystem::path f = "../files/gss.bin";
    EXPECT_TRUE(std::filesystem::exists(f));
    if(std::filesystem::exists(f)){
        std::filesystem::remove(f);
    }
}


TEST_F(GloabalStringStorageTest, InsertStringIntoEmptyFileTest){
    GlobalStringStorage GSS;
    int header = GSS.putString("Test");
    EXPECT_EQ(GSS.getString(header),"Test");
    if(std::filesystem::exists("../files/gss.bin")){
        std::filesystem::remove("../files/gss.bin");
    }
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
    if(std::filesystem::exists("../files/gss.bin")){
        std::filesystem::remove("../files/gss.bin");
    }
}

TEST_F(GloabalStringStorageTest,DeleteTest){

}