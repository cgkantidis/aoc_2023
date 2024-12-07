#include "fmt/core.h"
#include "fmt/ranges.h"
#include "libassert/assert.hpp"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

#include "utility.hpp"

static void
tests();
static std::uint64_t
product_of_num_ways_to_win(std::ranges::range auto &&lines);
static std::pair<std::vector<std::uint64_t>, std::vector<std::uint64_t>>
parse_times_and_distances(std::ranges::range auto &&lines);
std::uint64_t
num_ways_to_win(std::uint64_t const time, std::uint64_t const distance);

int
main(int argc, char const *const *argv) {
  tests();

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

  std::vector<std::string> lines;
  for (std::string line; std::getline(infile, line);) {
    lines.emplace_back(std::move(line));
  }

  fmt::println("{}", product_of_num_ways_to_win(lines));
  return 0;
}

void
tests() {
  using namespace std::literals::string_view_literals;
  auto lines = std::vector{{
      "Time:      7  15   30"sv,
      "Distance:  9  40  200"sv,
  }};
  ASSERT(product_of_num_ways_to_win(lines) == 288);
}

std::uint64_t
product_of_num_ways_to_win(std::ranges::range auto &&lines) {
  auto [times, distances] = parse_times_and_distances(lines);
  std::uint64_t product{1};
  for (auto const &[time, distance] : std::views::zip(times, distances)) {
    product *= num_ways_to_win(time, distance);
  }
  return product;
}

std::pair<std::vector<std::uint64_t>, std::vector<std::uint64_t>>
parse_times_and_distances(std::ranges::range auto &&lines) {
  std::vector<std::uint64_t> times;
  std::vector<std::uint64_t> distances;
  std::ranges::transform(split(split(lines[0], "Time:")[0], " "),
                         std::back_inserter(times),
                         str_to_int);
  std::ranges::transform(split(split(lines[1], "Distance:")[0], " "),
                         std::back_inserter(distances),
                         str_to_int);
  return {times, distances};
}

std::uint64_t
num_ways_to_win(std::uint64_t const time, std::uint64_t const distance) {
  std::uint64_t num_ways{};
  for (std::uint64_t t{1}; t < time; ++t) {
    if (t * (time - t) > distance) {
      ++num_ways;
    }
  }
  return num_ways;
}
