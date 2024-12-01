#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstdint> // std::uint8_t
#include <string_view>
#include <vector>

bool
is_digit(char ch);

std::uint8_t
char_to_int(char ch);

std::size_t
str_to_int(std::string_view sv);

std::vector<std::string_view>
split(std::string_view sv, std::string_view delim = " ");

#endif // UTILITY_HPP
