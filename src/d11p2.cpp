#include "matrix.hpp" // Matrix
#include "utility.hpp"

#include <print> // std::println
#include <unordered_set> // std::unordered_set
#include <vector> // std::vector

using Map = Matrix<char>;

namespace
{
void
tests();
u64
get_sum_of_shortest_path_lengths(std::vector<std::string> const &lines,
                                 u64 expansion);
Map
parse_map(std::vector<std::string> const &lines);
std::unordered_set<u64>
get_empty_rows(Map const &map);
std::unordered_set<u64>
get_empty_cols(Map const &map);
std::vector<Location>
get_galaxy_locs(Map const &aug_map);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_sum_of_shortest_path_lengths(lines, 1'000'000));
  return 0;
}

namespace
{
void
tests() {
  std::vector<std::string> const lines{
      "...#......",
      ".......#..",
      "#.........",
      "..........",
      "......#...",
      ".#........",
      ".........#",
      "..........",
      ".......#..",
      "#...#.....",
  };
  Map map = parse_map(lines);
  ASSERT(get_sum_of_shortest_path_lengths(lines, 10) == 1030);
  ASSERT(get_sum_of_shortest_path_lengths(lines, 100) == 8410);
}

u64
get_sum_of_shortest_path_lengths(std::vector<std::string> const &lines, u64 expansion) {
  Map map = parse_map(lines);
  auto empty_rows = get_empty_rows(map);
  auto empty_cols = get_empty_cols(map);

  std::vector<Location> galaxy_locs = get_galaxy_locs(map);
  u64 sum = 0;
  for (u64 i = 0; i < galaxy_locs.size() - 1; ++i) {
    for (u64 j = i + 1; j < galaxy_locs.size(); ++j) {
      u64 row1 = galaxy_locs[i].row;
      u64 row2 = galaxy_locs[j].row;
      if (row1 > row2) {
        std::swap(row1, row2);
      }
      u64 col1 = galaxy_locs[i].col;
      u64 col2 = galaxy_locs[j].col;
      if (col1 > col2) {
        std::swap(col1, col2);
      }

      sum += row2 - row1 + col2 - col1;
      for (u64 row = row1 + 1; row < row2; ++row) {
        if (empty_rows.contains(row)) {
          sum += expansion - 1;
        }
      }
      for (u64 col = col1 + 1; col < col2; ++col) {
        if (empty_cols.contains(col)) {
          sum += expansion - 1;
        }
      }
    }
  }

  return sum;
}

Map
parse_map(std::vector<std::string> const &lines) {
  u64 rows = lines.size();
  u64 cols = lines[0].size();

  Map map(rows, cols);
  for (u64 row = 0; row < rows; ++row) {
    for (u64 col = 0; col < cols; ++col) {
      map(row, col) = lines[row][col];
    }
  }

  return map;
}

std::unordered_set<u64>
get_empty_rows(Map const &map) {
  std::unordered_set<u64> empty_rows;
  for (u64 row = 0; row < map.rows(); ++row) {
    bool is_empty = true;
    for (u64 col = 0; col < map.cols(); ++col) {
      if (map(row, col) != '.') {
        is_empty = false;
        break;
      }
    }
    if (is_empty) {
      empty_rows.insert(row);
    }
  }
  return empty_rows;
}

std::unordered_set<u64>
get_empty_cols(Map const &map) {
  std::unordered_set<u64> empty_cols;
  for (u64 col = 0; col < map.cols(); ++col) {
    bool is_empty = true;
    for (u64 row = 0; row < map.rows(); ++row) {
      if (map(row, col) != '.') {
        is_empty = false;
        break;
      }
    }
    if (is_empty) {
      empty_cols.insert(col);
    }
  }
  return empty_cols;
}

std::vector<Location>
get_galaxy_locs(Map const &aug_map) {
  std::vector<Location> galaxy_locs;
  for (u64 row = 0; row < aug_map.rows(); ++row) {
    for (u64 col = 0; col < aug_map.cols(); ++col) {
      if (aug_map(row, col) == '#') {
        galaxy_locs.emplace_back(row, col);
      }
    }
  }
  return galaxy_locs;
}
} // namespace
