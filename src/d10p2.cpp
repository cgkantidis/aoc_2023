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
get_num_tiles_inside_loop(std::vector<std::string> const &lines);
std::pair<Location, Map>
parse_map(std::vector<std::string> const &lines);
std::vector<Location>
get_loop(Location const &start, Map const &map);
bool
advance_path(std::vector<Location> &path, Map const &map);
bool
is_transition_valid(Map const &map, Location const &src, Location const &dst);
void
update_map_start(Map &map, std::vector<Location> const &loop);
Dir
get_direction(Location const &src, Location const &dst);
void
mark_by_ray_tracing(Map &map);
void
print_map(Map const &map);
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

u64
get_num_tiles_inside_loop(std::vector<std::string> const &lines) {
  auto const [start, map] = parse_map(lines);
  auto loop_path = get_loop(start, map);

  Map clean_map(map.rows(), map.cols(), '.');
  for (auto const &loc : loop_path) {
    clean_map(loc.row, loc.col) = lines[loc.row][loc.col];
  }
  update_map_start(clean_map, loop_path);

  mark_by_ray_tracing(clean_map);
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

void
update_map_start(Map &map, std::vector<Location> const &loop) {
  Location const &prev = loop[loop.size() - 2];
  Location const &strt = loop[0];
  Location const &next = loop[1];
  std::pair<Dir, Dir> dirs{get_direction(prev, strt), get_direction(strt, next)};
  if (dirs == std::pair{Dir::UP, Dir::RIGHT}
      || dirs == std::pair{Dir::LEFT, Dir::DOWN}) {
    map(strt.row, strt.col) = 'F';
    return;
  }
  if (dirs == std::pair{Dir::UP, Dir::LEFT}
      || dirs == std::pair{Dir::RIGHT, Dir::DOWN}) {
    map(strt.row, strt.col) = '7';
    return;
  }
  if (dirs == std::pair{Dir::DOWN, Dir::LEFT}
      || dirs == std::pair{Dir::RIGHT, Dir::UP}) {
    map(strt.row, strt.col) = 'J';
    return;
  }
  if (dirs == std::pair{Dir::DOWN, Dir::RIGHT}
      || dirs == std::pair{Dir::LEFT, Dir::UP}) {
    map(strt.row, strt.col) = 'L';
    return;
  }
  UNREACHABLE();
}

Dir
get_direction(Location const &src, Location const &dst) {
  if (src.row == dst.row + 1) {
    return Dir::UP;
  }
  if (src.row + 1 == dst.row) {
    return Dir::DOWN;
  }
  if (src.col == dst.col + 1) {
    return Dir::LEFT;
  }
  if (src.col + 1 == dst.col) {
    return Dir::RIGHT;
  }
  UNREACHABLE();
}

void
mark_by_ray_tracing(Map &map) {
  for (u64 row = 0; row < map.rows(); ++row) {
    bool is_inside = false;
    bool down_detected = false;
    bool up_detected = false;
    for (u64 col = 0; col < map.cols(); ++col) {
      if (map(row, col) == 'F' || map(row, col) == '7') {
        if (up_detected) {
          is_inside = !is_inside;
          down_detected = false;
          up_detected = false;
        } else if (down_detected) {
          down_detected = false;
        } else {
          down_detected = true;
        }
      } else if (map(row, col) == 'J' || map(row, col) == 'L') {
        if (down_detected) {
          is_inside = !is_inside;
          down_detected = false;
          up_detected = false;
        } else if (up_detected) {
          up_detected = false;
        } else {
          up_detected = true;
        }
      } else if (map(row, col) == '.') {
        map(row, col) = is_inside ? 'I' : 'O';
      } else if (map(row, col) == '|') {
        is_inside = !is_inside;
      }
    }
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

} // namespace
