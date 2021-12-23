#include "Utils.h"

namespace Day20A {

const int N = 500;
char matrix[N][N];
char tmpmtx[N][N];

auto solve = [](std::istringstream &is, std::ostringstream &os, int enhancement_count) {
    std::string enhance;
    is >> enhance;

    std::vector<std::string> input;
    std::copy(std::istream_iterator<std::string>(is), std::istream_iterator<std::string>(), std::back_inserter(input));
    int inputR = std::ssize(input);
    int inputC = std::ssize(input[0]);

    memset(matrix, '.', sizeof(matrix));
    for (int r = 0; r < inputR; ++r) {
        for (int c = 0; c < inputC; ++c) {
            matrix[N / 2 - inputR / 2 + r][N / 2 - inputC / 2 + c] = input[r][c];
        }
    }

    for (int count = 0; count < enhancement_count; ++count) {
        int on_count = 0;
        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                on_count += matrix[r][c] == '#';
            }
        }
        std::cout << "before iter=" << count << ": count=" << on_count << "\n";

        for (int r = 0; r < N; ++r) {
            for (int c = 0; c < N; ++c) {
                int enhance_idx = 0;
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        int nr = r + dr, nc = c + dc;
                        int v = 0;
                        if (0 <= nr && nr < N && 0 <= nc && nc < N) {
                            v = matrix[nr][nc] == '#';
                        } else {
                            v = matrix[0][0] == '#';
                        }
                        enhance_idx = (enhance_idx << 1) | v;
                    }
                }
                tmpmtx[r][c] = enhance[enhance_idx];
            }
        }
        memcpy(matrix, tmpmtx, sizeof(tmpmtx));
    }

    int on_count = 0;
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            on_count += matrix[r][c] == '#';
        }
    }
    os << on_count;
};

auto solve1 = [](auto &is, auto &os) { return solve(is, os, 2); };
auto solve2 = [](auto &is, auto &os) { return solve(is, os, 50); };

void main() {
    {
        Utils::Tester tester{"day20"};

        tester.test("part1", "in1", "35", solve1);
        tester.test("part1", "in2", "5819", solve1);
        tester.test("part1", "in1", "35", solve1);
        tester.test("part1", "in2", "5819", solve1);
        tester.test("part2", "in1", "3351", solve2);
        tester.test("part2", "in2", "", solve2);
    }
}
} // namespace Day20A