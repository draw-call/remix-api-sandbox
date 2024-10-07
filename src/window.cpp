
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
  AppState *appState = GetAppState();

  // Not allowed to call this from inside the device callbacks
  if (appState->GetInsideDeviceCallback())
    return DXUT_ERR_MSGBOX(L"AppCreateWindow inside device callback", E_FAIL);

  appState->SetWindowCreateCalled(true);

  if (!appState->GetInited()) {
    // If AppInit() was already called and failed, then fail.
    // AppInit() must first succeed for this function to succeed
    if (appState->GetInitCalled()) {
      return E_FAIL;
    }
    return DXTRACE_ERR_MSGBOX(L"AppCreateWindow called before AppInit. FIXME", E_FAIL);

    // If AppInit() hasn't been called, then automatically call it
    // with default params
 /* hr = AppInit();
    if (FAILED(hr)) {
      return hr;
    }*/
  }

  if (appState->GetHWNDFocus() == NULL) {
    if (hInstance == NULL) {
      hInstance = static_cast<HINSTANCE>(GetModuleHandle(NULL));
    }
    appState->SetHInstance(hInstance);

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
      if (dwError != ERROR_CLASS_ALREADY_EXISTS)
        return DXUT_ERR_MSGBOX(L"RegisterClass", HRESULT_FROM_WIN32(dwError));
    }

    // Override the window's initial & size position if there were cmd line args
    if (appState->GetOverrideStartX() != -1)
      x = appState->GetOverrideStartX();
    if (appState->GetOverrideStartY() != -1)
      y = appState->GetOverrideStartY();

    appState->SetWindowCreatedWithDefaultPositions(false);
    if (x == CW_USEDEFAULT && y == CW_USEDEFAULT)
      appState->SetWindowCreatedWithDefaultPositions(true);

    // Find the window's initial size, but it might be changed later
    int nDefaultWidth  = 640;
    int nDefaultHeight = 480;
    if (appState->GetOverrideWidth() != 0)
      nDefaultWidth = appState->GetOverrideWidth();
    if (appState->GetOverrideHeight() != 0)
      nDefaultHeight = appState->GetOverrideHeight();

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
      return DXUT_ERR_MSGBOX(L"CreateWindow", HRESULT_FROM_WIN32(dwError));
    }
  
    appState->SetIsWindowed(true);
    appState->SetIsFullscreen(false);
    appState->SetWindowCreated(true);
    appState->SetHWNDFocus(hWnd);
    appState->SetHWNDDeviceFullScreen(hWnd);
    appState->SetHWNDDeviceWindowed(hWnd);
    appState->SetWindowTitle((WCHAR *)strWindowTitle);
  }

  return S_OK;
}

