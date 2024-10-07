#include "dxerror.hpp"
#include "state.hpp"
#include "timer.hpp"


//--------------------------------------------------------------------------------------
// Call-Once AppStateManager
//--------------------------------------------------------------------------------------
AppStateManager::AppStateManager() {
  if (g_state == NULL) {
    // Initialize RTX API Hooks
    // XXX: THIS MUST BE DONE BEFORE ENTERING A CRITICAL SECTION
    remixapi_ErrorCode r;
    remixapi_Interface remix;
    if ((r = remixapi::bridge_initRemixApi(&remix)) != REMIXAPI_ERROR_CODE_SUCCESS)
    {
      DXTRACE_ERR_MSGBOX(L"bridge_initRemixApi: failed", r);
    }
    Sleep(1000);
    
    // Set initial state
    g_bThreadSafe = false;
    g_state = std::unique_ptr<AppState>(new AppState());
    if (!g_state.get())
    {
      DXTRACE_ERR_MSGBOX(L"AppState creation silently failed", E_FAIL);
    }
    g_state.get()->SetRemix(remix);
    g_state.get()->SetD3D9(Direct3DCreate9(D3D_SDK_VERSION));
    g_state.get()->SetEnumerator(new CD3D9Enumeration((g_state.get()->GetD3D9()), false));
  }
}

AppState *GetAppState() {
  static AppStateManager g_state_manager;
  assert (g_state);
  return  g_state.get();
}

AppStateManager::~AppStateManager() {
  g_state.reset();
}


//--------------------------------------------------------------------------------------
// Pauses time or rendering.  Keeps a ref count so pausing can be layered
//--------------------------------------------------------------------------------------
void WINAPI AppPause(bool bPauseTime, bool bPauseRendering) {
  int nPauseTimeCount = GetAppState()->GetPauseTimeCount();
  if (bPauseTime) {
    nPauseTimeCount++;
  } else {
    nPauseTimeCount--;
  }

  if (nPauseTimeCount < 0) {
    nPauseTimeCount = 0;
  }
  GetAppState()->SetPauseTimeCount(nPauseTimeCount);

  int nPauseRenderingCount = GetAppState()->GetPauseRenderingCount();
  if (bPauseRendering) {
    nPauseRenderingCount++;
  } else {
    nPauseRenderingCount--;
  }

  if (nPauseRenderingCount < 0) {
    nPauseRenderingCount = 0;
  }
  
  GetAppState()->SetPauseRenderingCount(nPauseRenderingCount);

  if (nPauseTimeCount > 0) {
    // Stop the scene from animating
    AppGetGlobalTimer()->Stop();
  } else {
    // Restart the timer
    AppGetGlobalTimer()->Start();
  }

  GetAppState()->SetIsRenderingPaused(nPauseRenderingCount > 0);
  GetAppState()->SetIsTimePaused(nPauseTimeCount > 0);
}

