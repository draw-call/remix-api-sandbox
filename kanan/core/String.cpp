#include <memory>
#include <string>
#include <Windows.h>

#include "String.hpp"

using namespace std;

namespace kanan {
string narrow(wstring_view str) {
    auto length = WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.length(), nullptr, 0, nullptr, nullptr);
    string narrowStr{};

    narrowStr.resize(length);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), (int)str.length(), (LPSTR)narrowStr.c_str(), length, nullptr, nullptr);
    return narrowStr;
}

wstring widen(string_view str) {
    auto length = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.length(), nullptr, 0);
    wstring wideStr{};

    wideStr.resize(length);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.length(), (LPWSTR)wideStr.c_str(), length);
    return wideStr;
}

vector<string> split(string str, const string& delim) {
    vector<string> pieces{};
    size_t last{};
    size_t next{};

    while ((next = str.find(delim, last)) != string::npos) {
        pieces.emplace_back(str.substr(last, next - last));
        last = next + delim.length();
    }

    return pieces;
}
} // namespace kanan
