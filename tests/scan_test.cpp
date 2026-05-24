#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

template<typename T, size_t Sz>
void Print(const T& a_T)
{
    if constexpr (Sz)
    {
        constexpr size_t index(Sz-1);
        std::print("Print {} '{}'\n", index, std::get<index>(a_T));
        Print<T, index>(a_T);
    }
}

///////////////////////////////////////////////////////////////////////////////
// StringScanTest
TEST(StringTest, EmptyFormat) 
{
    const char* value = "text";
    auto result = stdx::scan<std::string>(value, "{}");
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value().vals, std::make_tuple(std::string(value)));

    auto result2 = stdx::scan<const std::string>(value, "{}");
    ASSERT_TRUE(result2);
    ASSERT_EQ(result2.value().vals, std::make_tuple(std::string(value)));

    // if (result)
    // {
    //     auto& tpl = result.value().vals;
    //     Print<decltype(tpl), 2>(tpl);
    // }
    // else
    //     std::print("{}\n", result.error().message);

}

TEST(StringTest, Format) 
{
    const char* value = "text";
    constexpr const char* frm = "{%s}";
    auto result = stdx::scan<std::string>(value, frm);
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value().vals, std::make_tuple(std::string(value)));

    auto result2 = stdx::scan<const std::string>(value, frm);
    ASSERT_TRUE(result2);
    ASSERT_EQ(result2.value().vals, std::make_tuple(std::string(value)));
}

TEST(StringTest, ComplexFormat) 
{
    const char* value = "text";
    constexpr const char* frm = "something is {%s}";

    std::string text = std::format("something is {}", value);
    auto result = stdx::scan<std::string>(text.data(), frm);
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value().vals, std::make_tuple(std::string(value)));

    auto result2 = stdx::scan<const std::string>(text.data(), frm);
    ASSERT_TRUE(result2);
    ASSERT_EQ(result2.value().vals, std::make_tuple(std::string(value)));
}

///////////////////////////////////////////////////////////////////////////////
// StringViewScanTest
TEST(StringViewScanTest, EmptyFormat) 
{
    const char* value = "text";
    auto result = stdx::scan<std::string_view>(value, "{}");
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value().vals, std::make_tuple(std::string_view(value)));

    auto result2 = stdx::scan<const std::string_view>(value, "{}");
    ASSERT_TRUE(result2);
    ASSERT_EQ(result2.value().vals, std::make_tuple(std::string_view(value)));
}

TEST(StringViewScanTest, Format) 
{
    const char* value = "text";
    constexpr const char* frm = "{%s}";

    auto result = stdx::scan<std::string_view>(value, frm);
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value().vals, std::make_tuple(std::string_view(value)));

    auto result2 = stdx::scan<const std::string_view>(value, frm);
    ASSERT_TRUE(result2);
    ASSERT_EQ(result2.value().vals, std::make_tuple(std::string_view(value)));
}

TEST(StringViewScanTest, ComplexFormat) 
{
    const char* value = "text";
    constexpr const char* frm = "something is {%s}";

    std::string text = std::format("something is {}", value);
    auto result = stdx::scan<std::string_view>(text.data(), frm);
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value().vals, std::make_tuple(std::string_view(value)));

    auto result2 = stdx::scan<const std::string_view>(text.data(), frm);
    ASSERT_TRUE(result2);
    ASSERT_EQ(result2.value().vals, std::make_tuple(std::string_view(value)));
}

///////////////////////////////////////////////////////////////////////////////
// int8_tScanTest
TEST(int8_tScanTest, EmptyFormat) 
{
    const int8_t val = 10;
    std::string text = std::format("{}", val);
    auto result = stdx::scan<int8_t>(text.data(), "{}");
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value().vals, std::make_tuple(int8_t(10)));

    auto result2 = stdx::scan<const int8_t>(text.data(), "{}");
    ASSERT_TRUE(result2);
    ASSERT_EQ(result2.value().vals, std::make_tuple(int8_t(10)));
}

TEST(int8_tScanTest, Format) 
{
    const int8_t val = 10;
    std::string text = std::format("{}", val);
    constexpr const char* frm = "{%d}";

    auto result = stdx::scan<int8_t>(text.data(), frm);
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value().vals, std::make_tuple(int8_t(10)));

    auto result2 = stdx::scan<const int8_t>(text.data(), frm);
    ASSERT_TRUE(result2);
    ASSERT_EQ(result2.value().vals, std::make_tuple(int8_t(10)));
}

