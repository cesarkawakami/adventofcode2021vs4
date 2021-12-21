#include "Utils.h"
#include <string_view>

namespace Day18A {

struct Number;

struct Number {
    std::vector<std::tuple<int, int>> data;

    static Number from_sv(const std::string_view &sv) {
        std::vector<std::tuple<int, int>> data;
        int level = 0;
        for (int i = 0; i < std::ssize(sv); ++i) {
            char c = sv[i];
            if (c == '[') {
                ++level;
            } else if (c == ']') {
                --level;
            } else if ('0' <= c && c <= '9') {
                data.push_back({level, c - '0'});
            }
        }
        return Number{data};
    }

    std::optional<Number> reduce_nested() const {
        for (int i = 0; i < std::ssize(data) - 1; ++i) {
            const auto &[left_level, left_value] = data[i];
            const auto &[right_level, right_value] = data[i + 1];
            if (left_level == 5 && right_level == 5) {
                std::vector<std::tuple<int, int>> new_data;
                std::copy(data.begin(), data.begin() + i, std::back_inserter(new_data));
                new_data.push_back({left_level - 1, 0});
                std::copy(data.begin() + i + 2, data.end(), std::back_inserter(new_data));
                if (i != 0) {
                    auto &[lefter_level, lefter_value] = new_data[i - 1];
                    lefter_value += left_value;
                }
                if (i + 2 < std::ssize(data)) {
                    auto &[righter_level, righter_value] = new_data[i + 1];
                    righter_value += right_value;
                }
                return Number{new_data};
            }
        }
        return std::nullopt;
    }

    std::optional<Number> reduce_split() const {
        for (int i = 0; i < std::ssize(data); ++i) {
            const auto &[level, value] = data[i];
            if (value >= 10) {
                std::vector<std::tuple<int, int>> new_data;
                std::copy(data.begin(), data.begin() + i, std::back_inserter(new_data));
                new_data.push_back({level + 1, value / 2});
                new_data.push_back({level + 1, value - value / 2});
                std::copy(data.begin() + i + 1, data.end(), std::back_inserter(new_data));
                return Number{new_data};
            }
        }
        return std::nullopt;
    }

    Number reduce() const;

    Number add(Number other) const {
        std::vector<std::tuple<int, int>> new_data;
        for (const auto &[level, value] : data) {
            new_data.push_back({level + 1, value});
        }
        for (const auto &[level, value] : other.data) {
            new_data.push_back({level + 1, value});
        }
        return Number{new_data}.reduce();
    }

    std::tuple<int, int64_t> magnitude(int start = 0, int level = 0) const {
        const auto &[data_level, data_value] = data[start];
        if (data_level == level) {
            return {start + 1, data_value};
        } else if (level > data_level) {
            abort();
        }
        const auto &[left_end, left_value] = magnitude(start, level + 1);
        const auto &[right_end, right_value] = magnitude(left_end, level + 1);
        return {right_end, 3 * left_value + 2 * right_value};
    }
};

std::ostream &operator<<(std::ostream &os, const Number &n) {
    std::string sep{""};
    for (const auto &[level, value] : n.data) {
        os << sep << "(" << level << "," << value << ")";
        sep = ", ";
    }
    return os;
}

Number Number::reduce() const {
    Number rv{*this};
    //std::cerr << "original: " << *this << "\n";
    for (int iteration = 1; iteration <= 1000; ++iteration) {
        if (auto next = rv.reduce_nested(); next) {
            rv = *next;
            //std::cerr << "r nested: " << rv << "\n";
            continue;
        }
        if (auto next = rv.reduce_split(); next) {
            rv = *next;
            //std::cerr << "r split:  " << rv << "\n";
            continue;
        }
        break;
    }
    return rv;
}

void read_input(std::istringstream &is, auto fn) {
    std::string line;
    while (is >> line) {
        fn(std::string_view{line});
    }
}

auto solve1 = [](std::istringstream &is, std::ostringstream &os) {
    bool first_read = true;
    Number n;
    read_input(is, [&](auto line) {
        auto x = Number::from_sv(line);
        if (first_read) {
            first_read = false;
            n = x;
        } else {
            n = n.add(x);
        }
    });
    const auto &[_end, magnitude] = n.magnitude();
    os << magnitude;
};

auto solve2 = [](std::istringstream &is, std::ostringstream &os) {
    std::vector<Number> ns;
    read_input(is, [&](auto line) {
        ns.push_back(Number::from_sv(line));
    });
    int64_t max_magnitude = std::numeric_limits<int64_t>::min();
    for (int i = 0; i < std::ssize(ns); ++i) {
        for (int j = 0; j < std::ssize(ns); ++j) {
            if (i == j) {
                continue;
            }
            const auto &[_end, magnitude] = ns[i].add(ns[j]).magnitude();
            max_magnitude = std::max(max_magnitude, magnitude);
        }
    }
    os << max_magnitude;
};

void main() {
    {
        Utils::Tester tester{"day18"};

        tester.test("part1", "in1", "4140", solve1);
        tester.test("part1", "in2", "3884", solve1);

        tester.test("part2", "in1", "3993", solve2);
        tester.test("part2", "in2", "4595", solve2);
    }
}
} // namespace Day18A