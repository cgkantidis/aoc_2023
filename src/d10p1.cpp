#include "matrix.hpp"
#include "utility.hpp"

#include <print> // std::println
#include <ranges>
#include <unordered_set> // std::unordered_set

namespace
{
void
tests();
u64
get_num_steps_to_farthest_pipe(std::vector<std::string> const &lines);
std::pair<Location, Matrix<std::unordered_set<Dir>>>
parse_map(std::vector<std::string> const &lines);
bool
advance_path(std::vector<Location> &path,
             Matrix<std::unordered_set<Dir>> const &map);
bool
is_transition_valid(Matrix<std::unordered_set<Dir>> const &map,
                    Location const &src,
                    Location const &dst);
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
  auto row = start.row;
  auto col = start.col;

  std::vector<std::vector<Location>> paths;
  for (Dir dir : ALL_DIRS) {
    std::vector<Location> path{start};
    switch (dir) {
    case Dir::UP: {
      if (row > 0) {
        Location dst{.row = row - 1, .col = col};
        if (is_transition_valid(map, start, dst)) {
          path.emplace_back(dst);
          paths.emplace_back(path);
        }
      }
      break;
    }
    case Dir::DOWN: {
      if (row < map.rows() - 1) {
        Location dst{.row = row + 1, .col = col};
        if (is_transition_valid(map, start, dst)) {
          path.emplace_back(dst);
          paths.emplace_back(path);
        }
      }
      break;
    }
    case Dir::LEFT: {
      if (col > 0) {
        Location dst{.row = row, .col = col - 1};
        if (is_transition_valid(map, start, dst)) {
          path.emplace_back(dst);
          paths.emplace_back(path);
        }
      }
      break;
    }
    case Dir::RIGHT: {
      if (col < map.cols() - 1) {
        Location dst{.row = row, .col = col + 1};
        if (is_transition_valid(map, start, dst)) {
          path.emplace_back(dst);
          paths.emplace_back(path);
        }
      }
      break;
    }
    }
  }
  for (auto &path : paths) {
    while (advance_path(path, map) && path.back() != start) {}
    if (path.back() == start) {
      return (path.size() - 1) / 2;
    }
  }
  UNREACHABLE();
}

std::pair<Location, Matrix<std::unordered_set<Dir>>>
parse_map(std::vector<std::string> const &lines) {
  std::size_t num_rows = lines.size();
  std::size_t num_cols = lines[0].size();
  Matrix<std::unordered_set<Dir>> map(num_rows, num_cols);
  Location start{};

  for (auto const &[row_, line] : std::views::enumerate(lines)) {
    for (auto const &[col_, ch] : std::views::enumerate(line)) {
      auto row = static_cast<u64>(row_);
      auto col = static_cast<u64>(col_);

      switch (ch) {
      case '|': {
        map(row, col) = {Dir::UP, Dir::DOWN};
        break;
      }
      case '-': {
        map(row, col) = {Dir::LEFT, Dir::RIGHT};
        break;
      }
      case 'L': {
        map(row, col) = {Dir::UP, Dir::RIGHT};
        break;
      }
      case 'J': {
        map(row, col) = {Dir::UP, Dir::LEFT};
        break;
      }
      case '7': {
        map(row, col) = {Dir::DOWN, Dir::LEFT};
        break;
      }
      case 'F': {
        map(row, col) = {Dir::DOWN, Dir::RIGHT};
        break;
      }
      case '.': {
        break;
      }
      case 'S': {
        start = {.row = row, .col = col};
        map(row, col) = ALL_DIRS;
        break;
      }
      default: {
        UNREACHABLE();
      }
      }
    }
  }

  return {start, map};
}

bool
advance_path(std::vector<Location> &path,
             Matrix<std::unordered_set<Dir>> const &map) {
  auto const src = path.back();
  auto const &prev = path[path.size() - 2];
  auto const row = src.row;
  auto const col = src.col;
  for (Dir dir : ALL_DIRS) {
    switch (dir) {
    case Dir::UP: {
      if (row > 0) {
        Location dst{.row = row - 1, .col = col};
        if (dst != prev && is_transition_valid(map, src, dst)) {
          path.emplace_back(dst);
          return true;
        }
      }
      break;
    }
    case Dir::DOWN: {
      if (row < map.rows() - 1) {
        Location dst{.row = row + 1, .col = col};
        if (dst != prev && is_transition_valid(map, src, dst)) {
          path.emplace_back(dst);
          return true;
        }
      }
      break;
    }
    case Dir::LEFT: {
      if (col > 0) {
        Location dst{.row = row, .col = col - 1};
        if (dst != prev && is_transition_valid(map, src, dst)) {
          path.emplace_back(dst);
          return true;
        }
      }
      break;
    }
    case Dir::RIGHT: {
      if (col < map.cols() - 1) {
        Location dst{.row = row, .col = col + 1};
        if (dst != prev && is_transition_valid(map, src, dst)) {
          path.emplace_back(dst);
          return true;
        }
      }
      break;
    }
    }
  }
  return false;
}

bool
is_transition_valid(Matrix<std::unordered_set<Dir>> const &map,
                    Location const &src,
                    Location const &dst) {
  if (src.row == dst.row) {
    if (src.col + 1 == dst.col) {
      return map(src).contains(Dir::RIGHT) && map(dst).contains(Dir::LEFT);
    }
    return map(src).contains(Dir::LEFT) && map(dst).contains(Dir::RIGHT);
  }
  if (src.row + 1 == dst.row) {
    return map(src).contains(Dir::DOWN) && map(dst).contains(Dir::UP);
  }
  return map(src).contains(Dir::UP) && map(dst).contains(Dir::DOWN);
}

} // namespace
