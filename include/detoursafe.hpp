#pragma once
#include <type_traits>

#include "detours.h"

template<typename T> concept function_pointer = std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>;

template<function_pointer T>
long DetourAttachSafe(T &function, std::type_identity_t<T> detour) noexcept
{
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  LONG r;
  if ((r = DetourAttach(reinterpret_cast<void **>(&function), reinterpret_cast<void *>(detour))) != NO_ERROR) {
    return r;
  }
  return DetourTransactionCommit();
}

template<function_pointer T>
long DetourDetachSafe(T &function, std::type_identity_t<T> detour) noexcept
{
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  LONG r;
  if ((r = DetourDetach(reinterpret_cast<void **>(&function), reinterpret_cast<void *>(detour))) != NO_ERROR) {
    return r;
  }
  return DetourTransactionCommit();
}
