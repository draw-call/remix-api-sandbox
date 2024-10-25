#include "Scan.hpp"
#include "Module.hpp"
#include "Pattern.hpp"
#include "String.hpp"

namespace kanan {
std::optional<uintptr_t> scan(const std::string &module, const std::string &pattern) {
  return scan(GetModuleHandle(module.c_str()), pattern);
}

std::optional<uintptr_t> scan(const std::string &module, uintptr_t start, const std::string &pattern) {
  HMODULE mod = GetModuleHandle(module.c_str());
  return scan(start, (getModuleSize(mod).value_or(0) - start + (uintptr_t)mod), pattern);
}

std::optional<uintptr_t> scan(HMODULE module, const std::string &pattern) {
  return scan((uintptr_t)module, getModuleSize(module).value_or(0), pattern);
}

std::optional<uintptr_t> scan(uintptr_t start, size_t length, const std::string &pattern) {
  if (start == 0 || length == 0) {
    return {};
  }
  Pattern p{pattern};
  return p.find(start, length);
}

std::optional<uintptr_t> scan(const std::string &pattern) {
  auto mod  = GetModuleHandle(nullptr);
  auto size = getModuleSize(mod).value_or(0);
  return scan((uintptr_t)mod, size, pattern);
}
} // namespace kanan
