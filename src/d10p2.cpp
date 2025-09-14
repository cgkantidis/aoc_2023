#include "matrix.hpp"
#include "utility.hpp"

#include <print> // std::println
#include <ranges>
#include <stack> // std::stack
#include <unordered_set> // std::unordered_set

using Map = Matrix<char>;

namespace
{
void
tests();
u64
get_num_tiles_inside_loop(std::vector<std::string> const &lines);
std::pair<Location, Map>
parse_map(std::vector<std::string> const &lines);
std::vector<Location>
get_loop(Location const &start, Map const &map);
bool
is_loop_clockwise(std::vector<Location> const &loop);
bool
advance_path(std::vector<Location> &path, Map const &map);
bool
is_transition_valid(Map const &map, Location const &src, Location const &dst);
void
flood_char(Map &map, char ch);
} // namespace

int
main(int argc, char const **argv) {
  tests();
  std::vector<std::string> lines = read_program_input(argc, argv);
  std::println("{}", get_num_tiles_inside_loop(lines));
  return 0;
}

namespace
{
void
tests() {
  {
    std::vector<std::string> const lines{
        "...........",
        ".S-------7.",
        ".|F-----7|.",
        ".||.....||.",
        ".||.....||.",
        ".|L-7.F-J|.",
        ".|..|.|..|.",
        ".L--J.L--J.",
        "...........",
    };
    ASSERT(get_num_tiles_inside_loop(lines) == 4);
  }
  {
    std::vector<std::string> const lines{
        "..........",
        ".S------7.",
        ".|F----7|.",
        ".||....||.",
        ".||....||.",
        ".|L-7F-J|.",
        ".|..||..|.",
        ".L--JL--J.",
        "..........",
    };
    ASSERT(get_num_tiles_inside_loop(lines) == 4);
  }
  {
    std::vector<std::string> const lines{
        ".F----7F7F7F7F-7....",
        ".|F--7||||||||FJ....",
        ".||.FJ||||||||L7....",
        "FJL7L7LJLJ||LJ.L-7..",
        "L--J.L7...LJS7F-7L7.",
        "....F-J..F7FJ|L7L7L7",
        "....L7.F7||L7|.L7L7|",
        ".....|FJLJ|FJ|F7|.LJ",
        "....FJL-7.||.||||...",
        "....L---J.LJ.LJLJ...",
    };
    ASSERT(get_num_tiles_inside_loop(lines) == 8);
  }
  {
    std::vector<std::string> const lines{
        "FF7FSF7F7F7F7F7F---7",
        "L|LJ||||||||||||F--J",
        "FL-7LJLJ||||||LJL-77",
        "F--JF--7||LJLJ7F7FJ-",
        "L---JF-JLJ.||-FJLJJ7",
        "|F|F-JF---7F7-L7L|7|",
        "|FFJF7L7F-JF7|JL---7",
        "7-L-JL7||F7|L7F-7F7|",
        "L.L7LFJ|||||FJL7||LJ",
        "L7JLJL-JLJLJL--JLJ.L",
    };
    ASSERT(get_num_tiles_inside_loop(lines) == 10);
  }
}

void
print_map(Map const &map) {
  for (std::size_t row = 0; row < map.rows(); ++row) {
    bool all_dot = true;
    for (u64 col = 0; col < map.cols(); ++col) {
      if (map(row, col) != '.') {
        all_dot = false;
        break;
      }
    }
    if (all_dot) {
      continue;
    }
    for (std::size_t col = 0; col < map.cols(); ++col) {
      std::print("{}", map(row, col));
    }
    std::println();
  }
  std::println();
}

bool
mark_horizontal_right(Map &clean_map, std::ranges::viewable_range auto &&win) {
  bool const going_right =
      win[0].col + 1 == win[1].col && win[1].col + 1 == win[2].col;
  if (!going_right) {
    return false;
  }
  u64 const num_rows = clean_map.rows();
  if (win[1].row > 0) {
    auto &tile_up = clean_map(win[1].row - 1, win[1].col);
    if (tile_up == '.') {
      tile_up = 'O';
    }
  }
  if (win[1].row < num_rows - 1) {
    auto &tile_down = clean_map(win[1].row + 1, win[1].col);
    if (tile_down == '.') {
      tile_down = 'I';
    }
  }
  return true;
}

bool
mark_horizontal_left(Map &clean_map, std::ranges::viewable_range auto &&win) {
  bool const going_left =
      win[0].col == win[1].col + 1 && win[1].col == win[2].col + 1;
  if (!going_left) {
    return false;
  }
  u64 const num_rows = clean_map.rows();
  if (win[1].row > 0) {
    auto &tile_up = clean_map(win[1].row - 1, win[1].col);
    if (tile_up == '.') {
      tile_up = 'I';
    }
  }
  if (win[1].row < num_rows - 1) {
    auto &tile_down = clean_map(win[1].row + 1, win[1].col);
    if (tile_down == '.') {
      tile_down = 'O';
    }
  }
  return true;
}

bool
mark_vertical_down(Map &clean_map, std::ranges::viewable_range auto &&win) {
  bool const going_down =
      win[0].row + 1 == win[1].row && win[1].row + 1 == win[2].row;
  if (!going_down) {
    return false;
  }
  u64 const num_cols = clean_map.cols();
  if (win[1].col > 0) {
    auto &tile_left = clean_map(win[1].row, win[1].col - 1);
    if (tile_left == '.') {
      tile_left = 'I';
    }
  }
  if (win[1].col < num_cols - 1) {
    auto &tile_right = clean_map(win[1].row, win[1].col + 1);
    if (tile_right == '.') {
      tile_right = 'O';
    }
  }
  return true;
}

bool
mark_vertical_up(Map &clean_map, std::ranges::viewable_range auto &&win) {
  bool const going_up =
      win[0].row == win[1].row + 1 && win[1].row == win[2].row + 1;
  if (!going_up) {
    return false;
  }
  u64 const num_cols = clean_map.cols();
  if (win[1].col > 0) {
    auto &tile_left = clean_map(win[1].row, win[1].col - 1);
    if (tile_left == '.') {
      tile_left = 'O';
    }
  }
  if (win[1].col < num_cols - 1) {
    auto &tile_right = clean_map(win[1].row, win[1].col + 1);
    if (tile_right == '.') {
      tile_right = 'I';
    }
  }
  return true;
}

bool
mark_south_east(Map &clean_map, std::ranges::viewable_range auto &&win) {
  bool const going_south_to_east =
      win[0].row == win[1].row + 1 && win[1].row == win[2].row
      && win[0].col == win[1].col && win[1].col + 1 == win[2].col;
  bool const going_east_to_south =
      win[0].row == win[1].row && win[1].row + 1 == win[2].row
      && win[0].col == win[1].col + 1 && win[1].col == win[2].col;
  if (!(going_south_to_east || going_east_to_south)) {
    return false;
  }
  char const ch1 = going_south_to_east ? 'O' : 'I';
  char const ch2 = going_south_to_east ? 'I' : 'O';

  if (win[1].row > 0) {
    auto &tile_up = clean_map(win[1].row - 1, win[1].col);
    if (tile_up == '.') {
      tile_up = ch1;
    }
  }
  if (win[1].col > 0) {
    auto &tile_left = clean_map(win[1].row, win[1].col - 1);
    if (tile_left == '.') {
      tile_left = ch1;
    }
  }
  if (win[1].row > 0 && win[1].col > 0) {
    auto &tile_up_left = clean_map(win[1].row - 1, win[1].col - 1);
    if (tile_up_left == '.') {
      tile_up_left = ch1;
    }
  }
  auto &tile_down_right = clean_map(win[1].row + 1, win[1].col + 1);
  if (tile_down_right == '.') {
    tile_down_right = ch2;
  }

  return true;
}

bool
mark_south_west(Map &clean_map, std::ranges::viewable_range auto &&win) {
  bool const going_south_to_west =
      win[0].row == win[1].row + 1 && win[1].row == win[2].row
      && win[0].col == win[1].col && win[1].col == win[2].col + 1;
  bool const going_west_to_south =
      win[0].row == win[1].row && win[1].row + 1 == win[2].row
      && win[0].col + 1 == win[1].col && win[1].col == win[2].col;
  if (!(going_south_to_west || going_west_to_south)) {
    return false;
  }
  char const ch1 = going_west_to_south ? 'O' : 'I';
  char const ch2 = going_west_to_south ? 'I' : 'O';
  u64 const num_cols = clean_map.cols();

  if (win[1].row > 0) {
    auto &tile_up = clean_map(win[1].row - 1, win[1].col);
    if (tile_up == '.') {
      tile_up = ch1;
    }
  }
  if (win[1].col < num_cols - 1) {
    auto &tile_right = clean_map(win[1].row, win[1].col + 1);
    if (tile_right == '.') {
      tile_right = ch1;
    }
  }
  if (win[1].row > 0 && win[1].col < num_cols - 1) {
    auto &tile_up_right = clean_map(win[1].row - 1, win[1].col + 1);
    if (tile_up_right == '.') {
      tile_up_right = ch1;
    }
  }
  auto &tile_down_left = clean_map(win[1].row + 1, win[1].col - 1);
  if (tile_down_left == '.') {
    tile_down_left = ch2;
  }

  return true;
}

u64
get_num_tiles_inside_loop(std::vector<std::string> const &lines) {
  auto const [start, map] = parse_map(lines);
  auto loop_path = get_loop(start, map);
  if (!is_loop_clockwise(loop_path)) {
    loop_path = std::views::reverse(loop_path)
                | std::ranges::to<std::vector<Location>>();
  }

  Map clean_map(map.rows(), map.cols(), '.');
  for (auto const &loc : loop_path) {
    clean_map(loc.row, loc.col) = lines[loc.row][loc.col];
  }

  for (auto const &win : std::views::slide(loop_path, 3)) {
    mark_horizontal_right(clean_map, win) || mark_horizontal_left(clean_map, win)
        || mark_vertical_down(clean_map, win) || mark_vertical_up(clean_map, win)
        || mark_south_east(clean_map, win) || mark_south_west(clean_map, win);
  }
  print_map(clean_map);
  flood_char(clean_map, 'I');
  flood_char(clean_map, 'O');
  print_map(clean_map);

  u64 num_tiles{};
  for (u64 row = 0; row < clean_map.rows(); ++row) {
    for (u64 col = 0; col < clean_map.cols(); ++col) {
      if (clean_map(row, col) == 'I') {
        ++num_tiles;
      }
    }
  }
  return num_tiles;
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
is_loop_clockwise(std::vector<Location> const &loop) {
  // find the topmost tile in the loop
  auto topmost_row = std::numeric_limits<u64>::max();
  long top_row_idx = 0;
  for (auto const &[idx, loc] : std::views::enumerate(loop)) {
    if (loc.row < topmost_row) {
      topmost_row = loc.row;
      top_row_idx = idx;
    }
  }

  auto const &loc0 =
      loop[top_row_idx > 0 ? static_cast<u64>(top_row_idx) - 1 : loop.size() - 2];
  auto const &loc2 = loop[static_cast<u64>(top_row_idx) < loop.size() - 2
                              ? static_cast<u64>(top_row_idx) + 1
                              : 0];

  // it has to be a corner
  return loc2.col > loc0.col;
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

void
flood_char(Map &map, char ch) {
  std::unordered_set<Location> visited;
  std::stack<Location> to_visit;
  for (u64 row = 0; row < map.rows(); ++row) {
    for (u64 col = 0; col < map.cols(); ++col) {
      if (map(row, col) == ch) {
        visited.emplace(row, col);
        to_visit.emplace(row, col);
      }
    }
  }

  auto push_to_stack = [&map, &visited, &to_visit, &ch](Location const &dst) {
    if (!visited.contains(dst) && map(dst) == '.') {
      map(dst) = ch;
      to_visit.emplace(dst);
    }
  };

  while (!to_visit.empty()) {
    Location top = to_visit.top();
    to_visit.pop();

    if (top.row > 0) {
      push_to_stack(Location{.row = top.row - 1, .col = top.col});
    }
    if (top.row < map.rows() - 1) {
      push_to_stack(Location{.row = top.row + 1, .col = top.col});
    }
    if (top.col > 0) {
      push_to_stack(Location{.row = top.row, .col = top.col - 1});
    }
    if (top.col < map.cols() - 1) {
      push_to_stack(Location{.row = top.row, .col = top.col + 1});
    }
  }
}

} // namespace
