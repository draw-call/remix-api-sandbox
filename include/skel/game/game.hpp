#pragma once

/* We only really support trl.exe right now ...
 * TODO: this should probably support TRL/TR7 or maybe TR8
 */

#ifdef TRL
  #define USE_PCSTATEMANAGER
  #define StateImpl TRLState
  #include "skel/game/trl.hpp"
#else
  #error "Only TRL is supported currently. Define TRL"
#endif

namespace skel::cdc {

  static const auto GameState = new StateImpl();
  static const auto GameStateWndProcCallback = &StateImpl::WndProcCallback;

}
