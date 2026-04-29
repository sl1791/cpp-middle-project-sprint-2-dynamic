#pragma once

#include <string>
#include <tuple>

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
    std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts>
struct scan_result {
    std::tuple<Ts...> values;

    scan_result() = default;

    explicit scan_result(std::tuple<Ts...> tuple) : values(std::move(tuple)) {}

    scan_result(Ts... args) : values(std::move(args)...) {}
};

} // namespace stdx::details
