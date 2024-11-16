#pragma once

namespace skel::cdc {
#ifdef TRL
  // Address Specific Information
  constexpr auto *PCDeviceManagerSig = "A1 ? ? ? ? 8B 0D ? ? ? ? 68 ? ? ? ? 50 E8";
  constexpr auto GlobalDataAddr      = 0x1076980;
  constexpr auto PCRenderDeviceAddr  = 0x10027CC;
  constexpr auto CameraAddr          = 0x10FC660;
  constexpr auto InputDisableAddr    = 0x1101689;
  constexpr auto PCDeviceManagerECXPtrOff = 0x7;
  constexpr auto PCDeviceManagerStateManagerOff = 0x214;

  // Default Configuration / Variables
  constexpr auto DefaultFovDeg     = 50;
  constexpr auto DefaultFovDist    = 512.0f;
  constexpr auto DefaultMinDepth   = 0.98f;
  constexpr auto DefaultMaxPath    = 9.8e7f;
  constexpr auto DefaultFrameStart = 60 * 5;
#endif
}
