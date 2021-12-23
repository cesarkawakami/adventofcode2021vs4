#include "Utils.h"
#include <map>

namespace Day21A {

int roll(int x) {
    return (x - 1) % 10 + 1;
}

auto solve1 = [](std::istringstream &is, std::ostringstream &os) {
    std::string buf;
    int p1start, p2start;
    is >> buf >> buf >> buf >> buf >> p1start >> buf >> buf >> buf >> buf >> p2start;

    int p1 = p1start, p2 = p2start;
    int64_t p1score = 0, p2score = 0;
    int diesum = 6 - 9;
    int dierolls = 0;
    int64_t losingscore;

    auto iterate = [](const std::string &name, int &pos, int64_t &score, int &diesum, int &dierolls, int64_t otherscore, int64_t &losingscore) {
        diesum += 9;
        dierolls += 3;
        pos = roll(pos + diesum);
        score += pos;
        //std::cout << name << ": pos=" << pos << " score=" << score << "\n";
        if (score >= 1000) {
            losingscore = otherscore;
            return false;
        }
        return true;
    };

    while (true) {
        if (!iterate("p1", p1, p1score, diesum, dierolls, p2score, losingscore)) {
            break;
        }
        if (!iterate("p2", p2, p2score, diesum, dierolls, p1score, losingscore)) {
            break;
        }
    }

    std::cout << "losingscore=" << losingscore << "\n";
    std::cout << "dierolls=   " << dierolls << "\n";

    os << losingscore * dierolls;
};

const std::vector<std::tuple<int, int>> ROLLS = {
    {3, 1},
    {4, 3},
    {5, 6},
    {6, 7},
    {7, 6},
    {8, 3},
    {9, 1},
};

auto solve2 = [](std::istringstream &is, std::ostringstream &os) {
    std::string buf;
    int p1start, p2start;
    is >> buf >> buf >> buf >> buf >> p1start >> buf >> buf >> buf >> buf >> p2start;

    const auto SENTINEL = std::tuple<int64_t, int64_t>{-1, -1};
    std::map<std::tuple<int, int, int, int>, std::tuple<int64_t, int64_t>> memo;
    auto go = [&](auto rec, int my_pos, int my_score, int their_pos, int their_score) -> std::tuple<int64_t, int64_t> {
        if (my_score >= 21) {
            return {1, 0};
        } else if (their_score >= 21) {
            return {0, 1};
        }
        if (memo.find({my_pos, my_score, their_pos, their_score}) != memo.end()) {
            return memo[{my_pos, my_score, their_pos, their_score}];
        }

        int64_t my_wins = 0, their_wins = 0;
        for (const auto [roll_value, roll_freq] : ROLLS) {
            int cur_my_pos = roll(my_pos + roll_value);
            const auto [cur_their_wins, cur_my_wins] = rec(rec, their_pos, their_score, cur_my_pos, my_score + cur_my_pos);
            my_wins += roll_freq * cur_my_wins;
            their_wins += roll_freq * cur_their_wins;
        }
        memo[{my_pos, my_score, their_pos, their_score}] = {my_wins, their_wins};
        return {my_wins, their_wins};
    };

    const auto [p1wins, p2wins] = go(go, p1start, 0, p2start, 0);
    std::cout << "p1 wins: " << p1wins << "\n";
    std::cout << "p2 wins: " << p2wins << "\n";
    os << std::max(p1wins, p2wins);
};

void main() {
    {
        Utils::Tester tester{"day21"};

        tester.test("part1", "in1", "739785", solve1);
        tester.test("part1", "in2", "720750", solve1);
        tester.test("part2", "in1", "444356092776315", solve2);
        tester.test("part2", "in2", "", solve2);
    }
}
} // namespace Day21A