#pragma once

#include <string>
#include <tuple>

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename T>
using scan_value_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename... Ts>
struct scan_result {
    using tuple_type = std::tuple<scan_value_t<Ts>...>;

    tuple_type values;

    scan_result() = default;

    explicit scan_result(tuple_type tuple) : values(std::move(tuple)) {}

    scan_result(Ts... args) : values(std::move(args)...) {}
};

} // namespace stdx::details
