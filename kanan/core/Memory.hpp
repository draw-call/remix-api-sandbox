#pragma once

#include <cstdint>

namespace kanan {
bool isGoodPtr(uintptr_t ptr, size_t len, uint32_t access);
bool isGoodReadPtr(uintptr_t ptr, size_t len);
bool isGoodWritePtr(uintptr_t ptr, size_t len);
bool isGoodCodePtr(uintptr_t ptr, size_t len);

template <typename T> constexpr auto
relativeToAbsolute(uintptr_t address) noexcept
{
	return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}
} // namespace kanan
