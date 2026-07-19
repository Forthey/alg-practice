#pragma once
#include <ostream>

template<std::ranges::range T>
    requires (!requires(std::ostream &os, const T &v) { os.operator<<(v); }
              && !std::convertible_to<T, std::string_view>)
std::ostream &operator<<(std::ostream &os, const T &array) {
    for (auto element: array) {
        os << element << " ";
    }
    return os;
}
