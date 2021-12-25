#include "Utils.h"
#include <array>

namespace Day25A {

const int MAXN = 150;

struct Seafloor {
    int R, C;
    std::vector<char> data;

    auto operator<=>(const Seafloor &) const = default;

    void read(std::istringstream &is) {
        data.clear();
        std::string line;
        for (R = 0; is >> line; ++R) {
            C = static_cast<int>(std::ssize(line));
            std::copy(line.begin(), line.end(), std::back_inserter(data));
        }
    }

    char &operator[](const std::tuple<int, int> &coord) {
        const auto &[r, c] = coord;
        return data[r * C + c];
    }

    const char &operator[](const std::tuple<int, int> &coord) const {
        const auto &[r, c] = coord;
        return data[r * C + c];
    }

    bool move_one_dir(char ch, int dr, int dc) {
        static Seafloor old;
        old = *this;
        for (int r = 0; r < R; ++r) {
            for (int c = 0; c < C; ++c) {
                (*this)[{r, c}] = '.';
            }
        }
        bool any_moved = false;
        for (int r = 0; r < R; ++r) {
            for (int c = 0; c < C; ++c) {
                int nr = (r + dr) % R, nc = (c + dc) % C;
                if (old[{r, c}] == ch && old[{nr, nc}] == '.') {
                    any_moved = true;
                    (*this)[{nr, nc}] = ch;
                } else if (old[{r, c}] != '.') {
                    (*this)[{r, c}] = old[{r, c}];
                }
            }
        }
        return any_moved;
    }

    bool move_one_step() {
        bool moved_right = move_one_dir('>', 0, 1);
        bool moved_down = move_one_dir('v', 1, 0);
        return moved_right || moved_down;
    }

    std::string to_string() const {
        std::string s;
        for (int r = 0; r < R; ++r) {
            for (int c = 0; c < C; ++c) {
                s.push_back((*this)[{r, c}]);
            }
            s.push_back('\n');
        }
        return s;
    }
};

Seafloor seafloor;

auto solve1 = [](auto &is, auto &os) {
    seafloor.read(is);
    //std::cout << "Initial state:\n";
    //std::cout << seafloor.to_string();
    //std::cout << "\n";
    for (int step = 1;; ++step) {
        bool any_moved = seafloor.move_one_step();
        //std::cout << "After " << step << " steps:\n";
        //std::cout << seafloor.to_string();
        //std::cout << "\n";
        if (!any_moved) {
            os << step;
            return;
        }
    }
};

void main() {
    {
        Utils::Tester tester{"day25"};

        tester.test("part1", "in1", "58", solve1);
        tester.test("part1", "in2", "520", solve1);
    }
}

} // namespace Day25A