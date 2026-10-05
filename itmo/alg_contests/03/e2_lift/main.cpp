
#include <cstdint>
#include <iostream>
#include <ranges>
#include <vector>

using PersonsCount = std::int64_t;
using Duration = std::int64_t;

namespace {
class PersonsCalculator {
public:
    struct Params {
        PersonsCount liftCapacity{};
        Duration floorDelta{};
        Duration targetTime{};
        std::vector<PersonsCount> personsCount;
        PersonsCount allPersonsCount{};
    };

    explicit PersonsCalculator(Params p)
        : m_personsCount{std::move(p.personsCount)}
        , m_liftCapacity{p.liftCapacity}
        , m_floorDelta{p.floorDelta}
        , m_targetTime{p.targetTime}
        , m_allPersonsCount{p.allPersonsCount} {}

    PersonsCount calcMaxPersonCount() {
        PersonsCount personsLeft = 0;
        PersonsCount personsRight = m_allPersonsCount + 1;

        while (personsRight - personsLeft > 1) {
            const auto personsMiddle = personsLeft + (personsRight - personsLeft) / 2;

            if (canLiftManyPassengers(personsMiddle)) {
                personsLeft = personsMiddle;
                continue;
            }

            personsRight = personsMiddle;
        }

        return personsLeft;
    }

private:
    bool canLiftManyPassengers(PersonsCount targetCount) const {
        std::size_t lastFloorIndex = 0;
        PersonsCount personsOnLastFloor = 0;

        while (targetCount > 0 && lastFloorIndex < m_personsCount.size()) {
            if (targetCount <= m_personsCount[lastFloorIndex]) {
                personsOnLastFloor = targetCount;
                targetCount = 0;
                continue;
            }

            targetCount -= m_personsCount[lastFloorIndex];
            ++lastFloorIndex;
        }

        /// Мы уже считаем, что нам нужно подняться на последний этаж, чтобы заполнить людьми лифт
        Duration consumedTime = 2 * lastFloorIndex * m_floorDelta;
        /// Оставшееся место в лифте для заполнения его людьми
        PersonsCount currentRideCapacity = m_liftCapacity;

        // Пока мы не довезли всех людей с 1+ этажа, либо пока мы не пересекли дедлайн
        while (lastFloorIndex >= 1 && consumedTime < m_targetTime) {
            // Текущий этаж не заполнит лифт до конца
            // Равенство удобней сюда перенести, так как это гарантия, что мы лишний раз не приедем на этаж,
            // Который буквально только что опустошили ровно для заполнения лифта
            // И что мы будем сразу пропускать этажи с 0 человек при любой заполненности
            if (currentRideCapacity >= personsOnLastFloor) {
                currentRideCapacity -= personsOnLastFloor;
                --lastFloorIndex;
                personsOnLastFloor = m_personsCount[lastFloorIndex];
                continue;
            }

            // Текущий этаж заполняет лифт до конца и кто-то еще останется. В модели мы как бы спустились
            // Уменьшаем количество людей на этом этаже до того, которым мы можем забить лифт
            personsOnLastFloor -= currentRideCapacity;
            // Обновляем заполненность лифта
            currentRideCapacity = m_liftCapacity;
            // Сразу считаем время, которое нам потребуется на поездку до этого этажа
            consumedTime += 2 * lastFloorIndex * m_floorDelta;
        }

        // Только при таком значении мы отвезем всех, не выйдя раньше по дедлайну
        return lastFloorIndex == 0;
    }

    std::vector<PersonsCount> m_personsCount;
    PersonsCount m_liftCapacity;
    Duration m_floorDelta;
    Duration m_targetTime;
    PersonsCount m_allPersonsCount;
};

class IOWrapper {
public:
    explicit IOWrapper(std::istream& in, std::ostream& out) : m_in{in}, m_out{out} {}

    [[nodiscard]] PersonsCalculator::Params input() const {
        std::size_t n;

        m_in >> n;

        PersonsCalculator::Params input;

        m_in >> input.liftCapacity >> input.floorDelta >> input.targetTime;
        input.personsCount.resize(n + 1);

        for (auto i : std::views::iota(1ull, n + 1)) {
            m_in >> input.personsCount[i];
            input.allPersonsCount += input.personsCount[i];
        }

        return input;
    }

    void setAnswer(PersonsCount maxPersonsCount) const { m_out << maxPersonsCount << std::endl; }

private:
    std::istream& m_in;
    std::ostream& m_out;
};
}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    IOWrapper io{std::cin, std::cout};

    PersonsCalculator calculator{io.input()};

    io.setAnswer(calculator.calcMaxPersonCount());

    return 0;
}
