#pragma once

// watbulb's minihook macroset
#define HARGS(...) __VA_ARGS__
#define HOOK(CALL_T, RETURN_T, NAME, ARGS) \
  typedef RETURN_T (CALL_T *t_##NAME##)(HARGS(ARGS)); \
  static t_##NAME o_##NAME = nullptr; \
  static LPVOID tramp_##NAME = nullptr; \
  static RETURN_T CALL_T NAME(HARGS(ARGS))

#define HOOK_FASTCALL(RETURN_T, NAME, ARGS) HOOK(__fastcall, RETURN_T, NAME, ARGS)
#define HOOK_CDECL(RETURN_T, NAME, ARGS)    HOOK(__cdecl,    RETURN_T, NAME, ARGS)
#define HOOK_STDCALL(RETURN_T, NAME, ARGS)  HOOK(__stdcall,  RETURN_T, NAME, ARGS)
