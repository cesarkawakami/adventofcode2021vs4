#include "Utils.h"
#include <array>
#include <map>

namespace Day24A {

const int N = 14;

const std::array<std::tuple<int, int, int>, N> params{{
    {1, 13, 8},
    {1, 12, 16},
    {1, 10, 4},
    {26, -11, 1},
    {1, 14, 13},
    {1, 13, 5},
    {1, 12, 0},
    {26, -5, 10},
    {1, 10, 7},
    {26, 0, 2},
    {26, -11, 13},
    {26, -13, 15},
    {26, -13, 14},
    {26, -11, 9},
}};
const std::array<int64_t, N + 2> pow10{
    []() {
        std::array<int64_t, N + 2> rv{1};
        for (int i = 1; i < N + 2; ++i) {
            rv[i] = rv[i - 1] * 10;
        }
        return rv;
    }()};

std::tuple<bool, int64_t> eval(int64_t input) {
    bool x = false;
    int64_t z = 0;
    for (int i = 0; i < N; ++i) {
        auto [A, B, C] = params[i];
        int digit_idx = N - 1 - i;
        int in_digit = input / pow10[digit_idx] % 10;
        x = (z % 26 + B) != in_digit;
        z = z / A * (25 * int64_t{x} + 1) + int64_t{x} * (int64_t{in_digit} + C);
    }
    return {x, z};
}

void go_left(int i, int limit, int64_t prev_input, int64_t prev_z, auto fn) {
    if (i == limit) {
        // fn(input[limit - 1], z[limit - 1])
        fn(prev_input, prev_z);
        return;
    }
    //if (i == 2) {
    //    std::cout << "left state: " << std::setw(3) << prev_input << "\n";
    //}
    auto [A, B, C] = params[i];
    for (int digit = 9; digit >= 1; --digit) {
        int64_t input = prev_input * 10 + digit;
        int x = (prev_z % 26 + B) != digit;
        int64_t z = prev_z / A * (25 * x + 1) + x * (digit + C);
        go_left(i + 1, limit, input, z, fn);
    }
}

std::tuple<int64_t, int64_t> calc_zbounds(int x, int64_t z, int inp, int C, int A) {
    int64_t alpha = (z - int64_t{x} * (int64_t{inp} + C)) / (25 * int64_t{x} + 1);
    return {alpha * A, alpha * A + A - 1};
};

void go_right(int i, int limit, int64_t prev_input, int prev_digit, int prev_x, int64_t prev_z, auto fn) {
    if (i == 13) {
        for (int digit = 1; digit <= 9; ++digit) {
            go_right(i - 1, limit, digit, digit, 0, 0, fn);
        }
        return;
    }
    if (i == limit) {
        // fn(...[limit + 1])
        fn(prev_input, prev_x, prev_z);
        return;
    }

    auto [A, B, C] = params[i + 1];
    auto [minz, maxz] = calc_zbounds(prev_x, prev_z, prev_digit, C, A);
    for (int64_t z = minz; z <= maxz; ++z) {
        if (prev_x != ((z % 26 + B) != prev_digit)) {
            continue;
        }
        for (int x = 0; x <= 1; ++x) {
            for (int digit = 1; digit <= 9; ++digit) {
                int64_t input = prev_input + digit * pow10[N - 1 - i];
                go_right(i - 1, limit, input, digit, x, z, fn);
            }
        }
    }
}

const int MAX_INTERM_Z = 100000000;
std::array<int64_t, MAX_INTERM_Z> interm_state;

auto solve = [](auto &is, auto &os, int64_t init_best_answer, auto is_better_than) {
    std::fill(interm_state.begin(), interm_state.end(), -1);

    go_left(0,
            //11,
            10,
            0, 0, [&is_better_than](int64_t input, int64_t z) {
                if (z >= MAX_INTERM_Z) {
                    return;
                }
                if (interm_state[z] == -1 || is_better_than(input, interm_state[z])) {
                    interm_state[z] = input;
                }
            });

    int64_t best_answer = init_best_answer;
    go_right(13, 8, 0, 0, 0, 0, [&best_answer, &is_better_than](int64_t input, int x, int64_t z) {
        if (interm_state[z] != -1) {
            int64_t combined = interm_state[z] * pow10[4] + input % pow10[4];
            auto [check_x, check_z] = eval(combined);
            if (check_z != 0) {
                std::cout << "failed final check: combined=" << combined << " z=" << check_z << "\n";
                std::cout << "                    stt[z]=" << interm_state[z] << " right_inp=" << input << "\n";
                return;
            }
            if (is_better_than(combined, best_answer)) {
                best_answer = combined;
                //std::cout << "best answer so far: " << best_answer << "\n";
            }
        }
    });

    os << best_answer;
};

auto solve1 = [](auto &is, auto &os) {
    return solve(is, os, std::numeric_limits<int64_t>::min(), std::greater<int64_t>());
};

auto solve2 = [](auto &is, auto &os) {
    return solve(is, os, std::numeric_limits<int64_t>::max(), std::less<int64_t>());
};

void main() {
    {
        Utils::Tester tester{"day24"};

        //tester.test("part1", "in1", "96929994293996", solve1);
        //tester.test("part2", "in1", "41811761181141", solve2);
        
    }

	Utils::bench("day24", "part1", "in1", "96929994293996", 1, solve1);
	Utils::bench("day24", "part2", "in1", "41811761181141", 1, solve2);
}

} // namespace Day24A