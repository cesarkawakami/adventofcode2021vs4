#include "Utils.h"

namespace Day16A {

char htoi(char x) {
    char z = (x >> 6) & 1;
    return (x & 0xf) + (z | (z << 3));
}

std::vector<uint64_t> read_input(std::istringstream &is) {
    std::vector<uint64_t> rv;
    rv.reserve((is.str().length() + 15) / 16);
    uint64_t partial = 0;
    int partial_count = 0;
    for (char c : is.str()) {
        if (c >= '0') {
            int v = htoi(c);
            if (partial_count >= 64) {
                rv.push_back(partial);
                partial = partial_count = 0;
            }
            partial = (partial << 4) | v;
            partial_count += 4;
        }
    }
    if (partial_count) {
        rv.push_back(partial << (64 - partial_count));
    }
    return rv;
}

struct Parser {
    const std::vector<uint64_t> &data_v;
    int pos, subpos;

    int version_sum = 0;

    static Parser make(const std::vector<uint64_t> &start) {
        return {start, 0, 0};
    }

    std::ostream &indented(std::ostream &os, int depth) {
        for (int i = 0; i < depth; ++i) {
            os << "  ";
        }
        return os;
    }

    int get() {
        int rv = bool(data_v[pos] & (uint64_t{1} << (63 - subpos)));
        ++subpos;
        if (subpos == 64) {
            subpos = 0;
            ++pos;
        }
        return rv;
    }

    int getpos() {
        return pos * 64 + subpos;
    }

    int read(const int original_count) {
        int remaining = original_count;
        int v = 0;
        while (remaining) {
            int chunk_clamp = 64 - subpos;
            int chunk = std::min(remaining, chunk_clamp);
            remaining -= chunk;
            int chunk_bits = (data_v[pos] >> (64 - chunk - subpos)) & ((uint64_t{1} << chunk) - 1);
            v = (v << chunk) | chunk_bits;
            subpos += chunk;
            if (subpos == 64) {
                subpos = 0;
                ++pos;
            }
        }
        return v;
    }

    int64_t parse_packet() {
        int version = read(3);
        int type = read(3);

        version_sum += version;

        if (type == 4) {
            return parse_literal();
        } else if (type == 0) {
            return parse_operator_acc(0, [](int64_t acc, int64_t v) { return acc + v; });
        } else if (type == 1) {
            return parse_operator_acc(1, [](int64_t acc, int64_t v) { return acc * v; });
        } else if (type == 2) {
            return parse_operator_acc(std::numeric_limits<int64_t>::max(), [](int64_t acc, int64_t v) { return std::min(acc, v); });
        } else if (type == 3) {
            return parse_operator_acc(std::numeric_limits<int64_t>::min(), [](int64_t acc, int64_t v) { return std::max(acc, v); });
        } else if (type == 5) {
            return parse_operator_pair([](int64_t left, int64_t right) { return left > right; });
        } else if (type == 6) {
            return parse_operator_pair([](int64_t left, int64_t right) { return left < right; });
        } else if (type == 7) {
            return parse_operator_pair([](int64_t left, int64_t right) { return left == right; });
        }
        abort();
    }

    int64_t parse_operator_acc(int64_t init, auto acc) {
        int64_t value = init;
        parse_multi([&](int64_t v) { value = acc(value, v); });
        return value;
    }

    int64_t parse_operator_pair(auto fn) {
        int current_index = 0;
        int64_t left, right;
        parse_multi([&](int64_t v) {
            if (current_index == 0) {
                left = v;
            } else if (current_index == 1) {
                right = v;
            } else {
                abort();
            }
            ++current_index;
        });
        int64_t value = fn(left, right);
        return value;
    }

    void parse_multi(auto fn) {
        int length_type = read(1);
        if (length_type) {
            int subpacket_count = read(11);
            for (int i = 0; i < subpacket_count; ++i) {
                int64_t value = parse_packet();
                fn(value);
            }
        } else {
            int total_sublength = read(15);
            auto start_pos = getpos();
            while (getpos() - start_pos < total_sublength) {
                int64_t value = parse_packet();
                fn(value);
            }
        }
    }

    int64_t parse_literal() {
        int64_t v = 0;
        while (true) {
            int x = read(5);
            v = (v << 4) | (x & 0xf);
            if (!(x & 0x10)) {
                break;
            }
        }
        return v;
    }
};

auto solve1 = [](std::istringstream &is, std::ostream &os) {
    auto input = read_input(is);
    auto parser = Parser::make(input);
    parser.parse_packet();
    os << parser.version_sum;
};

auto solve2 = [](std::istringstream &is, std::ostream &os) {
    auto input = read_input(is);
    auto parser = Parser::make(input);
    os << parser.parse_packet();
};

void main() {
    {
        Utils::Tester tester{"day16"};

        tester.test("part1", "in-2", "6", solve1);
        tester.test("part1", "in-1", "9", solve1);
        tester.test("part1", "in0", "14", solve1);
        tester.test("part1", "in1", "16", solve1);
        tester.test("part1", "in2", "12", solve1);
        tester.test("part1", "in3", "23", solve1);
        tester.test("part1", "in4", "31", solve1);
        tester.test("part1", "in5", "974", solve1);

        tester.test("part2", "in6", "3", solve2);
        tester.test("part2", "in7", "54", solve2);
        tester.test("part2", "in8", "7", solve2);
        tester.test("part2", "in9", "9", solve2);
        tester.test("part2", "in10", "1", solve2);
        tester.test("part2", "in11", "0", solve2);
        tester.test("part2", "in13", "1", solve2);
        tester.test("part2", "in5", "180616437720", solve2);
    }

    Utils::bench("day16", "part2", "in5", "180616437720", 10000, solve2);
}
} // namespace Day16A
