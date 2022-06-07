// PWH. 2022.06.07

#pragma once

#include <string>

std::wstring ToWString(std::string_view sv);
std::wstring ToWString(std::string const& str);
std::wstring ToWString(char const* sz);
