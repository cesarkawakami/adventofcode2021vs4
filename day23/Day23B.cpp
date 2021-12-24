#include "Utils.h"
#include <array>
#include <cassert>
#include <map>
#include <queue>

namespace Day23B {

const int N = 23;
const int M = 16;

const std::array<bool, N> is_room{0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0};
const std::array<int, N> hallmap{0, 1, 2, 2, 2, 2, 3, 4, 4, 4, 4, 5, 6, 6, 6, 6, 7, 8, 8, 8, 8, 9, 10};
const std::array<int, 11> hallidx{0, 1, -1, 6, -1, 11, -1, 16, -1, 21, 22};

const std::array<int, M> cost_per_step{1, 1, 1, 1, 10, 10, 10, 10, 100, 100, 100, 100, 1000, 1000, 1000, 1000};
const std::array<int, M> tgt_for_estim{2, 2, 2, 2, 7, 7, 7, 7, 12, 12, 12, 12, 17, 17, 17, 17};
const std::array<int, M> type_of_posidx{0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
const std::array<int, N> type_of_position{-1, -1, 0, 0, 0, 0, -1, 1, 1, 1, 1, -1, 2, 2, 2, 2, -1, 3, 3, 3, 3, -1, -1};
const std::array<bool, N> is_bottom{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0};
const std::array<int, N> calcd_dist_to_hallway{-1, -1, 1, 2, 3, 4, -1, 1, 2, 3, 4, -1, 1, 2, 3, 4, -1, 1, 2, 3, 4, -1, -1};

const std::array<std::array<int, N>, N> calcd_dist = []() {
    auto calc_dist = [](auto rec, int a, int b) {
        if (is_room[a] && !is_room[b]) {
            int dist_to_hallway = calcd_dist_to_hallway[a];
            int in_hallway_dist = std::abs(hallmap[a] - hallmap[b]);
            return dist_to_hallway + in_hallway_dist;
        } else if (!is_room[a] && is_room[b]) {
            return rec(rec, b, a);
        }
        return -1;
    };

    std::array<std::array<int, N>, N> dists;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            dists[i][j] = calc_dist(calc_dist, i, j);
        }
    }
    return dists;
}();

const std::array<std::array<std::vector<int>, N>, N> calcd_path = []() {
    auto build_path = [](auto rec, int a, int b) -> std::vector<int> {
        if (is_room[a] && !is_room[b]) {
            std::vector<int> path{a};
            for (int i = 1; i <= calcd_dist_to_hallway[a] - 1; ++i) {
                path.push_back(a - i);
            }

            int hallstep = (hallmap[a] < hallmap[b]) ? 1 : -1;
            for (int i = hallmap[a];; i += hallstep) {
                if (hallidx[i] != -1) {
                    path.push_back(hallidx[i]);
                }
                if (i == hallmap[b]) {
                    break;
                }
            }
            return path;
        } else if (!is_room[a] && is_room[b]) {
            auto rv = rec(rec, b, a);
            std::reverse(rv.begin(), rv.end());
            return rv;
        } else {
            return {};
        }
    };

    std::array<std::array<std::vector<int>, N>, N> paths;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            paths[i][j] = build_path(build_path, i, j);
        }
    }
    return paths;
}();

struct State {
    std::array<int, M> positions;

    auto operator<=>(const State &other) const = default;

    bool occupied(int p) const {
        for (int pos : positions) {
            if (p == pos) {
                return true;
            }
        }
        return false;
    }

    void for_each_next_pos(int pos_idx, auto fn) const {
        int p = positions[pos_idx];
        for (int r = 0; r < N; ++r) {
            int dist = calcd_dist[p][r];
            if (dist == -1) {
                continue;
            }

            bool path_is_viable = true;
            for (int interm : calcd_path[p][r]) {
                if (interm == p) {
                    continue;
                }
                if (occupied(interm)) {
                    path_is_viable = false;
                    break;
                }
            }
            if (!path_is_viable) {
                continue;
            }

            if (!fn(r, dist)) {
                break;
            }
        }
    }

    void for_each_next_state(auto fn) const {
        bool found_final = false;
        for (int pos_idx = 0; pos_idx < M; ++pos_idx) {
            if (is_final_pos(pos_idx)) {
                continue;
            }

            for_each_next_pos(pos_idx, [&fn, pos_idx, this, &found_final](int next, int dist) {
                State next_state{*this};
                next_state.positions[pos_idx] = next;
                if (!is_room[positions[pos_idx]] && !next_state.is_final_pos(pos_idx)) {
                    return true;
                }
                if (next_state.is_final_pos(pos_idx)) {
                    fn(next_state, dist * cost_per_step[pos_idx]);
                    found_final = true;
                    return false;
                }
                return true;
            });
        }
        if (found_final) {
            return;
        }

        for (int pos_idx = 0; pos_idx < M; ++pos_idx) {
            if (is_final_pos(pos_idx)) {
                continue;
            }

            for_each_next_pos(pos_idx, [&fn, pos_idx, this](int next, int dist) {
                State next_state{*this};
                next_state.positions[pos_idx] = next;
                if (!is_room[positions[pos_idx]] && !next_state.is_final_pos(pos_idx)) {
                    return true;
                }
                fn(next_state, dist * cost_per_step[pos_idx]);
                return true;
            });
        }
    }

