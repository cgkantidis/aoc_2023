#include "utility.hpp"

#include <algorithm> // std::ranges::fold_left
#include <print> // std::println
#include <ranges> // std::views::enumerate

struct Part
{
  std::uint64_t m_part_num;
  std::size_t m_row;
  std::size_t m_col;
  std::size_t m_len;
};

struct Gear
{
  std::uint64_t m_ratio;
  std::size_t m_row;
  std::size_t m_col;
};

namespace
{
void
tests();
std::uint64_t
get_sum_of_gear_ratios(std::ranges::range auto &&lines);
std::vector<Part>
get_parts(std::ranges::range auto &&lines);
bool
is_part(Part const &part, std::ranges::range auto &&lines);
bool
is_symbol(char ch);
std::vector<Gear>
get_gears(std::vector<Part> const &parts, std::ranges::range auto &&lines);
bool
is_gear(Gear &gear, std::vector<Part> const &parts);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_sum_of_gear_ratios(lines));
  return 0;
}

namespace
{
void
tests() {
  using namespace std::literals::string_view_literals;
  auto const lines = std::array{
      "467..114.."sv,
      "...*......"sv,
      "..35..633."sv,
      "......#..."sv,
      "617*......"sv,
      ".....+.58."sv,
      "..592....."sv,
      "......755."sv,
      "...$.*...."sv,
      ".664.598.."sv,
  };
  ASSERT(get_sum_of_gear_ratios(lines) == 467835);
}

std::uint64_t
get_sum_of_gear_ratios(std::ranges::range auto &&lines) {
  std::vector<Part> parts = get_parts(lines);
  std::vector<Gear> gears = get_gears(parts, lines);
  std::size_t total =
      std::ranges::fold_left(gears,
                             0ULL,
                             [](std::size_t const &sum, Gear const &gear) {
                               return sum + gear.m_ratio;
                             });
  return total;
}

std::vector<Part>
get_parts(std::ranges::range auto &&lines) {
  std::vector<Part> parts;
  std::size_t const line_length{lines.front().size()};

  for (auto const &[row, line] : std::views::enumerate(lines)) {
    for (std::size_t col = 0; col < line_length; ++col) {
      std::size_t length = 0;
      while (col < line_length && is_digit(line[col])) {
        ++col;
        ++length;
      }
      if (length != 0) {
        std::size_t l_col = col - length;
        Part part{str_to_int<std::uint64_t>(line.substr(l_col, length)),
                  static_cast<std::size_t>(row),
                  l_col,
                  length};
        if (is_part(part, lines)) {
          parts.emplace_back(part);
        }
      }
    }
  }
  return parts;
}

bool
is_part(Part const &part, std::ranges::range auto &&lines) {
  std::size_t const &row = part.m_row;
  std::size_t const &col = part.m_col;
  std::size_t const &len = part.m_len;

  std::string_view line{lines[row]};
  // check if previous is a symbol
  if (col > 0 && is_symbol(line[col - 1])) {
    return true;
  }

  // check if next is a symbol
  if (col + len < line.length() && is_symbol(line[col + len])) {
    return true;
  }

  // compute the bounding box's left column and length
  std::size_t bb_col = col;
  std::size_t bb_len = len;
  if (bb_col > 0) {
    --bb_col;
    ++bb_len;
  }
  if (bb_col + bb_len - 1 < line.length()) {
    ++bb_len;
  }
  // check if previous line contains a symbol
  if (row > 0) {
    if (std::ranges::any_of(lines[row - 1].substr(bb_col, bb_len), is_symbol)) {
      return true;
    }
  }
  // check if next line contains a symbol
  if (row < lines.size() - 1) {
    if (std::ranges::any_of(lines[row + 1].substr(bb_col, bb_len), is_symbol)) {
      return true;
    }
  }

  return false;
}

bool
is_symbol(char ch) {
  return ch != '.' && !is_digit(ch);
}

std::vector<Gear>
get_gears(std::vector<Part> const &parts, std::ranges::range auto &&lines) {
  std::vector<Gear> gears;
  for (auto const &[row, line] : std::views::enumerate(lines)) {
    for (auto const &[col, ch] : std::views::enumerate(line)) {
      if (ch != '*') {
        continue;
      }

      Gear gear{.m_ratio = 0,
                .m_row = static_cast<std::size_t>(row),
                .m_col = static_cast<std::size_t>(col)};
      if (is_gear(gear, parts)) {
        gears.emplace_back(gear);
      }
    }
  }
  return gears;
}

bool
is_gear(Gear &gear, std::vector<Part> const &parts) {
  std::uint64_t count{};
  gear.m_ratio = 1;
  for (Part const &part : std::views::filter(parts, [&gear](Part const &part) {
         return part.m_row >= gear.m_row - 1 && part.m_row <= gear.m_row + 1
                && part.m_col + part.m_len >= gear.m_col
                && part.m_col <= gear.m_col + 1;
       })) {
    ++count;
    if (count > 2) {
      return false;
    }
    gear.m_ratio *= part.m_part_num;
  }
  return count == 2;
}

} // namespace
