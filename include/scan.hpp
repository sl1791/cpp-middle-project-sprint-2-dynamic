#pragma once

#include "parse.hpp"
#include "types.hpp"

namespace stdx {

using cvec = const std::vector<std::string_view>;

template <typename Tpl>
std::unique_ptr<details::scan_error>
ScanVals(Tpl& scanRes, cvec& input, cvec& format)
{
    return nullptr;
}

template<typename Tpl, typename First, typename... Rest>
std::unique_ptr<details::scan_error> ScanVals(Tpl& scanRes, cvec& input, cvec& format) 
{
    constexpr size_t index = std::tuple_size_v<Tpl> - sizeof...(Rest) - 1;

    std::print("ScanVals {} '{}' '{}'\n", index, input[index], format[index]);

    auto parseRes = details::parse_value_with_format<First>(input[index], format[index]);
    if (!parseRes)
        return std::make_unique<details::scan_error>(parseRes.error());

    std::get<index>(scanRes) = *parseRes;
    return ScanVals<Tpl, Rest...>(scanRes, input, format);
}

// замените болванку функции scan на рабочую версию
template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error> 
scan(std::string_view input, std::string_view format) 
{
    using parseRes = std::pair<std::vector<std::string_view>, std::vector<std::string_view>>;
    std::expected<parseRes, details::scan_error> psrRes = details::parse_sources(input, format);
    if(!psrRes)
        return std::unexpected(psrRes.error());
    
    cvec& form = psrRes.value().first;
    cvec& inp = psrRes.value().second; 

    details::scan_result<Ts...> scanRes;
    std::unique_ptr<details::scan_error> err = 
        ScanVals<std::tuple<Ts...>, Ts...>(scanRes.vals, inp, form);
    if(err)
        return std::unexpected(*err);
    return scanRes;
}

} // namespace stdx
