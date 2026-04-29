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
std::expected<T, scan_error> parse_value(std::string_view input)
{
    if constexpr (std::is_same_v<T, std::string>)
    {
        return std::string{input};
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        try
        {
            std::string str{input};
            std::size_t pos = 0;

            double parsed = std::stod(str, &pos);

            if(pos != str.size())
            {
                return std::unexpected(scan_error{"failed to parse floating point value"});
            }

            return static_cast<T>(parsed);
        }
        catch (...)
        {
            return std::unexpected(scan_error{"failed to parse floating point value"});
        }
    }
    else
    {
        T value;

        const char* begin = input.data();
        const char* end = input.data() + input.size();

        auto [ptr, ec] = std::from_chars(begin, end, value);

        if(ec != std::errc{} || ptr != end)
        {
            return std::unexpected(scan_error{"failed to parse value"});
        }

        return value;
    }
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, 
                                                     std::string_view fmt) 
{
    if(fmt.empty())
    {
        return parse_value<T>(input);
    }

    if(fmt == ":s")
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            return parse_value<T>(input);
        }
        else
        {
            return std::unexpected(scan_error{"format {:s} requires std::string"});
        }
    }

    if(fmt == ":d")
    {
        if constexpr (std::is_signed_v<T> && std::is_integral_v<T>)
        {
            return parse_value<T>(input);
        }
        else
        {
            return std::unexpected(scan_error{"format {:d} requires signed integer"});
        }
    }

    if(fmt == ":u")
    {
        if constexpr (std::is_unsigned_v<T> && std::is_integral_v<T>)
        {
            return parse_value<T>(input);
        }
        else
        {
            return std::unexpected(scan_error{"format {:u} requires unsigned integer"});
        }
    }

    if(fmt == ":f")
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return parse_value<T>(input);
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