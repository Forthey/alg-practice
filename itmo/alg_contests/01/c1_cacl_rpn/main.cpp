#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

class Value {
public:
    using Type = std::int64_t;

    explicit Value(Type value) : m_value(value) {}

    [[nodiscard]] Type value() const { return m_value; }

private:
    Type m_value;
};

class ValueGetter {
public:
    virtual ~ValueGetter() = default;

    [[nodiscard]] virtual std::optional<Value> get() = 0;
};

class StackWrapper : public ValueGetter {
public:
    explicit StackWrapper(std::stack<Value> stack) : m_stack{std::move(stack)} {}

    std::optional<Value> get() override {
        if (m_stack.empty()) {
            return std::nullopt;
        }

        const auto value = m_stack.top();
        m_stack.pop();
        return value;
    }

    void set(Value value) {
        m_stack.push(value);
    }

    [[nodiscard]] std::size_t size() const {
        return m_stack.size();
    }

private:
    std::stack<Value> m_stack;
};

class Operator {
public:
    virtual ~Operator() = default;

    [[nodiscard]] virtual std::optional<Value> calc(ValueGetter& values) = 0;
};

class SumOperator final : public Operator {
public:
    std::optional<Value> calc(ValueGetter& values) override {
        const auto first = values.get();
        const auto second = values.get();

        if (!first || !second) {
            return std::nullopt;
        }

        return Value{first->value() + second->value()};
    }
};

class DiffOperator final : public Operator {
public:
    std::optional<Value> calc(ValueGetter& values) override {
        const auto second = values.get();
        const auto first = values.get();

        if (!first || !second) {
            return std::nullopt;
        }

        return Value{first->value() - second->value()};
    }
};

class ProdOperator final : public Operator {
public:
    std::optional<Value> calc(ValueGetter& values) override {
        const auto second = values.get();
        const auto first = values.get();

        if (!first || !second) {
            return std::nullopt;
        }

        return Value{first->value() * second->value()};
    }
};

class DivOperator final : public Operator {
public:
    std::optional<Value> calc(ValueGetter& values) override {
        const auto second = values.get();
        const auto first = values.get();

        if (!first || !second || second->value() == 0) {
            return std::nullopt;
        }

        return Value{first->value() / second->value()};
    }
};

template<std::input_iterator Iter>
Value extractValue(Iter& iter, Iter end) {
    Value::Type value{};

    while (iter != end && std::isdigit(*iter)) {
        value = value * 10 + (*iter - '0');
        ++iter;
    }

    return Value{value};
}

std::unique_ptr<Operator> createOperator(char op) {
    switch (op) {
        case '+':
            return std::make_unique<SumOperator>();
        case '-':
            return std::make_unique<DiffOperator>();
        case '*':
            return std::make_unique<ProdOperator>();
        case '/':
            return std::make_unique<DivOperator>();
        default:
            return nullptr;
    }
}

using Tokens = std::vector<std::variant<Value, std::unique_ptr<Operator>>>;

[[nodiscard]] std::optional<Tokens> tokenize(const std::string& rpnStr) {
    Tokens tokens;

    for (auto iter = rpnStr.begin(); iter != rpnStr.end();) {
        if (std::isspace(*iter)) {
            ++iter;
            continue;
        }

        if (std::isdigit(*iter)) {
            tokens.emplace_back(extractValue(iter, rpnStr.end()));
            continue;
        }

        auto newOperator = createOperator(*iter);
        if (!newOperator) {
            return std::nullopt;
        }

        tokens.emplace_back(std::move(newOperator));

        ++iter;
    }

    return tokens;
}

std::optional<Value> evaluateRpn(const Tokens& tokens) {
    StackWrapper stack{std::stack<Value>{}};

    for (const auto& token : tokens) {
        if (const auto value = std::get_if<Value>(&token)) {
            stack.set(*value);
            continue;
        }

        const auto calcResult = std::get<std::unique_ptr<Operator>>(token)->calc(stack);
        if (!calcResult) {
            return std::nullopt;
        }
        stack.set(*calcResult);
    }

    return stack.size() == 1 ? stack.get() : std::nullopt;
}

std::optional<Value> evaluateRpn(const std::string& rpnStr) {
    const auto tokens = tokenize(rpnStr);
    if (!tokens) {
        return std::nullopt;
    }

    return evaluateRpn(*tokens);
}

int main() {
    std::string rpnStr;

    std::getline(std::cin, rpnStr);

    const auto result = evaluateRpn(rpnStr);
    if (!result) {
        std::cout << "Bad expression" << std::endl;
        return 1;
    }
    std::cout << result->value() << std::endl;

    return 0;
}
