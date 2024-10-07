#pragma once

#include <Windows.h>

//--------------------------------------------------------------------------------------
// Creates a window with the specified window title, icon, menu, and starting position.
//--------------------------------------------------------------------------------------
HRESULT WINAPI AppCreateWindow(
  const WCHAR* strWindowTitle,
  HINSTANCE hInstance, HICON hIcon, HMENU hMenu,
  int x, int y
);

//--------------------------------------------------------------------------------------
// Handles window messages
//--------------------------------------------------------------------------------------
LRESULT CALLBACK AppWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//--------------------------------------------------------------------------------------
// Checks if the window client rect has changed and if it has, then reset the device
//--------------------------------------------------------------------------------------
void CheckForWindowSizeChange();


//--------------------------------------------------------------------------------------
// Checks to see if the HWND changed monitors, and if it did it creates a device 
// from the monitor's adapter and recreates the scene.
//--------------------------------------------------------------------------------------
void CheckForWindowChangingMonitors();
