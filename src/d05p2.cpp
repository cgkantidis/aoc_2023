#include <algorithm>
#include <fmt/core.h>
#include <fstream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

#include "utility.hpp"

struct mapping
{
  std::size_t dst;
  std::size_t src;
  std::size_t sz;
};

std::vector<std::size_t>
parse_seeds(std::ifstream &infile) {
  std::string line;
  std::getline(infile, line);
  auto str_seeds = split(split(line, "seeds: ")[0]);
  std::vector<std::size_t> seeds;
  std::ranges::transform(str_seeds, std::back_inserter(seeds), str_to_int<std::size_t>);
  std::getline(infile, line); // empty line
  return seeds;
}

std::size_t
convert(std::size_t value, std::vector<mapping> const &map) {
  for (mapping const &m : map) {
    if (value < m.src) {
      // value is not mapped
      return value;
    }
    if (value < m.src + m.sz) {
      return m.dst + value - m.src;
    }
  }
  return value;
}

std::size_t
convert(std::size_t value, std::vector<std::vector<mapping>> const &mappings) {
  return std::accumulate(
      mappings.begin(),
      mappings.end(),
      value,
      [](std::size_t prev_value, std::vector<mapping> const &map) {
        return convert(prev_value, map);
      });
}

std::vector<std::vector<mapping>>
parse_mappings(std::ifstream &infile) {
  std::vector<std::vector<mapping>> mappings(7);
  for (std::vector<mapping> &map : mappings) {
    bool is_first = true;
    for (std::string line; std::getline(infile, line) && !line.empty();) {
      if (is_first) {
        is_first = false;
        continue;
      }
      auto tokens = split(line) | std::views::transform(str_to_int<std::size_t>);
      map.emplace_back(tokens[0], tokens[1], tokens[2]);
    }
    std::ranges::sort(map, [](mapping const &left, mapping const &right) {
      return left.src < right.src;
    });
  }
  return mappings;
}

std::size_t
min(std::vector<std::size_t> const &values) {
  std::size_t min_value{std::numeric_limits<std::size_t>::max()};
  for (std::size_t const &value : values) {
    if (value < min_value) {
      min_value = value;
    }
  }
  return min_value;
}

int
main(int argc, char const **argv) {
  auto args = std::span(argv, size_t(argc));
  if (args.size() != 2) {
    fmt::println(stderr, "usage: {} input.txt", args[0]);
    return 1;
  }

  std::ifstream infile(args[1]);
  if (!infile.is_open()) {
    fmt::println(stderr, "couldn't open file {}", args[1]);
    return 2;
  }

  auto seeds = parse_seeds(infile);
  auto mappings = parse_mappings(infile);
  std::size_t min_location{std::numeric_limits<std::size_t>::max()};
  for (auto chunk : std::views::chunk(seeds, 2)) {
    for (std::size_t seed = chunk[0]; seed < chunk[0] + chunk[1]; ++seed) {
      auto location = convert(seed, mappings);
      if (location < min_location) {
        min_location = location;
      }
    }
  }
  fmt::println("{}", min_location);

  return 0;
}
