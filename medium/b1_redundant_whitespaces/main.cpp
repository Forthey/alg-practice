#include <iostream>
#include <iterator>

template<std::input_iterator Iter>
Iter skipWhitespaces(const Iter& begin, const Iter& end) {
    if (begin == end || !std::isspace(*begin)) {
        return begin;
    }

    auto iter = begin;
    while (iter != end) {
        if (!std::isspace(*iter)) {
            return iter;
        }
        ++iter;
    }

    return end;
}

std::string trimWhitespaces(const std::string& str) {
    const auto iterBegin = skipWhitespaces(str.begin(), str.end());
    if (iterBegin == str.end()) {
        return {};
    }

    const auto iterEnd = skipWhitespaces(str.rbegin(), str.rend());

    bool wasLastSpace = false;
    std::string trimmedStr;

    for (auto iter = iterBegin; iter != iterEnd.base(); ++iter) {
        if (!std::isspace(*iter)) {
            trimmedStr += *iter;
            wasLastSpace = false;
            continue;
        }

        if (wasLastSpace) {
            continue;
        }

        trimmedStr += *iter;
        wasLastSpace = true;
    }

    return trimmedStr;
}

int main() {
    std::string str;

    std::getline(std::cin, str);

    std::cout << trimWhitespaces(str) << std::endl;

    return 0;
}
