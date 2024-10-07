#pragma once

#include "remixapi/bridge_remix_api.h"

// Remix
static remixapi_LightInfo          g_remix_lightInfo;
static remixapi_LightHandle        g_remix_scene_light = NULL;
static remixapi_LightInfoSphereEXT g_remix_sphereLight;
static bool g_remix_sphereLightDirection = 1;

constexpr remixapi_HardcodedVertex makeVertex(float x, float y, float z)
{
  remixapi_HardcodedVertex v = {
    { x, y, z },
    { 0, 0, -1 },
    { 1, 1 },
    0xFFFFFFFF
  };
  return v;
}
