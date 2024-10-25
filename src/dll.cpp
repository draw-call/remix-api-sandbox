#include <iostream>

#include <Windows.h>
#include <assert.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "detours.h"

#include "kanan.h"
#include "skel/d3d9ex.hpp"

#ifndef DllName
#define DllName "magos.dll"
#endif

#ifndef CompanionName
#define CompanionName "magos.app.exe"
#endif

#ifndef CompanionExportDevice
#define CompanionExportDevice ExportDevice
#define CompanionExportDeviceStr "ExportDevice"
#endif

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
EXTERN_C __declspec(dllexport) VOID NullExport(VOID) { };

// Note: only used when running in ExportDevice mode (companion app)
static skel::d3d9ex::D3D9Device * gD3D9Device = NULL;

// Direct3DCreate9 Wrapper Hook
HOOK_STDCALL(IDirect3D9 *, LocalDirect3DCreate9, HARGS(UINT SDKVersion)) {
  std::cout << "[MAGOS] Inside LocalDirect3DCreate9" << std::endl;
  std::fflush(stdout);
  return (new skel::d3d9ex::_d3d9(o_LocalDirect3DCreate9(SDKVersion)));
}

// ExportDevice Hook
HOOK_STDCALL(skel::d3d9ex::D3D9Device *, CompanionExportDevice, HARGS(IDirect3DDevice9 * device)) {
  std::cout << "[MAGOS] Inside ExportDevice" << std::endl;
  std::fflush(stdout);
  gD3D9Device = new skel::d3d9ex::D3D9Device(device);
  return gD3D9Device;
}

//// Entrypoint
DWORD APIENTRY StartupThread([[maybe_unused]] LPVOID lpParam) noexcept
{
  if (AllocConsole()) {
    FILE* gLogFP;
    (void) freopen_s(&gLogFP, "CONIN$",  "r",     stdin);
    (void) freopen_s(&gLogFP, "CONOUT$", "w",     stdout);
    (void) freopen_s(&gLogFP, "CONOUT$", "w",     stderr);
    (void) freopen_s(&gLogFP, "console.txt", "w", stdout);
    SetConsoleTitleA(DllName);
  }
  std::cout << "[MAGOS] Injected" << std::endl;
  std::fflush(stdout);

  // Detours
  {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    o_ExportDevice = (t_ExportDevice)(GetProcAddress(GetModuleHandleA(CompanionName), CompanionExportDeviceStr));
    if (o_ExportDevice) {
      DetourAttach((PVOID *)&o_ExportDevice, ExportDevice);
    }
    
    o_LocalDirect3DCreate9 = &Direct3DCreate9;
    DetourAttach((PVOID *)&o_LocalDirect3DCreate9, LocalDirect3DCreate9);
    
    if (DetourTransactionCommit() != NO_ERROR) {
      std::cout << "[MAGOS] ERROR: Detour" << std::endl;
      std::fflush(stdout);
      goto exit;
    }
  }
 
  MSG msg;
  ZeroMemory(&msg, sizeof(msg));
  while(msg.message != WM_QUIT) {
    if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

exit:
  std::cout << "[MAGOS] Bye! :waves:" << std::endl;
  std::fflush(stdout);

  CreateThread(
    nullptr, 0,
    (LPTHREAD_START_ROUTINE)FreeLibrary,
    &__ImageBase,
    0, nullptr
  );

  return 0;
}

BOOL APIENTRY DllMain (HMODULE dllHandle, DWORD reason, LPVOID reserved)
{
  switch (reason) {
    case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls(dllHandle);
      CreateThread(0, 0, StartupThread, 0, 0, 0);
      break;
    case DLL_PROCESS_DETACH:
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      DetourDetach((PVOID *)&o_LocalDirect3DCreate9, LocalDirect3DCreate9);
      if (o_ExportDevice) {
        DetourDetach((PVOID *)&o_ExportDevice, ExportDevice);
      }
      DetourTransactionCommit();
      break;
  }
  return TRUE;
}