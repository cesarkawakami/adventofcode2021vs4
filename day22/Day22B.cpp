#include "Utils.h"
#include <cassert>

namespace Day22B {

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

int clamp(int min, int max, int value) {
    return std::min(max, std::max(min, value));
}

struct OCNode {
    int delta;
    int xmin, xmax, ymin, ymax, zmin, zmax;
    bool subdivided;
    int xmid, ymid, zmid;
    std::unique_ptr<OCNode> lll, llh, lhl, lhh, hll, hlh, hhl, hhh;

    static OCNode make(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax) {
        return {0, xmin, xmax, ymin, ymax, zmin, zmax, false, -1, -1, -1};
    }

    void apply(int adelta, int ax, int ay, int az) {
        if (ax >= xmax || ay >= ymax || az >= zmax) {
            return;
        }
        if (ax <= xmin && ay <= ymin && az <= zmin) {
            delta += adelta;
            return;
        }

        if (!subdivided) {
            subdivided = true;
            xmid = clamp(xmin, xmax, ax);
            ymid = clamp(ymin, ymax, ay);
            zmid = clamp(zmin, zmax, az);
            lll = std::make_unique<OCNode>(OCNode::make(xmin, xmid, ymin, ymid, zmin, zmid));
            llh = std::make_unique<OCNode>(OCNode::make(xmin, xmid, ymin, ymid, zmid, zmax));
            lhl = std::make_unique<OCNode>(OCNode::make(xmin, xmid, ymid, ymax, zmin, zmid));
            lhh = std::make_unique<OCNode>(OCNode::make(xmin, xmid, ymid, ymax, zmid, zmax));
            hll = std::make_unique<OCNode>(OCNode::make(xmid, xmax, ymin, ymid, zmin, zmid));
            hlh = std::make_unique<OCNode>(OCNode::make(xmid, xmax, ymin, ymid, zmid, zmax));
            hhl = std::make_unique<OCNode>(OCNode::make(xmid, xmax, ymid, ymax, zmin, zmid));
            hhh = std::make_unique<OCNode>(OCNode::make(xmid, xmax, ymid, ymax, zmid, zmax));
        }

        lll->apply(adelta, ax, ay, az);
        llh->apply(adelta, ax, ay, az);
        lhl->apply(adelta, ax, ay, az);
        lhh->apply(adelta, ax, ay, az);
        hll->apply(adelta, ax, ay, az);
        hlh->apply(adelta, ax, ay, az);
        hhl->apply(adelta, ax, ay, az);
        hhh->apply(adelta, ax, ay, az);
    }

    void apply(int adelta, int axmin, int axmax, int aymin, int aymax, int azmin, int azmax) {
        apply(adelta, axmin, aymin, azmin);
        apply(-adelta, axmin, aymin, azmax);
        apply(-adelta, axmin, aymax, azmin);
        apply(-adelta, axmax, aymin, azmin);
        apply(adelta, axmin, aymax, azmax);
        apply(adelta, axmax, aymin, azmax);
        apply(adelta, axmax, aymax, azmin);
        apply(-adelta, axmax, aymax, azmax);
    }

    int64_t count_on(int acc_delta, int min, int max) const {
        acc_delta += delta;
        assert(acc_delta >= 0);
        if (!subdivided) {
            if (acc_delta == 0) {
                return 0;
            } else {
                int64_t xsize = int64_t{clamp(min, max, xmax)} - clamp(min, max, xmin);
                int64_t ysize = int64_t{clamp(min, max, ymax)} - clamp(min, max, ymin);
                int64_t zsize = int64_t{clamp(min, max, zmax)} - clamp(min, max, zmin);
                return xsize * ysize * zsize;
            }
        }
        int64_t rv = 0;
        rv += lll->count_on(acc_delta, min, max);
        rv += llh->count_on(acc_delta, min, max);
        rv += lhl->count_on(acc_delta, min, max);
        rv += lhh->count_on(acc_delta, min, max);
        rv += hll->count_on(acc_delta, min, max);
        rv += hlh->count_on(acc_delta, min, max);
        rv += hhl->count_on(acc_delta, min, max);
        rv += hhh->count_on(acc_delta, min, max);
        return rv;
    }
};

const int MIN_COORD = std::numeric_limits<int>::min() / 2 + 10;
const int MAX_COORD = std::numeric_limits<int>::max() / 2 - 10;

auto solve = [](auto &is, auto &os, int min, int max) {
    auto cuboids = read(is);
    auto octree{std::make_unique<OCNode>(OCNode::make(MIN_COORD, MAX_COORD, MIN_COORD, MAX_COORD, MIN_COORD, MAX_COORD))};
    for (const InputCuboid &cuboid : cuboids) {
        octree->apply(1, cuboid.xmin, cuboid.xmax + 1, cuboid.ymin, cuboid.ymax + 1, cuboid.zmin, cuboid.zmax + 1);
    }
    os << octree->count_on(0, min, max);
};

auto solve1 = [](auto &is, auto &os) { solve(is, os, -50, 51); };
auto solve2 = [](auto &is, auto &os) { solve(is, os, MIN_COORD, MAX_COORD); };

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

} // namespace Day22B