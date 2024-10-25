#pragma once

// We only really support trl.exe right now ...
// TODO: this should probably support TRL/TR7 or maybe TR8
#define TRL

#ifdef TRL
#include "game/trl.hpp"
#else
#error "Only TRL is supported currently. Define TRL"
#endif

namespace skel::cdc {
#ifdef TRL
  static auto GameState = new TRLState();
#endif
// TODO: extend
}
