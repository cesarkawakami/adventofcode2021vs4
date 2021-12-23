#include "Utils.h"
#include <cassert>

namespace Day22A {

struct InputCuboid {
    bool on;
    int xmin, xmax, ymin, ymax, zmin, zmax;
};

std::optional<InputCuboid> read_one(std::istringstream &is) {
    InputCuboid rv{};
    std::string onoff, coordbuf;
    is >> onoff >> coordbuf;
    if (!is) {
        return std::nullopt;
    }
    rv.on = onoff == "on";

    for (char &c : coordbuf) {
        if (c != '-' && (c < '0' || c > '9')) {
            c = ' ';
        }
    }
    std::istringstream{coordbuf} >> rv.xmin >> rv.xmax >> rv.ymin >> rv.ymax >> rv.zmin >> rv.zmax;
    return rv;
}

std::vector<InputCuboid> read(std::istringstream &is) {
    std::vector<InputCuboid> rv;
    while (true) {
        auto value = read_one(is);
        if (!value.has_value()) {
            break;
        }
        rv.push_back(*value);
    }
    return rv;
}

struct Reactor {
    std::vector<int> xticks, yticks, zticks;
    std::vector<bool> matrix;

    int64_t index(const std::tuple<int, int, int>& tick) const {
        const auto &[xt, yt, zt] = tick;
        return (xt * std::ssize(yticks) + yt) * std::ssize(zticks) + zt;
    }

    bool set(const std::tuple<int, int, int>& tick, bool value) {
        matrix[index(tick)] = value;
        return value;
    }

    bool at(const std::tuple<int, int, int> &tick) const {
        return matrix[index(tick)];
    }

    void ingest_cuboid(const InputCuboid &cuboid) {
        auto find_tick = [](auto &ticks, int cmin, int cmax) -> std::tuple<int, int> {
            int c_imin = std::lower_bound(ticks.begin(), ticks.end(), cmin) - ticks.begin();
            int c_imax = std::lower_bound(ticks.begin(), ticks.end(), cmax + 1) - ticks.begin();
            assert(ticks[c_imin] == cmin);
            assert(ticks[c_imax] == cmax + 1);
            return {c_imin, c_imax};
        };
        const auto [x_imin, x_imax] = find_tick(xticks, cuboid.xmin, cuboid.xmax);
        const auto [y_imin, y_imax] = find_tick(yticks, cuboid.ymin, cuboid.ymax);
        const auto [z_imin, z_imax] = find_tick(zticks, cuboid.zmin, cuboid.zmax);

        for (int x_i = x_imin; x_i < x_imax; ++x_i) {
            for (int y_i = y_imin; y_i < y_imax; ++y_i) {
                for (int z_i = z_imin; z_i < z_imax; ++z_i) {
                    set({x_i, y_i, z_i}, cuboid.on);
                }
            }
        }
    }

    int64_t on_count(int min, int max) const {
        max += 1;
        auto clamp = [](int min, int max, int val) {
            return std::min(max, std::max(min, val));
        };
        int64_t count = 0;
        for (int x_i = 0; x_i < std::ssize(xticks) - 1; ++x_i) {
            for (int y_i = 0; y_i < std::ssize(yticks) - 1; ++y_i) {
                for (int z_i = 0; z_i < std::ssize(zticks) - 1; ++z_i) {
                    int xsize = clamp(min, max, xticks[x_i + 1]) - clamp(min, max, xticks[x_i]);
                    int ysize = clamp(min, max, yticks[y_i + 1]) - clamp(min, max, yticks[y_i]);
                    int zsize = clamp(min, max, zticks[z_i + 1]) - clamp(min, max, zticks[z_i]);
                    count += int64_t{xsize} * ysize * zsize * at({x_i, y_i, z_i});
                }
            }
        }
        return count;
    }

    static Reactor make(const std::vector<int> &xticks, const std::vector<int> &yticks, const std::vector<int> &zticks) {
        std::vector<bool> matrix(std::size(xticks) * std::size(yticks) * std::size(zticks), false);
        return {xticks, yticks, zticks, matrix};
    }

    static Reactor from_cuboids(const std::vector<InputCuboid> &cuboids) {
        std::vector<int> xcoords, ycoords, zcoords;
        auto add_three = [](auto &coords, int val) {
            coords.push_back(val);
            coords.push_back(val + 1);
        };
        for (const auto &cuboid : cuboids) {
            add_three(xcoords, cuboid.xmin);
            add_three(xcoords, cuboid.xmax);
            add_three(ycoords, cuboid.ymin);
            add_three(ycoords, cuboid.ymax);
            add_three(zcoords, cuboid.zmin);
            add_three(zcoords, cuboid.zmax);
        }
        auto uniqueize = [](auto &coords) {
            std::sort(coords.begin(), coords.end());
            coords.erase(std::unique(coords.begin(), coords.end()), coords.end());
        };
        uniqueize(xcoords);
        uniqueize(ycoords);
        uniqueize(zcoords);
        return make(xcoords, ycoords, zcoords);
    }
};

auto solve = [](auto &is, auto &os, int min, int max) {
    auto cuboids = read(is);
    auto reactor = Reactor::from_cuboids(cuboids);
    int done_count = 0;
    for (const auto &cuboid : cuboids) {
        reactor.ingest_cuboid(cuboid);
        std::cout << "done: " << done_count++ << " out of " << std::size(cuboids) << "\n";
    }
    os << reactor.on_count(min, max);
};

auto solve1 = [](auto &is, auto &os) {
    return solve(is, os, -50, 50);
};

auto solve2 = [](auto &is, auto &os) {
    return solve(is, os, std::numeric_limits<int>::min(), std::numeric_limits<int>::max() - 1);
};

void main() {
    {
        Utils::Tester tester{"day22"};

        tester.test("part1", "in1", "39", solve1);
        tester.test("part1", "in2", "590784", solve1);
        tester.test("part1", "in3", "609563", solve1);

        tester.test("part2", "in4", "2758514936282235", solve2);
        tester.test("part2", "in3", "", solve2);
    }
}
} // namespace Day22A