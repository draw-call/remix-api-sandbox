#pragma once

#include "remixapi/bridge_remix_api.h"


struct AppRemixInterface : remixapi_Interface {

  struct local {
    bool sphereLightDirection;

    remixapi_LightInfo          lightInfo;
    remixapi_LightInfoSphereEXT sphereLight;
    remixapi_LightHandle        sceneLight;

    remixapi_CameraInfo cameraInfo;
    remixapi_CameraInfoParameterizedEXT parametersForCamera;
  };

  local app;

  AppRemixInterface() {
    app.sphereLightDirection = 1;
  }

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
};


