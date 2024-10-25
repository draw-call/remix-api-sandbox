
#include "state.hpp"
#include "window.hpp"

//--------------------------------------------------------------------------------------
// Creates a window with the specified window title, icon, menu, and starting position.
//--------------------------------------------------------------------------------------
HRESULT WINAPI AppCreateWindow(
  const WCHAR* strWindowTitle,
  HINSTANCE hInstance, HICON hIcon, HMENU hMenu,
  int x, int y
)
{
  HRESULT  hr = 0;
  AppCtx *appCtx = GetAppCtx();

  // Not allowed to call this from inside the device callbacks
  if (appCtx->GetInsideDeviceCallback()) {
    return DXTRACE_ERR_MSGBOX(
      L"AppCreateWindow inside device callback",
      E_FAIL
    );
  }

  if (!appCtx->GetInited()) {
    return DXTRACE_ERR_MSGBOX(
      L"AppCreateWindow called before AppInit. FIXME",
      E_FAIL
    );
  }
  if (appCtx->GetWindowCreated()) {
    return DXTRACE_ERR_MSGBOX(
      L"AppCreateWindow called with existing window",
      E_FAIL
    );
  }
  if (appCtx->GetHWNDFocus()) {
    return DXTRACE_ERR_MSGBOX(
      L"Existing HWND without window?",
      E_FAIL
    );
  }
  
  appCtx->SetWindowCreateCalled(true);

  if (hInstance == NULL) {
    hInstance = static_cast<HINSTANCE>(GetModuleHandle(NULL));
  }
  appCtx->SetHInstance(hInstance);

  // Register the windows class
  WNDCLASS wndClass = {
    .style         = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc   = AppWndProc,
    .cbClsExtra    = 0,
    .cbWndExtra    = 0,
    .hInstance     = hInstance,
    .hIcon         = hIcon,
    .hCursor       = LoadCursor(NULL, IDC_ARROW),
    .hbrBackground = NULL,
    .lpszMenuName  = NULL,
    .lpszClassName = "Direct3DWindowClass",
  };
  if (!RegisterClass(&wndClass)) {
    DWORD dwError = GetLastError();
    if (dwError != ERROR_CLASS_ALREADY_EXISTS) {
      return DXTRACE_ERR_MSGBOX(L"RegisterClass", HRESULT_FROM_WIN32(dwError));
    }
  }

  // Override the window's initial & size position if there were cmd line args
  if (appCtx->GetOverrideStartX() != -1) {
    x = appCtx->GetOverrideStartX();
  }
  if (appCtx->GetOverrideStartY() != -1) {
    y = appCtx->GetOverrideStartY();
  }

  appCtx->SetWindowCreatedWithDefaultPositions(false);
  if (x == CW_USEDEFAULT && y == CW_USEDEFAULT) {
    appCtx->SetWindowCreatedWithDefaultPositions(true);
  }

  // Find the window's initial size, but it might be changed later
  int nDefaultWidth  = 1024;
  int nDefaultHeight = 760;
  if (appCtx->GetOverrideWidth() != 0) {
    nDefaultWidth = appCtx->GetOverrideWidth();
  }
  if (appCtx->GetOverrideHeight() != 0) {
    nDefaultHeight = appCtx->GetOverrideHeight();
  }

  RECT rc;
  SetRect(&rc, 0, 0, nDefaultWidth, nDefaultHeight);
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, (hMenu != NULL) ? true : false);

  // Create the render window
  HWND hWnd = CreateWindowW(
    L"Direct3DWindowClass",
    strWindowTitle,
    WS_OVERLAPPEDWINDOW,
    x, y,
    (rc.right  - rc.left),
    (rc.bottom - rc.top),
    0,
    hMenu, hInstance,
    0
  );
  if (hWnd == NULL) {
    DWORD dwError = GetLastError();
    return DXTRACE_ERR_MSGBOX(L"CreateWindow", HRESULT_FROM_WIN32(dwError));
  }

  appCtx->SetIsWindowed(true);
  appCtx->SetIsFullscreen(false);
  appCtx->SetWindowCreated(true);
  appCtx->SetHWNDFocus(hWnd);
  appCtx->SetHWNDDeviceFullScreen(hWnd);
  appCtx->SetHWNDDeviceWindowed(hWnd);
  appCtx->SetWindowTitle((WCHAR *)strWindowTitle);

  return S_OK;
}

