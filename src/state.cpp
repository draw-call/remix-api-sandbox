#include "dxerror.hpp"
#include "state.hpp"
#include "timer.hpp"
#include "meshloader.hpp"
#include "app_remix.hpp"

//--------------------------------------------------------------------------------------
// Call-Once AppCtxManager
//--------------------------------------------------------------------------------------
AppCtxManager::AppCtxManager() {
  if (g_ctx == NULL) {
    // Initialize RTX API
    // XXX: This must be done before entering a critical section 
    remixapi_ErrorCode r;
    AppRemixInterface *remix = new AppRemixInterface();
    if ((r = remixapi::bridge_initRemixApi(remix)) != REMIXAPI_ERROR_CODE_SUCCESS) {
      DXTRACE_ERR_MSGBOX(
        L"AppCtxManager: bridge_initRemixApi: failed",
        r
      );
    }

    // Set initial state
    g_bThreadSafe = false;
    g_ctx = std::unique_ptr<AppCtx>(new AppCtx());
    if (!g_ctx.get()) {
      DXTRACE_ERR_MSGBOX(
        L"AppCtxManager: AppCtx creation silently failed",
        E_FAIL
      );
    }

    auto D3D9 = Direct3DCreate9(D3D_SDK_VERSION);
    if (!D3D9) {
      DXTRACE_ERR_MSGBOX(
        L"AppCtxManager: D3D9 failed",
        HRESULT_FROM_WIN32(GetLastError())
      );

    }

    // Set runtime components
    g_ctx.get()->SetD3D9(D3D9);
    g_ctx.get()->SetRemix(remix);
    g_ctx.get()->SetEnumerator(
      new CD3D9Enumeration((g_ctx.get()->GetD3D9()), false)
    );
    g_ctx.get()->SetMeshLoader(
      new CMeshLoader()
    );
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