TEST(int8_tScanTest, ComplexFormat) 
{
    const int8_t val = 10;
    std::string text = std::format("something is {}", val);
    constexpr const char* frm = "something is {%d}";

    auto result = stdx::scan<int8_t>(text.data(), frm);
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value().vals, std::make_tuple(int8_t(10)));

    auto result2 = stdx::scan<const int8_t>(text.data(), frm);
    ASSERT_TRUE(result2);
    ASSERT_EQ(result2.value().vals, std::make_tuple(int8_t(10)));
}

TEST(int8_tScanTest, WrongFormat) 
{
    const int8_t val = 10;
    std::string text = std::format("{}", val);
    auto result = stdx::scan<int8_t>(text.data(), "{%u}");
    ASSERT_FALSE(result);

    auto result2 = stdx::scan<const int8_t>(text.data(), "{%u}");
    ASSERT_FALSE(result2);
}

// ///////////////////////////////////////////////////////////////////////////////
// // doubleScanTest
TEST(doubleScanTest, EmptyFormat) 
{
    using testType = double;
    const testType val = 100.72;
    std::string text = std::format("{}", val);

    auto result = stdx::scan<testType>(text.data(), "{}");
    ASSERT_TRUE(result);
    const bool isSame = abs(val - std::get<0>(result.value().vals)) < 0.000001;
    ASSERT_TRUE(isSame);

    auto result2 = stdx::scan<const testType>(text.data(), "{}");
    ASSERT_TRUE(result2);
    const bool isSame2 = abs(val - std::get<0>(result2.value().vals)) < 0.000001;
    ASSERT_TRUE(isSame2);
}

TEST(doubleScanTest, ComplexFormat) 
{
    using testType = double;
    const testType val = -101.77;
    std::string text = std::format("something is {}", val);
    constexpr const char* frm = "something is {%f}";

    auto result = stdx::scan<testType>(text.data(), frm);
    ASSERT_TRUE(result);
    const bool isSame = abs(val - std::get<0>(result.value().vals)) < 0.000001;
    ASSERT_TRUE(isSame);

    auto result2 = stdx::scan<const testType>(text.data(), frm);
    ASSERT_TRUE(result2);
    const bool isSame2 = abs(val - std::get<0>(result2.value().vals)) < 0.000001;
    ASSERT_TRUE(isSame2);

}

TEST(doubleScanTest, WrongFormat) 
{
    using testType = double;
    const testType val = 101.77;
    std::string text = std::format("{}", val);

    auto result = stdx::scan<testType>(text.data(), "{%s}");
    ASSERT_FALSE(result);

    auto result2 = stdx::scan<testType>(text.data(), "{%s}");
    ASSERT_FALSE(result2);
}


///////////////////////////////////////////////////////////////////////////////
// ScanTest
TEST(ScanTest, Format) 
{
    using intType = int;
    using uintType = uint32_t;
    using strType = std::string;
    using strViewType = std::string_view;

    constexpr intType v1 = 1;
    constexpr intType v2 = 2;
    constexpr uintType v3 = 30;
    constexpr uintType v4 = 40;
    constexpr const char* v9 = "9String";
    constexpr const char* v10 = "10String";
    constexpr strViewType v11 = "11StringView";
    constexpr strViewType v12 = "12StringView";

    constexpr const char* frmt = "INT {} {} {} {} TEXT {} {} {} {} ";
    std::string text = std::format(frmt,
                            v1, v2, v3, v4,
                            v9, v10, v11, v12);

    auto result = stdx::scan<intType, intType, uintType, uintType,
                        strType, strType, strViewType, strViewType>
                        (text.data(), frmt);
    ASSERT_TRUE(result);
    auto tpl = std::make_tuple(v1, v2, v3, v4, v9, v10, v11, v12);
    ASSERT_EQ(result.value().vals, tpl);

    auto result2 = stdx::scan<const intType, const intType, 
                        const uintType, const uintType,
                        const strType, const strType, 
                        const strViewType, const strViewType>
                        (text.data(), frmt);
    ASSERT_TRUE(result2);
    auto tpl2 = std::make_tuple(v1, v2, v3, v4, v9, v10, v11, v12);
    ASSERT_EQ(result.value().vals, tpl2);

}
