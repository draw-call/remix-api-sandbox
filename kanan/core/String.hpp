#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace kanan {
//
// String utilities.
//

// Conversion functions for UTF8<->UTF16.
std::string  narrow(std::wstring_view str);
std::wstring  widen(std::string_view  str);

std::vector<std::string> split(std::string str, const std::string& delim);

} // namespace kanan
