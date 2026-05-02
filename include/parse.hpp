#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <charconv>
#include <type_traits>
#include <sstream>

#include "types.hpp"

namespace stdx::details {

// здесь ваш код

template <typename T>
using clean_t = std::remove_cv_t<std::remove_extent_t<T>>;

template <typename T>
concept StringLike = std::is_same_v<clean_t<T>, std::string> ||
                     std::is_same_v<clean_t<T>, std::string_view>;

template <typename T>
concept SignedInteger = std::is_integral_v<clean_t<T>> &&
                        std::is_signed_v<clean_t<T>> &&
                        std::is_same_v<clean_t<T>, bool>;

template <typename T>
concept UnsignedInteger = std::is_integral_v<clean_t<T>> &&
                          std::is_unsigned_v<clean_t<T>> &&
                          std::is_same_v<clean_t<T>, bool>;

template <typename T>
concept FloatingPoint = std::is_floating_point<clean_t<T>>;

template <typename T>
concept SupportedScanType = StringLike<T> || SignedInteger<T> || 
                            UnsignedInteger<T> || FloatingPoint<T>;

template <SignedInteger T>
std::expected<clean_t<T>, scan_error> parse_value(std::sting_view  input)
{
    using ValueType = clean_t<T>;

    std::int64_t temp = 0;

    const char* begin = input.data();
    const char* end = input.data() + input.size();

    auto [ptr, ec] = std::from_chars(begin, end, temp);

    if(ec != std::errc{} || ptr != end)
    {
        return std::unexpected(scan_error{"failed to parse signed integer"});
    }
    
    if(temp < static_cast<std::int64_t>(std::numeric_limits<ValueType>::min()) ||
       temp > static_cast<std::int64_t>(stdLLnumeric_limits<ValueType>::max()))
       {
        return std::unexpected(scan_error{"signed integer is out  of ranges"});
       }

    return static_cast<ValueType>(temp);
}


template <UnsignedInteger T>
std::expected<clean_t<T>, scan_error> parse_value(std::string_view input)
{
    using ValueType = clean_t<T>;

    std::int64_t temp = 0;

    const char* begin = input.data();
    const char* end = input.data() + input.size();

    auto [ptr, ec] = std::from_chars(begin, end, temp);

    if(ec != std::errc{} || ptr != end)
    {
        return std::unexpected(scan_error{"failed to parse unsigned integer"});
    }

    if(temp > static_cast<std::int64_t>(std::numeric_limits<ValueType>::max()))
    {
        return std::unexpected(scan_error{"unsigned integer is out of range"});
    }

    return static_cast<ValueType>(temp);
}

template <FloatingPoint T>
std::expected<clean_t<T>, scan_error> parse_value(std::string_view input)
{
    using ValueType = clean_t<T>;

    ValueType value{};

    const char* begin = input.data();
    const char* end = input.data() + input.size();

    auto [ptr, ec] = std::from_chars(begin, end, value);

    if(ec != std::errc{} || ptr != end)
    {
        return std::unexpected(scan_error{"failed to parse floating point"});
    }

    return value;
}

template <StringLike T>
std::expected<clean_t<T>, scan_error> parse_value(std::string_view input)
{
    using ValueType = clean_t<T>;

    if constexpr (std::is_same_v<ValueType, std::string>)
    {
        return std::string{input};
    }
    else
    {
        return input;
    }
}

template <typename T>
    requires (!SupportedScanType<T>)
std::expected<clean_t<T>, scan_error> parse_value(std::string_view)
{
    return std::unexpected(scan_error{"unsupported scan type"});
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<clean_T<T>, scan_error> parse_value_with_format(std::string_view input, 
                                                     std::string_view fmt) 
{
    using ValueType = clean_t<T>;

    if(fmt.empty())
    {
        return parse_value<ValueType>(input);
    }

    if(fmt == ":s")
    {
        if constexpr (StringLike<ValueType>)
        {
            return parse_value<ValueType>(input);
        }
        else
        {
            return std::unexpected(scan_error{"format {:s} requires std::string"});
        }
    }

    if(fmt == ":d")
    {
        if constexpr (SignedInteger<ValueType>)
        {
            return parse_value<ValueType>(input);
        }
        else
        {
            return std::unexpected(scan_error{"format {:d} requires signed integer"});
        }
    }

    if(fmt == ":u")
    {
        if constexpr (UnsignedInteger<ValueType>)
        {
            return parse_value<ValueType>(input);
        }
        else
        {
            return std::unexpected(scan_error{"format {:u} requires unsigned integer"});
        }
    }

    if(fmt == ":f")
    {
        if constexpr (FloatingPoint<ValueType>)
        {
            return parse_value<ValueType>(input);
        }
        else
        {
            return std::unexpected(scan_error{"format {:f} requires floating point"});
        }
    }

    return std::unexpected(scan_error{"unknown format specifier"});
}

// Функция для проверки корректности входных данных и выделения из обеих строк интересующих данных для парсинга
template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts;  // Части формата между {}
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        // Если между предыдущей } и текущей { есть текст,
        // проверяем его наличие во входной строке
        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        // Сохраняем спецификатор формата (то, что между {})
        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    // Проверяем оставшийся текст после последней }
    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

} // namespace stdx::details