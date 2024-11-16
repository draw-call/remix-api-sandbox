
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

#include "skel/menu.hpp"

namespace ImGui {
  bool DragScalar(const char *label, uint32_t &value, float speed, uint32_t min, uint32_t max) {
    return DragScalar(label, ImGuiDataType_U32, &value, speed, &min, &max, "%u");
  }

  bool DragScalar(const char *label, int32_t &value, float speed, int32_t min, int32_t max) {
    return DragScalar(label, ImGuiDataType_S32, &value, speed, &min, &max, "%d");
  }

  bool DragScalar(const char *label, uint64_t &value, float speed, uint64_t min, uint64_t max) {
    return DragScalar(label, ImGuiDataType_U64, &value, speed, &min, &max, "%llu");
  }

  bool DragScalar(const char *label, int64_t &value, float speed, int64_t min, int64_t max) {
    return DragScalar(label, ImGuiDataType_S64, &value, speed, &min, &max, "%lld");
  }

  bool DragScalar(const char *label, float &value, float speed, float min, float max) {
    return DragScalar(label, ImGuiDataType_Float, &value, speed, &min, &max, "%f");
  }
  bool DragScalar(const char *label, double &value, double speed, double min, double max) {
    return DragScalar(label, ImGuiDataType_Double, &value, speed, &min, &max, "%f");
  }
}

namespace skel::menu {

  void RenderDiagView(cdc::GameStateBase *GameState)
  {
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("DIAG VIEW");
    RenderMatrixDiagWindow(GameState);
    ImGui::End();
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
  }

  void RenderMatrixDiagWindow(cdc::GameStateBase *GameState) {
    {
      char obj_label[16] = {};
      char col_label[16] = {};
      // Iterate each object transform
      for (auto i = 0; i < GameState->sceneSetPosN; i++) {
        std::snprintf(obj_label, sizeof(obj_label), "OBJECT %d", i);
        if (ImGui::TreeNode(obj_label)) {
          for (auto j = 0; j < 4; j++) {
            for (auto k = 0; k < 4; k++) {
              std::snprintf(col_label, sizeof(col_label), "col_%d_%d_%d", i+1, j+1, k+1);
              ImGui::DragScalar(col_label, GameState->lastConstData[i].second.m[j][k], 0.001, -20, 20);
            }
          }
          ImGui::TreePop();
          ImGui::Spacing();
        }
      }
    }
  }

} // namespace skel::menu