    bool in_final_room(int i) const {
        int p = positions[i];
        return type_of_posidx[i] == type_of_position[p];
    }

    bool is_final_pos(int i) const {
        int p = positions[i];
        if (in_final_room(i)) {
            std::array<bool, 5> asdf{};
            for (int j = 0; j < 4; ++j) {
                int k = i - i % 4 + j;
                if (in_final_room(k)) {
                    asdf[calcd_dist_to_hallway[positions[k]]] = true;
                }
            }
            for (int j = calcd_dist_to_hallway[p]; j <= 4; ++j) {
                if (!asdf[j]) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    bool is_final() const {
        for (int i = 0; i < M; ++i) {
            if (!is_final_pos(i)) {
                return false;
            }
        }
        return true;
    }

    int estimate() const {
        int rv = 0;
        for (int i = 0; i < M; ++i) {
            if (!is_final_pos(i)) {
                int best_dist = std::numeric_limits<int>::max();
                for (int j = 0; j < N; ++j) {
                    if (!is_room[j]) {
                        int this_dist = calcd_dist[positions[i]][j] + calcd_dist[j][tgt_for_estim[i]];
                        this_dist *= cost_per_step[i];
                        best_dist = std::min(best_dist, this_dist);
                    }
                }
                rv += best_dist;
            }
        }
        return rv;
    }

    static State read(std::istringstream &is) {
        std::string buf;
        std::getline(is, buf);
        std::getline(is, buf);
        std::array<std::string, 4> lines{
            "",
            "  #D#C#B#A#",
            "  #D#B#A#C#",
            "",
        };
        std::getline(is, lines[0]);
        std::getline(is, lines[3]);

        std::map<char, std::vector<int>> pods;
        const std::array<std::tuple<int, int, int>, M> cases{{
            {0, 3, 2},
            {1, 3, 3},
            {2, 3, 4},
            {3, 3, 5},
            {0, 5, 7},
            {1, 5, 8},
            {2, 5, 9},
            {3, 5, 10},
            {0, 7, 12},
            {1, 7, 13},
            {2, 7, 14},
            {3, 7, 15},
            {0, 9, 17},
            {1, 9, 18},
            {2, 9, 19},
            {3, 9, 20},
        }};
        for (const auto &[row, col, pos] : cases) {
            char c = lines[row][col];
            pods[c].push_back(pos);
        }
        assert(std::ssize(pods['A']) == 4);
        assert(std::ssize(pods['B']) == 4);
        assert(std::ssize(pods['C']) == 4);
        assert(std::ssize(pods['D']) == 4);
        return State{{
            pods['A'][0],
            pods['A'][1],
            pods['A'][2],
            pods['A'][3],

            pods['B'][0],
            pods['B'][1],
            pods['B'][2],
            pods['B'][3],

            pods['C'][0],
            pods['C'][1],
            pods['C'][2],
            pods['C'][3],

            pods['D'][0],
            pods['D'][1],
            pods['D'][2],
            pods['D'][3],
        }};
    }
};

template <typename T>
using MaxHeap = std::priority_queue<T, std::vector<T>, std::greater<T>>;

auto solve2 = [](auto &is, auto &os) {
    State initial{State::read(is)};

    //int best_dist = std::numeric_limits<int>::max();
    int best_dist = 100000;
    int best_finalcount = 0;
    auto go = [&best_dist, &best_finalcount](auto go, const State &state, int dist) {
        int finalcount = 0;
        for (int i = 0; i < M; ++i) {
            if (state.is_final_pos(i)) {
                ++finalcount;
            }
        }
        if (finalcount > best_finalcount) {
            best_finalcount = finalcount;
            std::cout << "best finalcount so far: " << best_finalcount << "\n";
        }
        if (state.is_final() && dist < best_dist) {
            best_dist = dist;
            std::cout << "best dist so far: " << best_dist << "\n";
            return;
        }
        if (dist + state.estimate() >= best_dist) {
            return;
        }
        state.for_each_next_state([&go, dist](const State &next_state, int step_dist) {
            go(go, next_state, dist + step_dist);
        });
    };
    go(go, initial, 0);

    os << best_dist;
};

void main() {
    {
        Utils::Tester tester{"day23"};

        tester.test("part2", "in1", "44169", solve2);
        tester.test("part2", "in2", "46772", solve2);
    }
}

} // namespace Day23A
