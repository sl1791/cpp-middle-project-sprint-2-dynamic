#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, ParseSingleString) 
{
    auto result = stdx::scan<std::string>("number", "{}");

    ASSERT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values), "number");
}

TEST(ScanTest, PasreSinglInt)
{
    auto result = stdx::scan<int>("23", "{}");

    ASSERT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values), 23);
}

TEST(ScanTest, ParseIntWithFormatD)
{
    auto result = stdx::scan<int>("23", "{:d}");

    ASSERT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values), 23);
}

TEST(ScanTest, ParseUnsignedWithFormatU)
{
    auto result = stdx::scan<unsigned>("23", "{:u}");

    ASSERT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values), 23u);
}

TEST(ScanTest, ParseDoubleWithFormatF)
{
    auto result = stdx::scan<double>("3.14", "{:f}");

    ASSERT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values), 3.14);
}

TEST(ScanTest, ParseStringWithFormatS)
{
    auto result = stdx::scan<std::string>("hello", "{:s}");

    ASSERT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values), "hello");
}

TEST(ScanTest, ParseTwoValues)
{
    auto result = stdx::scan<int, std::string>("23 hello", "{} {}");

    ASSERT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values), 23);
    EXPECT_EQ(std::get<1>(result->values), "hello");
}

TEST(ScanTest, ParseWithTextAroundValue)
{
    auto result = stdx::scan<int>("value = 23", "value = {}");

    ASSERT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values), 23);
}

TEST(ScanTest, ParseTwoValuesWithText)
{
    auto result = stdx::scan<int, std::string>("id: 23, name: alex",
                                               "id: {}, name: {}");

    ASSERT_TRUE(result);
    EXPECT_EQ(std::get<0>(result->values), 23);
    EXPECT_EQ(std::get<1>(result->values), "alex");
}

TEST(ScanTest, WrongTypeReturnsError)
{
    auto result = stdx::scan<int>("hello", "{}");

    ASSERT_FALSE(result);
}

TEST(ScanTest, WrongFormatReturnsError)
{
    auto result = stdx::scan<int>("23", "{:s}");

    ASSERT_FALSE(result);
}

TEST(ScanTest, NotEnoughValuesReturnsError) 
{
    auto result = stdx::scan<int, int>("1", "{} {}");

    ASSERT_FALSE(result);
}