//--------------------------------------------------------------------------------------
// Handles window messages
//--------------------------------------------------------------------------------------
LRESULT CALLBACK AppWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  AppCtx *appCtx = GetAppCtx();

  // Consolidate the keyboard messages and pass them to the app's keyboard callback
  if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) {
    bool bKeyDown = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
    DWORD dwMask  = (1 << 29);
    bool bAltDown = ((lParam & dwMask) != 0);

    bool* bKeys = appCtx->GetKeys();
    bKeys[(BYTE)(wParam & 0xFF)] = bKeyDown;

    LPAPPCTXCALLBACKKEYBOARD pCallbackKeyboard = appCtx->GetKeyboardFunc();
    if (pCallbackKeyboard)
      pCallbackKeyboard((UINT)wParam, bKeyDown, bAltDown, appCtx->GetKeyboardFuncUserContext());
  }

  // Consolidate the mouse button messages and pass them to the app's mouse callback
  if (uMsg == WM_LBUTTONDOWN   || uMsg == WM_LBUTTONUP     ||
      uMsg == WM_LBUTTONDBLCLK || uMsg == WM_MBUTTONDOWN   ||
      uMsg == WM_MBUTTONUP     || uMsg == WM_MBUTTONDBLCLK ||
      uMsg == WM_RBUTTONDOWN   || uMsg == WM_RBUTTONUP     ||
      uMsg == WM_RBUTTONDBLCLK || uMsg == WM_XBUTTONDOWN   ||
      uMsg == WM_XBUTTONUP     || uMsg == WM_XBUTTONDBLCLK ||
      uMsg == WM_MOUSEWHEEL    || (appCtx->GetNotifyOnMouseMove() && uMsg == WM_MOUSEMOVE)
  )
  {
    int xPos = (short)LOWORD(lParam);
    int yPos = (short)HIWORD(lParam);

    if (uMsg == WM_MOUSEWHEEL) {
      // WM_MOUSEWHEEL passes screen mouse coords
      // so convert them to client coords
      POINT pt {};
      pt.x = xPos;
      pt.y = yPos;
      ScreenToClient(hWnd, &pt);
      xPos = pt.x;
      yPos = pt.y;
    }

    int nMouseWheelDelta = 0;
    if (uMsg == WM_MOUSEWHEEL) {
      nMouseWheelDelta = (short)HIWORD(wParam);
    }

    int nMouseButtonState = LOWORD(wParam);
    bool bLeftButton   = ((nMouseButtonState & MK_LBUTTON)  != 0);
    bool bRightButton  = ((nMouseButtonState & MK_RBUTTON)  != 0);
    bool bMiddleButton = ((nMouseButtonState & MK_MBUTTON)  != 0);
    bool bSideButton1  = ((nMouseButtonState & MK_XBUTTON1) != 0);
    bool bSideButton2  = ((nMouseButtonState & MK_XBUTTON2) != 0);

    bool* bMouseButtons = appCtx->GetMouseButtons();
    bMouseButtons[0]    = bLeftButton;
    bMouseButtons[1]    = bMiddleButton;
    bMouseButtons[2]    = bRightButton;
    bMouseButtons[3]    = bSideButton1;
    bMouseButtons[4]    = bSideButton2;

    LPAPPCTXCALLBACKMOUSE pCallbackMouse = appCtx->GetMouseFunc();
    if (pCallbackMouse) {
      pCallbackMouse(
        bLeftButton,
        bRightButton,
        bMiddleButton,
        bSideButton1,
        bSideButton2,
        nMouseWheelDelta,
        xPos, yPos,
        appCtx->GetMouseFuncUserContext()
      );
    }
  }

  // Pass all messages to the app's MsgProc callback, and don't
  // process further messages if the apps says not to.
  LPAPPCTXCALLBACKMSGPROC pCallbackMsgProc = appCtx->GetWindowMsgFunc();
  if (pCallbackMsgProc) {
    bool bNoFurtherProcessing = false;
    LRESULT nResult = pCallbackMsgProc(hWnd, uMsg, wParam, lParam, &bNoFurtherProcessing,
      appCtx->GetWindowMsgFuncUserContext());
    if (bNoFurtherProcessing)
      return nResult;
  }

  switch (uMsg) {
    case WM_PAINT: {
      // Handle paint messages when the app is paused
      if (appCtx->GetIsRenderingPaused() && appCtx->GetDeviceObjectsCreated() && appCtx->GetDeviceObjectsReset()) {
        HRESULT hr;
        double fTime = appCtx->GetTime();
        float fElapsedTime = appCtx->GetElapsedTime();

        IDirect3DDevice9* pd3dDevice = appCtx->GetD3D9Device();
        if (pd3dDevice) {
          LPAPPCTXCALLBACKD3D9FRAMERENDER pCallbackFrameRender = appCtx->GetD3D9FrameRenderFunc();
          if (pCallbackFrameRender != NULL)
            pCallbackFrameRender(pd3dDevice, fTime, fElapsedTime,
              appCtx->GetD3D9FrameRenderFuncUserContext());

          hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);
          if (D3DERR_DEVICELOST == hr) {
            appCtx->SetDeviceLost(true);
          } else if (D3DERR_DRIVERINTERNALERROR == hr) {
            appCtx->SetDeviceLost(true);
          }
        }
      }
      break;
    }

    case WM_SIZE: {

      if (SIZE_MINIMIZED == wParam) {
        AppPause(true, true); // Pause while we're minimized

        appCtx->SetMinimized(true);
        appCtx->SetMaximized(false);
      } else {
        RECT rcCurrentClient;
        GetClientRect(appCtx->GetHWNDFocus(), &rcCurrentClient);
        if (rcCurrentClient.top == 0 && rcCurrentClient.bottom == 0) {
          // Rapidly clicking the task bar to minimize and restore a window
          // can cause a WM_SIZE message with SIZE_RESTORED when
          // the window has actually become minimized due to rapid change
          // so just ignore this message
        } else if (SIZE_MAXIMIZED == wParam) {
          if (appCtx->GetMinimized())
            AppPause(false, false); // Unpause since we're no longer minimized
          appCtx->SetMinimized(false);
          appCtx->SetMaximized(true);
          CheckForWindowSizeChange();
          CheckForWindowChangingMonitors();
        } else if (SIZE_RESTORED == wParam) {
          // DXUTCheckForDXGIFullScreenSwitch();
          if (appCtx->GetMaximized()) {
            appCtx->SetMaximized(false);
            CheckForWindowSizeChange();
            CheckForWindowChangingMonitors();
          } else if (appCtx->GetMinimized()) {
            AppPause(false, false); // Unpause since we're no longer minimized
            appCtx->SetMinimized(false);
            CheckForWindowSizeChange();
            CheckForWindowChangingMonitors();
          } else if (appCtx->GetInSizeMove()) {
            // If we're neither maximized nor minimized, the window size
            // is changing by the user dragging the window edges.  In this
            // case, we don't reset the device yet -- we wait until the
            // user stops dragging, and a WM_EXITSIZEMOVE message comes.
          } else {
            // This WM_SIZE come from resizing the window via an API like SetWindowPos() so
            // resize and reset the device now.
            CheckForWindowSizeChange();
            CheckForWindowChangingMonitors();
          }
        }
      }
      break;
    }

    case WM_GETMINMAXINFO: {
      // TODO make configurable
      ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 640;
      ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 480;
      break;
    }

    case WM_ENTERSIZEMOVE: {
      // Halt frame movement while the app is sizing or moving
      AppPause(true, true);
      appCtx->SetInSizeMove(true);
      break;
    }

    case WM_EXITSIZEMOVE: {
      AppPause(false, false);
      CheckForWindowSizeChange();
      CheckForWindowChangingMonitors();
      appCtx->SetInSizeMove(false);
      break;
    }

    case WM_MOUSEMOVE: {
      if (appCtx->GetIsActive() && !appCtx->GetIsWindowed()) {
        IDirect3DDevice9* pd3dDevice = appCtx->GetD3D9Device();
        if (pd3dDevice) {
          POINT ptCursor;
          GetCursorPos(&ptCursor);
          pd3dDevice->SetCursorPosition(ptCursor.x, ptCursor.y, 0);
        }
      }
      break;
    }

    case WM_SETCURSOR: {
      if (appCtx->GetIsActive() && !appCtx->GetIsWindowed()) {
        IDirect3DDevice9* pd3dDevice = appCtx->GetD3D9Device();
        if (pd3dDevice && appCtx->GetShowCursorWhenFullScreen())
          pd3dDevice->ShowCursor(true);
        return true; // prevent Windows from setting cursor to window class cursor
      }
      break;
    }

    case WM_SYSCOMMAND: {
      // Prevent moving/sizing in full screen mode
      switch ((wParam & 0xFFF0)) {
      case SC_MOVE:
      case SC_SIZE:
      case SC_MAXIMIZE:
      case SC_KEYMENU:
        if (!appCtx->GetIsWindowed())
          return 0;
        break;
      }
      break;
    }

    case WM_SYSKEYDOWN: {
      switch (wParam) {
        case VK_RETURN: {
          // TODO: Enable fullscreen swap
          //if (appCtx->GetHandleAltEnter()) {
          //  // Toggle full screen upon alt-enter
          //  DWORD dwMask = (1 << 29);
          //  if ((lParam & dwMask) != 0) // Alt is down also
          //  {
          //    // Toggle the full screen/window mode
          //    AppPause(true, true);
          //    AppToggleFullScreen();
          //    AppPause(false, false);
          //    return 0;
          //  }
          // }
          break;
        }
      }
      break;
    }

    case WM_KEYDOWN: {
      switch (wParam) {
        case VK_ESCAPE: {
          if (appCtx->GetHandleEscape()) {
            SendMessage(hWnd, WM_CLOSE, 0, 0);
          }
          break;
        }
      }
      break;
    }

    case WM_CLOSE: {
      HMENU hMenu;
      hMenu = GetMenu(hWnd);
      if (hMenu != NULL) {
        DestroyMenu(hMenu);
      }
      DestroyWindow(hWnd);
      UnregisterClassW(L"Direct3DWindowClass", NULL);
      appCtx->SetIsWindowed(false);
      appCtx->SetIsFullscreen(false);
      appCtx->SetWindowCreated(false);
      appCtx->SetHWNDFocus(NULL);
      appCtx->SetHWNDDeviceFullScreen(NULL);
      appCtx->SetHWNDDeviceWindowed(NULL);
      appCtx->SetWindowTitle((WCHAR *)L"Destroyed");
      return 0;
    }

    case WM_DESTROY: {
      PostQuitMessage(0);
      break;
    }
  }
  return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

//--------------------------------------------------------------------------------------
// Checks if the window client rect has changed and if it has, then reset the device
//--------------------------------------------------------------------------------------
void CheckForWindowSizeChange()
{
  // TODO: ImplementMe
  return;
}


//--------------------------------------------------------------------------------------
// Checks to see if the HWND changed monitors, and if it did it creates a device 
// from the monitor's adapter and recreates the scene.
//--------------------------------------------------------------------------------------
void CheckForWindowChangingMonitors()
{
  // TODO: ImplementMe
  return;
}