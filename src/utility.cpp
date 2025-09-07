#include "utility.hpp"
#include <fstream> // std::ifstream
#include <print> // std::println
#include <vector> // std::vector

bool
is_digit(char ch) {
  return ch >= '0' && ch <= '9';
}

uint8_t
char_to_int(char ch) {
  return static_cast<uint8_t>(ch - '0');
}

std::vector<std::string_view>
split(std::string_view sv, std::string_view delim) {
  std::vector<std::string_view> tokens;
  for (std::size_t right = sv.find(delim); right != std::string_view::npos;
       right = sv.find(delim)) {
    if (right == 0) {
      sv = sv.substr(right + delim.size());
      continue;
    }
    tokens.emplace_back(sv.substr(0, right));
    sv = sv.substr(right + delim.size());
  }
  if (!sv.empty()) {
    tokens.emplace_back(sv);
  }
  return tokens;
}

std::uint8_t
get_num_digits(u64 num) {
  if (num == 0) {
    return 1;
  }
  std::uint8_t num_digits{};
  while (num != 0) {
    ++num_digits;
    num /= 10;
  }
  return num_digits;
}

u64
pow10(std::uint8_t exp) {
  u64 val{1};
  for (std::uint8_t i{0}; i < exp; ++i) {
    val *= 10;
  }
  return val;
}

bool
operator==(Location const &lhs, Location const &rhs) {
  return lhs.row == rhs.row && lhs.col == rhs.col;
}

std::size_t
std::hash<Location>::operator()(Location const &loc) const noexcept {
  std::size_t h1 = std::hash<std::size_t>{}(loc.row);
  std::size_t h2 = std::hash<std::size_t>{}(loc.col);

  std::size_t ret_val = 0;
  hash_combine(ret_val, h1, h2);
  return ret_val;
}

std::vector<std::string>
read_program_input(int argc, char const * const *argv) {
  auto args = std::span(argv, size_t(argc));
  if (args.size() != 2) {
    std::println(stderr, "usage: {} input.txt", args[0]);
    return {};
  }

  std::ifstream infile(args[1]);
  if (!infile.is_open()) {
    std::println(stderr, "couldn't open file {}", args[1]);
    return {};
  }

  std::vector<std::string> lines;
  for (std::string line; std::getline(infile, line);) {
    lines.emplace_back(std::move(line));
  }
  return lines;
}