//--------------------------------------------------------------------------------------
// Handles window messages
//--------------------------------------------------------------------------------------
LRESULT CALLBACK AppWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  AppState *appState = GetAppState();

  // Consolidate the keyboard messages and pass them to the app's keyboard callback
  if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) {
    bool bKeyDown = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
    DWORD dwMask  = (1 << 29);
    bool bAltDown = ((lParam & dwMask) != 0);

    bool* bKeys = appState->GetKeys();
    bKeys[(BYTE)(wParam & 0xFF)] = bKeyDown;

    LPAPPSTATECALLBACKKEYBOARD pCallbackKeyboard = appState->GetKeyboardFunc();
    if (pCallbackKeyboard)
      pCallbackKeyboard((UINT)wParam, bKeyDown, bAltDown, appState->GetKeyboardFuncUserContext());
  }

  // Consolidate the mouse button messages and pass them to the app's mouse callback
  if (uMsg == WM_LBUTTONDOWN   || uMsg == WM_LBUTTONUP     ||
      uMsg == WM_LBUTTONDBLCLK || uMsg == WM_MBUTTONDOWN   ||
      uMsg == WM_MBUTTONUP     || uMsg == WM_MBUTTONDBLCLK ||
      uMsg == WM_RBUTTONDOWN   || uMsg == WM_RBUTTONUP     ||
      uMsg == WM_RBUTTONDBLCLK || uMsg == WM_XBUTTONDOWN   ||
      uMsg == WM_XBUTTONUP     || uMsg == WM_XBUTTONDBLCLK ||
      uMsg == WM_MOUSEWHEEL    || (appState->GetNotifyOnMouseMove() && uMsg == WM_MOUSEMOVE)
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
      if (uMsg == WM_MOUSEWHEEL)
        nMouseWheelDelta = (short)HIWORD(wParam);

      int nMouseButtonState = LOWORD(wParam);
      bool bLeftButton   = ((nMouseButtonState & MK_LBUTTON)  != 0);
      bool bRightButton  = ((nMouseButtonState & MK_RBUTTON)  != 0);
      bool bMiddleButton = ((nMouseButtonState & MK_MBUTTON)  != 0);
      bool bSideButton1  = ((nMouseButtonState & MK_XBUTTON1) != 0);
      bool bSideButton2  = ((nMouseButtonState & MK_XBUTTON2) != 0);

      bool* bMouseButtons = appState->GetMouseButtons();
      bMouseButtons[0]    = bLeftButton;
      bMouseButtons[1]    = bMiddleButton;
      bMouseButtons[2]    = bRightButton;
      bMouseButtons[3]    = bSideButton1;
      bMouseButtons[4]    = bSideButton2;

      LPAPPSTATECALLBACKMOUSE pCallbackMouse = appState->GetMouseFunc();
      if (pCallbackMouse)
        pCallbackMouse(bLeftButton, bRightButton, bMiddleButton, bSideButton1, bSideButton2, nMouseWheelDelta,
          xPos, yPos, appState->GetMouseFuncUserContext());
  }

  // Pass all messages to the app's MsgProc callback, and don't
  // process further messages if the apps says not to.
  LPAPPSTATECALLBACKMSGPROC pCallbackMsgProc = appState->GetWindowMsgFunc();
  if (pCallbackMsgProc) {
    bool bNoFurtherProcessing = false;
    LRESULT nResult = pCallbackMsgProc(hWnd, uMsg, wParam, lParam, &bNoFurtherProcessing,
      appState->GetWindowMsgFuncUserContext());
    if (bNoFurtherProcessing)
      return nResult;
  }

  switch (uMsg) {
    case WM_PAINT: {
      // Handle paint messages when the app is paused
      if (appState->GetIsRenderingPaused() && appState->GetDeviceObjectsCreated() && appState->GetDeviceObjectsReset()) {
        HRESULT hr;
        double fTime = appState->GetTime();
        float fElapsedTime = appState->GetElapsedTime();

        IDirect3DDevice9* pd3dDevice = appState->GetD3D9Device();
        if (pd3dDevice) {
          LPAPPSTATECALLBACKD3D9FRAMERENDER pCallbackFrameRender = appState->GetD3D9FrameRenderFunc();
          if (pCallbackFrameRender != NULL)
            pCallbackFrameRender(pd3dDevice, fTime, fElapsedTime,
              appState->GetD3D9FrameRenderFuncUserContext());

          hr = pd3dDevice->Present(NULL, NULL, NULL, NULL);
          if (D3DERR_DEVICELOST == hr) {
            appState->SetDeviceLost(true);
          } else if (D3DERR_DRIVERINTERNALERROR == hr) {
            // When D3DERR_DRIVERINTERNALERROR is returned from Present(),
            // the application can do one of the following:
            //
            // - End, with the pop-up window saying that the application cannot continue
            //   because of problems in the display adapter and that the user should
            //   contact the adapter manufacturer.
            //
            // - Attempt to restart by calling IDirect3DDevice9::Reset, which is essentially the same
            //   path as recovering from a lost device. If IDirect3DDevice9::Reset fails with
            //   D3DERR_DRIVERINTERNALERROR, the application should end immediately with the message
            //   that the user should contact the adapter manufacturer.
            //
            // The framework attempts the path of resetting the device
            //
            appState->SetDeviceLost(true);
          }
        }
      }
      break;
    }

    case WM_SIZE: {

      if (SIZE_MINIMIZED == wParam) {
        AppPause(true, true); // Pause while we're minimized

        appState->SetMinimized(true);
        appState->SetMaximized(false);
      } else {
        RECT rcCurrentClient;
        GetClientRect(appState->GetHWNDFocus(), &rcCurrentClient);
        if (rcCurrentClient.top == 0 && rcCurrentClient.bottom == 0) {
          // Rapidly clicking the task bar to minimize and restore a window
          // can cause a WM_SIZE message with SIZE_RESTORED when
          // the window has actually become minimized due to rapid change
          // so just ignore this message
        } else if (SIZE_MAXIMIZED == wParam) {
          if (appState->GetMinimized())
            AppPause(false, false); // Unpause since we're no longer minimized
          appState->SetMinimized(false);
          appState->SetMaximized(true);
          CheckForWindowSizeChange();
          CheckForWindowChangingMonitors();
        } else if (SIZE_RESTORED == wParam) {
          // DXUTCheckForDXGIFullScreenSwitch();
          if (appState->GetMaximized()) {
            appState->SetMaximized(false);
            CheckForWindowSizeChange();
            CheckForWindowChangingMonitors();
          } else if (appState->GetMinimized()) {
            AppPause(false, false); // Unpause since we're no longer minimized
            appState->SetMinimized(false);
            CheckForWindowSizeChange();
            CheckForWindowChangingMonitors();
          } else if (appState->GetInSizeMove()) {
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
      ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 480;
      ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 640;
      break;
    }

    case WM_ENTERSIZEMOVE: {
      // Halt frame movement while the app is sizing or moving
      AppPause(true, true);
      appState->SetInSizeMove(true);
      break;
    }

    case WM_EXITSIZEMOVE: {
      AppPause(false, false);
      CheckForWindowSizeChange();
      CheckForWindowChangingMonitors();
      appState->SetInSizeMove(false);
      break;
    }

    case WM_MOUSEMOVE: {
      if (appState->GetIsActive() && !appState->GetIsWindowed()) {
        IDirect3DDevice9* pd3dDevice = appState->GetD3D9Device();
        if (pd3dDevice) {
          POINT ptCursor;
          GetCursorPos(&ptCursor);
          pd3dDevice->SetCursorPosition(ptCursor.x, ptCursor.y, 0);
        }
      }
      break;
    }

    case WM_SETCURSOR: {
      if (appState->GetIsActive() && !appState->GetIsWindowed()) {
        IDirect3DDevice9* pd3dDevice = appState->GetD3D9Device();
        if (pd3dDevice && appState->GetShowCursorWhenFullScreen())
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
        if (!appState->GetIsWindowed())
          return 0;
        break;
      }
      break;
    }

    case WM_SYSKEYDOWN: {
      switch (wParam) {
        case VK_RETURN: {
          // TODO: Enable fullscreen swap
          //if (appState->GetHandleAltEnter()) {
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
          if (appState->GetHandleEscape()) {
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
      appState->SetIsWindowed(false);
      appState->SetIsFullscreen(false);
      appState->SetWindowCreated(false);
      appState->SetHWNDFocus(NULL);
      appState->SetHWNDDeviceFullScreen(NULL);
      appState->SetHWNDDeviceWindowed(NULL);
      appState->SetWindowTitle((WCHAR *)L"Destroyed");
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