#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <regex>

#include "types.hpp"

namespace stdx::details {

// здесь ваш код

template<typename T>
static constexpr bool IsNumberFormat(const std::string_view& s) 
requires std::is_same_v<T, int8_t> ||
        std::is_same_v<T, int16_t> ||
        std::is_same_v<T, int32_t> ||
        std::is_same_v<T, int64_t> 
{
    static const std::regex pattern(R"(^(\s*|%d)$)");
    return std::regex_match(std::string(s), pattern);
}

template<typename T>
static constexpr bool IsNumberFormat(const std::string_view& s) 
requires std::is_same_v<T, uint8_t> ||
        std::is_same_v<T, uint16_t> ||
        std::is_same_v<T, uint32_t> ||
        std::is_same_v<T, uint64_t> 
{
    static const std::regex pattern(R"(^(\s*|%u)$)");
    return std::regex_match(std::string(s), pattern);
}

template<typename T>
static constexpr bool IsNumberFormat(const std::string_view& s) 
requires std::is_same_v<T, float> ||
        std::is_same_v<T, double>
{
    static const std::regex pattern(R"(^(\s*|%f)$)");
    return std::regex_match(std::string(s), pattern);
}

static constexpr bool IsStringFormat(const std::string_view& s) 
{
    static const std::regex pattern(R"(^(\s*|%s)$)");
    return std::regex_match(std::string(s), pattern);
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) 
requires std::is_same_v<T, int8_t> ||
        std::is_same_v<T, int16_t> ||
        std::is_same_v<T, int32_t> ||
        std::is_same_v<T, int64_t> ||
        std::is_same_v<T, uint8_t> ||
        std::is_same_v<T, uint16_t> ||
        std::is_same_v<T, uint32_t> ||
        std::is_same_v<T, uint64_t> ||
        std::is_same_v<T, float> ||
        std::is_same_v<T, double>
{

    if (!IsNumberFormat<T>(fmt)) 
        return std::unexpected(scan_error{std::format("'{}' format does not match a numeric data type.", fmt)});
            
    T val = {};
    auto result = std::from_chars(input.data(), input.data() + input.size(), val);
    if (result.ec == std::errc()) 
        return val;

    std::string str = std::format("The value '{}' does not match the specified data type.", input);
    return std::unexpected(scan_error{str});
}

template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) 
requires std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>
{
    if (!IsStringFormat(fmt)) 
        return std::unexpected(scan_error{std::format("'{}' format does not match a string data type.", fmt)});

    T val(input);
    return val;
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