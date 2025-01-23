#include <algorithm>
#include <fmt/core.h>
#include <fstream>
#include <string>
#include <vector>

#include "utility.hpp"

bool
is_symbol(char ch) {
  return ch != '.' && !is_digit(ch);
}

struct Part
{
  std::size_t m_part_num;
};

bool
is_part(std::vector<std::string> const &lines,
        std::size_t row,
        std::size_t col,
        std::size_t length) {
  std::string_view line{lines[row]};
  // check if previous is a symbol
  if (col > 0 && is_symbol(line[col - 1])) {
    return true;
  }
  // check if next is a symbol
  if (col + length < line.length() && is_symbol(line[col + length])) {
    return true;
  }

  std::size_t other_col = col;
  std::size_t other_length = length;
  if (other_col > 0) {
    --other_col;
    ++other_length;
  }
  if (other_col + other_length - 1 < line.length()) {
    ++other_length;
  }
  // check if previous line contains a symbol
  if (row > 0) {
    if (std::ranges::any_of(lines[row - 1].substr(other_col, other_length),
                            is_symbol)) {
      return true;
    }
  }
  // check if next line contains a symbol
  if (row < lines.size() - 1) {
    if (std::ranges::any_of(lines[row + 1].substr(other_col, other_length),
                            is_symbol)) {
      return true;
    }
  }

  return false;
}

std::vector<std::vector<Part>>
get_parts(std::vector<std::string> const &lines) {
  std::vector<std::vector<Part>> parts(lines.size());
  std::size_t const line_length{lines.front().size()};
  for (std::size_t row = 0; row < lines.size(); ++row) {
    std::string_view line{lines[row]};
    for (std::size_t col = 0; col < line_length; ++col) {
      std::size_t left_col{col};
      if (!is_digit(line[left_col])) {
        continue;
      }
      std::size_t length = 1;
      while (is_digit(line[++col])) {
        ++length;
      }
      if (is_part(lines, row, left_col, length)) {
        parts[row].emplace_back(str_to_int<std::size_t>(line.substr(left_col, length)));
      }
    }
  }
  return parts;
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

  // read all lines, since we need to look around
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(infile, line)) {
    lines.emplace_back(std::move(line));
  }

  std::vector<std::vector<Part>> parts = get_parts(lines);
  std::size_t total = std::ranges::fold_left(
      parts,
      0ULL,
      [](std::size_t const &sum, std::vector<Part> const &sub_parts) {
        return sum
               + std::ranges::fold_left(
                   sub_parts,
                   0ULL,
                   [](std::size_t const &prev_sum, Part const &part) {
                     return prev_sum + part.m_part_num;
                   });
      });
  fmt::println("{}", total);
  return 0;
}

