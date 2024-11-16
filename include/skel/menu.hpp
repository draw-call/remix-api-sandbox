#pragma once

#include <stdint.h>
#include <vector>
#include <utility>

#include <d3dx9.h>

#include "skel/game/base.hpp"

namespace skel::menu {

  void RenderDiagView(cdc::GameStateBase *GameState); 
  void RenderMatrixDiagWindow(cdc::GameStateBase *GameState);

} // namespace skel::menu
