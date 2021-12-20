#include "Utils.h"
#include <map>
#include <set>

namespace Day17A {

struct Reader {
    const std::string &s;
    int pos = 0;

    static bool is_relevant(char c) {
        return ('0' <= c && c <= '9') || c == '-';
    }

    int read_int() {
        bool is_negative = false;
        while (!is_relevant(s[pos])) {
            ++pos;
        }
        if (s[pos] == '-') {
            is_negative = true;
            ++pos;
        }
        int value = 0;
        while (is_relevant(s[pos])) {
            value = value * 10 + (s[pos] - '0');
            ++pos;
        }
        if (is_negative) {
            value = -value;
        }
        return value;
    }
};

std::tuple<int, int, int, int> read_input(std::istringstream &is) {
    const std::string &buf = is.str();
    Reader reader{buf};
    int llx = reader.read_int();
    int urx = reader.read_int();
    int lly = reader.read_int();
    int ury = reader.read_int();
    return {llx, urx, lly, ury};
}

auto solve1 = [](std::istringstream &is, std::ostringstream &os) {
    const auto &[llx, urx, lly, ury] = read_input(is);
    int initial_y = -lly - 1;
    int max_y = initial_y * (initial_y + 1) / 2;
    os << max_y;
};

bool hits_target(int llx, int urx, int lly, int ury, int ivx, int ivy) {
    int x = 0, y = 0, vx = ivx, vy = ivy;
    if (ivx * (ivx + 1) / 2 < llx) {
        return false;
    }
    while (true) {
        if (x > urx) {
            return false;
        }
        if (y < lly) {
            return false;
        }
        if (llx <= x && x <= urx && lly <= y && y <= ury) {
            return true;
        }
        x += vx;
        y += vy;
        if (vx > 0) {
            vx -= 1;
        }
        vy -= 1;
    }
}

void for_each_hit_x(int llx, int urx, int ivx, auto fn) {
    int x = 0, vx = ivx;
    if (ivx * (ivx + 1) / 2 < llx) {
        return;
    }
    for (int step = 0; step < 300; ++step) {
        if (x > urx) {
            return;
        }
        if (llx <= x && x <= urx) {
            fn(step);
        }
        x += vx;
        if (vx > 0) {
            vx -= 1;
        }
    }
}

void for_each_hit_y(int lly, int ury, int ivy, auto fn) {
    int y = 0, vy = ivy;
    for (int step = 0;; ++step) {
        if (y < lly) {
            return;
        }
        if (lly <= y && y <= ury) {
            fn(step);
        }
        y += vy;
        vy -= 1;
    }
}

auto solve2 = [](std::istringstream &is, std::ostringstream &os) {
    const auto &[llx, urx, lly, ury] = read_input(is);
    int min_x = 1;
    int max_x = urx;
    int min_y = lly;
    int initial_y = -lly - 1;
    int max_y = initial_y * (initial_y + 1) / 2;

    std::map<int, std::vector<int>> feasible_x, feasible_y;

    for (int ivx = min_x; ivx <= max_x; ++ivx) {
        for_each_hit_x(llx, urx, ivx, [&](int step) {
            feasible_x[step].push_back(ivx);
        });
    }
    for (int ivy = min_y; ivy <= max_y; ++ivy) {
        for_each_hit_y(lly, ury, ivy, [&](int step) {
            feasible_y[step].push_back(ivy);
        });
    }

    std::set<std::tuple<int, int>> feasible_pairs;
    for (const auto &[step, ivxs] : feasible_x) {
        const auto &ivys = feasible_y[step];
        for (int ivx : ivxs) {
            for (int ivy : ivys) {
                feasible_pairs.insert({ivx, ivy});
            }
        }
    }

    os << std::size(feasible_pairs);

    //int feasible_count = 0;
    //for (int ivx : feasible_x) {
    //    for (int ivy : feasible_y) {
    //        if (hits_target(llx, urx, lly, ury, ivx, ivy)) {
    //            ++feasible_count;
    //        }
    //    }
    //}

    //int feasible_count = 0;
    //for (int ivx = min_x; ivx <= max_x; ++ivx) {
    //    for (int ivy = min_y; ivy <= max_y; ++ivy) {
    //        if (hits_target(llx, urx, lly, ury, ivx, ivy)) {
    //            ++feasible_count;
    //        }
    //    }
    //}

    //os << feasible_count;
};

void main() {
    {
        Utils::Tester tester{"day17"};

        tester.test("part1", "in1", "45", solve1);
        tester.test("part1", "in2", "7750", solve1);

        tester.test("part2", "in1", "112", solve2);
        tester.test("part2", "in2", "4120", solve2);
    }

    Utils::bench("day17", "part2", "in2", "4120", 100, solve2);
}
} // namespace Day17A