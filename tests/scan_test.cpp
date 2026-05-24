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

TEST(ScanTest, SimpleTest) 
{
    auto result = stdx::scan<int32_t, std::string>("22 nnnn", "{%d} {   }");
    if (result)
    {
        auto& tpl = result.value().vals;
        Print<decltype(tpl), 2>(tpl);
    }
    else
        std::print("{}\n", result.error().message);

    ASSERT_TRUE(result);
}