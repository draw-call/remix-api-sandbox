#include "dxerror.hpp"
#include "state.hpp"
#include "timer.hpp"
#include "meshloader.hpp"

//--------------------------------------------------------------------------------------
// Call-Once AppCtxManager
//--------------------------------------------------------------------------------------
AppCtxManager::AppCtxManager() {
  if (g_ctx == NULL) {
    // Initialize RTX API
    // XXX: THIS MUST BE DONE BEFORE ENTERING A CRITICAL SECTION
    remixapi_ErrorCode r;
    remixapi_Interface remix;
    if ((r = remixapi::bridge_initRemixApi(&remix)) != REMIXAPI_ERROR_CODE_SUCCESS) {
      DXTRACE_ERR_MSGBOX(
        L"bridge_initRemixApi: failed",
        r
      );
    }

    // Set initial state
    g_bThreadSafe = false;
    g_ctx = std::unique_ptr<AppCtx>(new AppCtx());
    if (!g_ctx.get()) {
      DXTRACE_ERR_MSGBOX(
        L"AppCtx creation silently failed",
        E_FAIL
      );
    }

    // Set runtime components
    g_ctx.get()->SetD3D9(Direct3DCreate9(D3D_SDK_VERSION));
    g_ctx.get()->SetRemix(remix);
    g_ctx.get()->SetEnumerator(new CD3D9Enumeration((g_ctx.get()->GetD3D9()), false));
    g_ctx.get()->SetMeshLoader(new CMeshLoader());
  }
}

AppCtx *GetAppCtx() {
  static AppCtxManager g_ctx_manager;
  assert (g_ctx);
  return  g_ctx.get();
}

AppCtxManager::~AppCtxManager() {
  g_ctx.reset();
}


//--------------------------------------------------------------------------------------
// Pauses time or rendering.  Keeps a ref count so pausing can be layered
//--------------------------------------------------------------------------------------
void WINAPI AppPause(bool bPauseTime, bool bPauseRendering) {
  int nPauseTimeCount = GetAppCtx()->GetPauseTimeCount();
  if (bPauseTime) {
    nPauseTimeCount++;
  } else {
    nPauseTimeCount--;
  }

  if (nPauseTimeCount < 0) {
    nPauseTimeCount = 0;
  }
  GetAppCtx()->SetPauseTimeCount(nPauseTimeCount);

  int nPauseRenderingCount = GetAppCtx()->GetPauseRenderingCount();
  if (bPauseRendering) {
    nPauseRenderingCount++;
  } else {
    nPauseRenderingCount--;
  }

  if (nPauseRenderingCount < 0) {
    nPauseRenderingCount = 0;
  }
  
  GetAppCtx()->SetPauseRenderingCount(nPauseRenderingCount);

  if (nPauseTimeCount > 0) {
    // Stop the scene from animating
    AppGetGlobalTimer()->Stop();
  } else {
    // Restart the timer
    AppGetGlobalTimer()->Start();
  }

  GetAppCtx()->SetIsRenderingPaused(nPauseRenderingCount > 0);
  GetAppCtx()->SetIsTimePaused(nPauseTimeCount > 0);
}

