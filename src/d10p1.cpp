#include "matrix.hpp"
#include "utility.hpp"

#include <print> // std::println
#include <ranges>
#include <unordered_set> // std::unordered_set

using Map = Matrix<char>;

namespace
{
void
tests();
u64
get_num_steps_to_farthest_pipe(std::vector<std::string> const &lines);
std::vector<Location>
get_loop(Location const &start, Map const &map);
std::pair<Location, Map>
parse_map(std::vector<std::string> const &lines);
bool
advance_path(std::vector<Location> &path, Map const &map);
bool
is_transition_valid(Map const &map, Location const &src, Location const &dst);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_num_steps_to_farthest_pipe(lines));
  return 0;
}

namespace
{
void
tests() {
  {
    std::vector<std::string> const lines{
        ".....",
        ".S-7.",
        ".|.|.",
        ".L-J.",
        ".....",
    };
    ASSERT(get_num_steps_to_farthest_pipe(lines) == 4);
  }
  {
    std::vector<std::string> const lines{
        "-L|F7",
        "7S-7|",
        "L|7||",
        "-L-J|",
        "L|-JF",
    };
    ASSERT(get_num_steps_to_farthest_pipe(lines) == 4);
  }
  {
    std::vector<std::string> const lines{
        "..F7.",
        ".FJ|.",
        "SJ.L7",
        "|F--J",
        "LJ...",
    };
    ASSERT(get_num_steps_to_farthest_pipe(lines) == 8);
  }
  {
    std::vector<std::string> const lines{
        "7-F7-",
        ".FJ|7",
        "SJLL7",
        "|F--J",
        "LJ.LJ",
    };
    ASSERT(get_num_steps_to_farthest_pipe(lines) == 8);
  }
}

u64
get_num_steps_to_farthest_pipe(std::vector<std::string> const &lines) {
  auto const [start, map] = parse_map(lines);
  auto loop_path = get_loop(start, map);
  return loop_path.size() / 2;
}

std::pair<Location, Map>
parse_map(std::vector<std::string> const &lines) {
  std::size_t num_rows = lines.size();
  std::size_t num_cols = lines[0].size();
  Map map(num_rows, num_cols);
  Location start{};

  for (auto const &[row_, line] : std::views::enumerate(lines)) {
    for (auto const &[col_, ch] : std::views::enumerate(line)) {
      auto row = static_cast<u64>(row_);
      auto col = static_cast<u64>(col_);

      if (ch == 'S') {
        start = {.row = row, .col = col};
      }
      map(row, col) = ch;
    }
  }

  return {start, map};
}

std::vector<Location>
get_loop(Location const &start, Map const &map) {
  auto row = start.row;
  auto col = start.col;

  std::vector<std::vector<Location>> paths;
  auto add_to_paths = [&map, &start, &paths](Location const &dst) {
    if (is_transition_valid(map, start, dst)) {
      paths.emplace_back(std::vector{start, dst});
    }
  };
  if (row > 0) {
    add_to_paths(Location{.row = row - 1, .col = col});
  }
  if (row < map.rows() - 1) {
    add_to_paths(Location{.row = row + 1, .col = col});
  }
  if (col > 0) {
    add_to_paths(Location{.row = row, .col = col - 1});
  }
  if (col < map.cols() - 1) {
    add_to_paths(Location{.row = row, .col = col + 1});
  }
  for (auto &path : paths) {
    while (advance_path(path, map) && path.back() != start) {}
    if (path.back() == start) {
      return path;
    }
  }
  UNREACHABLE();
}

bool
advance_path(std::vector<Location> &path, Map const &map) {
  auto const src = path.back();
  auto const prv = path[path.size() - 2];
  auto const row = src.row;
  auto const col = src.col;
  auto advance_path_int = [&map, &src, &prv, &path](Location const &dst) -> bool {
    if (dst != prv && is_transition_valid(map, src, dst)) {
      path.emplace_back(dst);
      return true;
    }
    return false;
  };
  if (row > 0 && advance_path_int(Location{.row = row - 1, .col = col})) {
    return true;
  }
  if (row < map.rows() - 1
      && advance_path_int(Location{.row = row + 1, .col = col})) {
    return true;
  }
  if (col > 0 && advance_path_int(Location{.row = row, .col = col - 1})) {
    return true;
  }
  if (col < map.cols() - 1
      && advance_path_int(Location{.row = row, .col = col + 1})) {
    return true;
  }
  return false;
}

bool
is_transition_valid(Map const &map, Location const &src, Location const &dst) {
  static const std::unordered_set<char> right_chars{'S', '-', 'F', 'L'};
  static const std::unordered_set<char> left_chars{'S', '-', '7', 'J'};
  static const std::unordered_set<char> up_chars{'S', '|', 'L', 'J'};
  static const std::unordered_set<char> down_chars{'S', '|', '7', 'F'};

  char const &src_tile = map(src);
  char const &dst_tile = map(dst);
  if (src.row == dst.row) {
    if (src.col + 1 == dst.col) {
      return right_chars.contains(src_tile) && left_chars.contains(dst_tile);
    }
    return left_chars.contains(src_tile) && right_chars.contains(dst_tile);
  }
  if (src.row + 1 == dst.row) {
    return down_chars.contains(src_tile) && up_chars.contains(dst_tile);
  }
  return up_chars.contains(src_tile) && down_chars.contains(dst_tile);
}
} // namespace
