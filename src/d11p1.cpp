#include "matrix.hpp" // Matrix
#include "utility.hpp"

#include <print> // std::println
#include <vector> // std::vector

using Map = Matrix<char>;

namespace
{
void
tests();
u64
get_sum_of_shortest_path_lengths(std::vector<std::string> const &lines);
Map
parse_map(std::vector<std::string> const &lines);
Map
get_augmented_map(Map const &map);
std::vector<u64>
get_empty_rows(Map const &map);
std::vector<u64>
get_empty_cols(Map const &map);
std::vector<Location>
get_galaxy_locs(Map const &aug_map);
void
print_map(Map const &map);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_sum_of_shortest_path_lengths(lines));
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
  std::vector<std::string> const aug_lines{
      "....#........",
      ".........#...",
      "#............",
      ".............",
      ".............",
      "........#....",
      ".#...........",
      "............#",
      ".............",
      ".............",
      ".........#...",
      "#....#.......",
  };
  Map map = parse_map(lines);
  Map aug_map_ref = parse_map(aug_lines);
  Map aug_map = get_augmented_map(map);
  if (aug_map != aug_map_ref) {
    print_map(aug_map_ref);
    print_map(aug_map);
  }
  ASSERT(aug_map == aug_map_ref);
  ASSERT(get_sum_of_shortest_path_lengths(lines) == 374);
}

u64
get_sum_of_shortest_path_lengths(std::vector<std::string> const &lines) {
  Map map = parse_map(lines);
  Map aug_map = get_augmented_map(map);
  std::vector<Location> galaxy_locs = get_galaxy_locs(aug_map);
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

Map
get_augmented_map(Map const &map) {
  auto empty_rows = get_empty_rows(map);
  auto empty_cols = get_empty_cols(map);
  Map aug_map(map.rows() + empty_rows.size(),
              map.cols() + empty_cols.size(),
              '.');

  u64 aug_row = 0;
  auto row_it = empty_rows.begin();
  for (u64 row = 0; row < map.rows(); ++row) {
    if (row_it != empty_rows.end() && row == *row_it) {
      ++aug_row;
      ++row_it;
    }

    u64 aug_col = 0;
    auto col_it = empty_cols.begin();
    for (u64 col = 0; col < map.cols(); ++col) {
      if (col_it != empty_cols.end() && col == *col_it) {
        ++aug_col;
        ++col_it;
      }
      aug_map(aug_row, aug_col) = map(row, col);
      ++aug_col;
    }
    ++aug_row;
  }
  return aug_map;
}

std::vector<u64>
get_empty_rows(Map const &map) {
  std::vector<u64> empty_rows;
  for (u64 row = 0; row < map.rows(); ++row) {
    bool is_empty = true;
    for (u64 col = 0; col < map.cols(); ++col) {
      if (map(row, col) != '.') {
        is_empty = false;
        break;
      }
    }
    if (is_empty) {
      empty_rows.emplace_back(row);
    }
  }
  return empty_rows;
}

std::vector<u64>
get_empty_cols(Map const &map) {
  std::vector<u64> empty_cols;
  for (u64 col = 0; col < map.cols(); ++col) {
    bool is_empty = true;
    for (u64 row = 0; row < map.rows(); ++row) {
      if (map(row, col) != '.') {
        is_empty = false;
        break;
      }
    }
    if (is_empty) {
      empty_cols.emplace_back(col);
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

void
print_map(Map const &map) {
  for (u64 row = 0; row < map.rows(); ++row) {
    for (u64 col = 0; col < map.cols(); ++col) {
      std::print("{}", map(row, col));
    }
    std::println();
  }
  std::println();
}
} // namespace
