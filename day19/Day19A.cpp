#include "Utils.h"
#include <cassert>

namespace Day19A {

struct Point {
    int x, y, z;

    auto operator<=>(const Point &other) const = default;

    Point operator+(const Point &other) const {
        return {x + other.x, y + other.y, z + other.z};
    }
    Point operator-(const Point &other) const {
        return {x - other.x, y - other.y, z - other.z};
    }
    Point rotate_x() const {
        return {x, -z, y};
    }
    Point rotate_y() const {
        return {z, y, -x};
    }
    Point rotate_z() const {
        return {-y, x, z};
    }
};

struct Scanner {
    int id;
    std::vector<Point> beacons;
    Point shift{};

    Scanner transform(auto fn) const {
        std::vector<Point> new_points(beacons.size());
        std::transform(beacons.begin(), beacons.end(), new_points.begin(), fn);
        std::sort(new_points.begin(), new_points.end());
        return {id, new_points};
    }

    Scanner rotate_x() const {
        return transform([](const Point &p) { return p.rotate_x(); });
    }
    Scanner rotate_y() const {
        return transform([](const Point &p) { return p.rotate_y(); });
    }
    Scanner rotate_z() const {
        return transform([](const Point &p) { return p.rotate_z(); });
    }

    void for_each_rotation(auto fn) const {
        Scanner current_rotation{*this};
        for (int rotations_around_x = 0; rotations_around_x < 4; ++rotations_around_x) {
            fn(current_rotation);
            fn(current_rotation.rotate_z());                       // x faces y
            fn(current_rotation.rotate_y());                       // x faces -z
            fn(current_rotation.rotate_z().rotate_z());            // x faces -x
            fn(current_rotation.rotate_z().rotate_z().rotate_z()); // x faces -y
            fn(current_rotation.rotate_y().rotate_y().rotate_y()); // x faces z
            current_rotation = current_rotation.rotate_x();
        }
    }

    void for_each_candidate_delta(const Scanner &reference, auto fn) const {
        // *this + delta = reference  =>  delta = reference - *this
        for (const Point &my_point : beacons) {
            for (const Point &their_point : reference.beacons) {
                fn(their_point - my_point);
            }
        }
    }

    void for_each_delta_candidate(const Scanner &reference, auto fn) const {
        for_each_candidate_delta(reference, [&](const Point &delta) {
            Scanner new_scanner{transform([&](const Point &p) { return p + delta; })};
            new_scanner.shift = delta;
            fn(new_scanner);
        });
    }

    void for_each_all_candidate(const Scanner &reference, auto fn) const {
        for_each_rotation([&](const Scanner &rotated_scanner) {
            rotated_scanner.for_each_delta_candidate(reference, [&](const Scanner &deltad_scanner) {
                fn(deltad_scanner);
            });
        });
    }

    int common_points(const Scanner &other) const {
        assert(std::is_sorted(beacons.begin(), beacons.end()));
        assert(std::is_sorted(other.beacons.begin(), other.beacons.end()));
        int rv = 0;
        for (auto it{beacons.begin()}, jt{other.beacons.begin()}; it != beacons.end() && jt != other.beacons.end();) {
            if (*it < *jt) {
                ++it;
            } else if (*it > *jt) {
                ++jt;
            } else {
                ++rv;
                ++it;
                ++jt;
            }
        }
        return rv;
    }

    std::optional<Scanner> match(const Scanner &reference) const {
        std::optional<Scanner> rv{std::nullopt};
        for_each_all_candidate(reference, [&](const Scanner &candidate) {
            if (reference.common_points(candidate) >= 12 && !rv.has_value()) {
                rv = candidate;
            }
        });
        return rv;
    }

    static std::optional<Scanner> read(std::istringstream &is, int id) {
        std::string line;
        std::getline(is, line);
        if (line.find("scanner") == std::string::npos) {
            return std::nullopt;
        }
        Scanner rv{id};
        while (true) {
            std::getline(is, line);
            if (!is || line.find(',') == std::string::npos) {
                break;
            }
            int x, y, z;
            char ch;
            std::istringstream{line} >> x >> ch >> y >> ch >> z;
            rv.beacons.push_back({x, y, z});
        }
        std::sort(rv.beacons.begin(), rv.beacons.end());
        return rv;
    }
};

std::vector<Scanner> read_scanners(std::istringstream &is) {
    std::vector<Scanner> scanners;
    int scanner_index = 0;
    while (true) {
        auto s{Scanner::read(is, scanner_index++)};
        if (!s.has_value()) {
            break;
        }
        scanners.push_back(*s);
    }
    return scanners;
}

auto solve = [](std::istringstream &is, std::ostringstream &os, int which_part) {
    const auto original_scanners = read_scanners(is);
    assert(original_scanners.size() > 0);

    std::vector<Scanner> stable{original_scanners[0]};
    auto finding_loop = [&]() {
        for (const auto &scanner_to_attempt : original_scanners) {
            if (std::any_of(stable.begin(), stable.end(), [&](const Scanner &matched) { return matched.id == scanner_to_attempt.id; })) {
                continue;
            }

            for (const auto &stable_to_attempt : stable) {
                auto maybe_matched{scanner_to_attempt.match(stable_to_attempt)};
                if (maybe_matched.has_value()) {
                    stable.push_back(*maybe_matched);
                    return;
                }
            }
        }
        abort();
    };
    while (stable.size() < original_scanners.size()) {
        std::cout << "scanners matched: " << stable.size() << "\n";
        finding_loop();
    }

    if (which_part == 1) {
        std::vector<Point> all_points;
        for (const auto &scanner : stable) {
            std::copy(scanner.beacons.begin(), scanner.beacons.end(), std::back_inserter(all_points));
        }
        std::sort(all_points.begin(), all_points.end());
        all_points.erase(std::unique(all_points.begin(), all_points.end()), all_points.end());
        os << all_points.size();
    } else if (which_part == 2) {
        int max_manhattan = std::numeric_limits<int>::min();
        for (const auto &scanner_1 : stable) {
            for (const auto &scanner_2 : stable) {
                auto delta = scanner_2.shift - scanner_1.shift;
                int manhattan = std::abs(delta.x) + std::abs(delta.y) + std::abs(delta.z);
                max_manhattan = std::max(max_manhattan, manhattan);
            }
        }
        os << max_manhattan;
    } else {
        abort();
    }
};

void main() {
    {
        Utils::Tester tester{"day19"};

        tester.test("part1", "in1", "79", [](auto &is, auto &os) { return solve(is, os, 1); });
        tester.test("part1", "in2", "467", [](auto &is, auto &os) { return solve(is, os, 1); });

        tester.test("part2", "in1", "3621", [](auto &is, auto &os) { return solve(is, os, 2); });
        tester.test("part2", "in2", "12226", [](auto &is, auto &os) { return solve(is, os, 2); });
    }
}
} // namespace Day19A