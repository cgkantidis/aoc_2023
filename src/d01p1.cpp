#include <fmt/core.h>
#include <fstream>
#include <string>

#include "utility.hpp"

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

  std::uint64_t total{};
  std::string line;
  while (std::getline(infile, line)) {
    auto first = std::find_if(line.begin(), line.end(), is_digit);
    auto last = std::find_if(line.rbegin(), line.rend(), is_digit);
    total += char_to_int(*first) * 10 + char_to_int(*last);
  }
  fmt::println("{}", total);
  return 0;
}
