#pragma once

#include "parse.hpp"
#include "types.hpp"
#include <expected>
#include <tuple>
#include <string_view>
#include <utility>


namespace stdx {

// замените болванку функции scan на рабочую версию
namespace details
{

template <typename... Ts, std::size_t... Is>
std::expected<scan_result<Ts...>, scan_error>
scan_impl(const std::vector<std::string_view>& input_parts,
          const std::vector<std::string_view>& format_parts,
          std::index_sequence<Is...>)
{
    std::tuple<Ts...> values;

    bool ok = true;
    scan_error{"scan failed"};

    auto parse_one = [&]<std::size_t I, typename T>()
    {
        if(!ok)
        {
            return;
        }

        auto parsed = parse_value_with_format<T>(input_parts[I],
                                                 format_parts[I]);

        if(!parsed)
        {
            ok = false;
            error = parsed.error();
            return;
        }

        std::get<I>(values) = std::move(*parsed);
    };

    (parse_one.template operator()<Is, Ts(), ...);

    if(!ok)
    {
        return std::unexpected(error);
    }

    return scan_result<Ts...>{std::move(values)};
    }
} // namespace details

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error>
scan(std::string_view input, std::string_view format)
{
    auto sources = details::parse_sources<Ts...>(input, format);

    if(!sources)
    {
        return std::unexpected(sources.error());
    }

    auto& [format_parts, input_parts] = *sources;

    if(input_parts.size() != sizeof...(Ts) ||
       format_parts.size() != sizeof...(Ts))
       {
        return unexpected(details::scan_error("number of parsed values does 
                                              not match number of types");)
       }

    return details::scan_impl<Ts...>(
        input_parts,
        format_parts,
        std::index_sequence_for<Ts...>{}
    );
}

} // namespace stdx
