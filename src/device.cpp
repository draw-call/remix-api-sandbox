#include <strsafe.h>
#include <d3dx9.h>

#include "device.hpp"
#include "dxerror.hpp"
#include "statecb.hpp"

//--------------------------------------------------------------------------------------
// Used to sort D3DDISPLAYMODEs
//--------------------------------------------------------------------------------------
static int __cdecl SortModesCallback(const void *arg1, const void *arg2)
{
  D3DDISPLAYMODE *pdm1 = (D3DDISPLAYMODE *)arg1;
  D3DDISPLAYMODE *pdm2 = (D3DDISPLAYMODE *)arg2;

  if (pdm1->Width > pdm2->Width)
    return 1;
  if (pdm1->Width < pdm2->Width)
    return -1;
  if (pdm1->Height > pdm2->Height)
    return 1;
  if (pdm1->Height < pdm2->Height)
    return -1;
  if (pdm1->Format > pdm2->Format)
    return 1;
  if (pdm1->Format < pdm2->Format)
    return -1;
  if (pdm1->RefreshRate > pdm2->RefreshRate)
    return 1;
  if (pdm1->RefreshRate < pdm2->RefreshRate)
    return -1;
  return 0;
}

//
//
// class CD3D9Enumeration
//
//

CD3D9Enumeration::CD3D9Enumeration(IDirect3D9 *d3d9Context, bool doEnumeration)
{
  m_pD3D = d3d9Context;
  m_IsD3D9DeviceAcceptableFunc             = NULL;
  m_pIsD3D9DeviceAcceptableFuncUserContext = NULL;

  SetResolutionMinMax(640, 480, UINT_MAX, UINT_MAX);
  SetRefreshMinMax(0, UINT_MAX);
  SetMultisampleQualityMax(0xFFFF);
  SetPossibleVertexProcessingList(true, true, true, false);
  SetRequirePostPixelShaderBlending(true);

  ResetPossibleDepthStencilFormats();
  ResetPossibleMultisampleTypeList();
  ResetPossiblePresentIntervalList();

  if (doEnumeration && !HasEnumerated()) {
    LPAPPSTATECALLBACKISD3D9DEVICEACCEPTABLE func = nullptr;
    void *context = nullptr;
    // GetAppState()->AppGetCallbackD3D9DeviceAcceptable(&m_IsD3D9DeviceAcceptableFunc, &m_pIsD3D9DeviceAcceptableFuncUserContext);
    if (FAILED(Enumerate())) {
      DXTRACE_ERR_MSGBOX(L"CD3D9Enumeration failed constructor enumeration!", E_FAIL);
    }
  }
}

CD3D9Enumeration::~CD3D9Enumeration() {
  ClearAdapterInfoList();
}

 
//--------------------------------------------------------------------------------------
IDirect3DDevice9 *WINAPI CD3D9Enumeration::CreateRefDevice(
    HWND hWnd,
    bool bNullRef
) {
  HRESULT hr;
  assert(m_pD3D);

  D3DDISPLAYMODE Mode;
  m_pD3D->GetAdapterDisplayMode(0, &Mode);

  RECT rc;
  GetClientRect(hWnd, &rc);

  D3DPRESENT_PARAMETERS pp = {
    .BackBufferWidth        = static_cast<UINT>(rc.right),
    .BackBufferHeight       = static_cast<UINT>(rc.bottom),
    .BackBufferFormat       = Mode.Format,
    .BackBufferCount        = 1,
    .MultiSampleType        = D3DMULTISAMPLE_NONE,
    .MultiSampleQuality     = 0,
    .SwapEffect             = D3DSWAPEFFECT_COPY,
    .hDeviceWindow          = hWnd,
    .Windowed               = TRUE,
    .EnableAutoDepthStencil = false,
    .AutoDepthStencilFormat = D3DFMT_D16,
    .Flags                  = 0,
    .FullScreen_RefreshRateInHz = 0,
    .PresentationInterval       = 0
  };

  IDirect3DDevice9 *pd3dDevice = NULL;
  hr = m_pD3D->CreateDevice(
    D3DADAPTER_DEFAULT,
    bNullRef ? D3DDEVTYPE_NULLREF : D3DDEVTYPE_REF,
    hWnd,
    D3DCREATE_HARDWARE_VERTEXPROCESSING,
    &pp,
    &pd3dDevice
  );

  return pd3dDevice;
}



//--------------------------------------------------------------------------------------
// Enumerate for each adapter all of the supported display modes,
// device types, adapter formats, back buffer formats, window/full screen support,
// depth stencil formats, multisampling types/qualities, and presentations intervals.
//
// For each combination of device type (HAL/REF), adapter format, back buffer format, and
// IsWindowed it will call the app's ConfirmDevice callback.  This allows the app
// to reject or allow that combination based on its caps/etc.  It also allows the
// app to change the BehaviorFlags.  The BehaviorFlags defaults non-pure HWVP
// if supported otherwise it will default to SWVP, however the app can change this
// through the ConfirmDevice callback.
//--------------------------------------------------------------------------------------
HRESULT CD3D9Enumeration::Enumerate(
    LPAPPSTATECALLBACKISD3D9DEVICEACCEPTABLE IsD3D9DeviceAcceptableFunc,
    void *pIsD3D9DeviceAcceptableFuncUserContext
)
{
  if (m_pD3D == NULL) {
    return DXTRACE_ERR(L"No Direct3D9 capable device detected", D3DERR_NOTFOUND);
  }

  m_IsD3D9DeviceAcceptableFunc = IsD3D9DeviceAcceptableFunc;
  m_pIsD3D9DeviceAcceptableFuncUserContext = pIsD3D9DeviceAcceptableFuncUserContext;

  HRESULT hr;
  ClearAdapterInfoList();
  CGrowableArray<D3DFORMAT> adapterFormatList;

  const D3DFORMAT allowedAdapterFormatArray[] = {D3DFMT_X8R8G8B8, D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, D3DFMT_A2R10G10B10};
  const UINT allowedAdapterFormatArrayCount   = sizeof(allowedAdapterFormatArray) / sizeof(allowedAdapterFormatArray[0]);

  UINT numAdapters = m_pD3D->GetAdapterCount();
  for (UINT adapterOrdinal = 0; adapterOrdinal < numAdapters; adapterOrdinal++) {
    CD3D9EnumAdapterInfo *pAdapterInfo = new CD3D9EnumAdapterInfo;
    if (pAdapterInfo == NULL)
      return DXTRACE_ERR(L"CD3D9Enumeration OOM", E_OUTOFMEMORY);

    pAdapterInfo->AdapterOrdinal = adapterOrdinal;
    m_pD3D->GetAdapterIdentifier(adapterOrdinal, 0, &pAdapterInfo->AdapterIdentifier);

    // Get list of all display modes on this adapter.
    // Also build a temporary list of all display adapter formats.
    adapterFormatList.RemoveAll();

    for (UINT iFormatList = 0; iFormatList < allowedAdapterFormatArrayCount; iFormatList++) {
      D3DFORMAT allowedAdapterFormat = allowedAdapterFormatArray[iFormatList];
      UINT numAdapterModes = m_pD3D->GetAdapterModeCount(adapterOrdinal, allowedAdapterFormat);
      for (UINT mode = 0; mode < numAdapterModes; mode++) {
        D3DDISPLAYMODE displayMode;
        m_pD3D->EnumAdapterModes(adapterOrdinal, allowedAdapterFormat, mode, &displayMode);

        if (displayMode.Width  < m_nMinWidth  ||
            displayMode.Height < m_nMinHeight || 
            displayMode.Width  > m_nMaxWidth  ||
            displayMode.Height > m_nMaxHeight ||
            displayMode.RefreshRate < m_nRefreshMin ||
            displayMode.RefreshRate > m_nRefreshMax)
        {
          continue;
        }

        pAdapterInfo->displayModeList.Add(displayMode);

        if (!adapterFormatList.Contains(displayMode.Format))
          adapterFormatList.Add(displayMode.Format);
      }
    }

    D3DDISPLAYMODE displayMode;
    m_pD3D->GetAdapterDisplayMode(adapterOrdinal, &displayMode);
    if (!adapterFormatList.Contains(displayMode.Format))
      adapterFormatList.Add(displayMode.Format);

    // Sort displaymode list
    qsort(
        pAdapterInfo->displayModeList.GetData(), pAdapterInfo->displayModeList.GetSize(), sizeof(D3DDISPLAYMODE),
        SortModesCallback
    );

    // Get info for each device on this adapter
    if (FAILED(EnumerateDevices(pAdapterInfo, &adapterFormatList))) {
      delete pAdapterInfo;
      continue;
    }

    // If at least one device on this adapter is available and compatible
    // with the app, add the adapterInfo to the list
    if (pAdapterInfo->deviceInfoList.GetSize() > 0) {
      hr = m_AdapterInfoList.Add(pAdapterInfo);
      if (FAILED(hr))
        return hr;
    } else
      delete pAdapterInfo;
  }

  //
  // Check for 2 or more adapters with the same name. Append the name
  // with some instance number if that's the case to help distinguish
  // them.
  //
  bool bUniqueDesc = true;
  CD3D9EnumAdapterInfo *pAdapterInfo;
  for (int i = 0; i < m_AdapterInfoList.GetSize(); i++) {
    CD3D9EnumAdapterInfo *pAdapterInfo1 = m_AdapterInfoList.GetAt(i);

    for (int j = i + 1; j < m_AdapterInfoList.GetSize(); j++) {
      CD3D9EnumAdapterInfo *pAdapterInfo2 = m_AdapterInfoList.GetAt(j);
      if (_stricmp(pAdapterInfo1->AdapterIdentifier.Description, pAdapterInfo2->AdapterIdentifier.Description) == 0) {
        bUniqueDesc = false;
        break;
      }
    }

    if (!bUniqueDesc)
      break;
  }

  for (int i = 0; i < m_AdapterInfoList.GetSize(); i++) {
    pAdapterInfo = m_AdapterInfoList.GetAt(i);

    MultiByteToWideChar(
      CP_ACP, 0, pAdapterInfo->AdapterIdentifier.Description, -1, pAdapterInfo->szUniqueDescription, 100
    );
    pAdapterInfo->szUniqueDescription[100] = 0;

    if (!bUniqueDesc) {
      WCHAR sz[100];
      swprintf_s(sz, 100, L" (#%d)", pAdapterInfo->AdapterOrdinal);
      wcscat_s(pAdapterInfo->szUniqueDescription, 256, sz);
    }
  }

  m_bHasEnumerated = true;

  return S_OK;
}

//--------------------------------------------------------------------------------------
// Enumerates D3D devices for a particular adapter.
//--------------------------------------------------------------------------------------
HRESULT CD3D9Enumeration::EnumerateDevices(
  CD3D9EnumAdapterInfo* pAdapterInfo,
  CGrowableArray<D3DFORMAT>* pAdapterFormatList)
{
  HRESULT hr;

  const D3DDEVTYPE devTypeArray[] = {
    D3DDEVTYPE_HAL,
    D3DDEVTYPE_SW,
    D3DDEVTYPE_REF
  };
  const UINT devTypeArrayCount = sizeof(devTypeArray) / sizeof(devTypeArray[0]);

  // Enumerate each Direct3D device type
  for (UINT iDeviceType = 0; iDeviceType < devTypeArrayCount; iDeviceType++) {
    CD3D9EnumDeviceInfo* pDeviceInfo = new CD3D9EnumDeviceInfo;
    if (pDeviceInfo == NULL) {
      return DXTRACE_ERR(L"CD3D9Enumeration OOM", E_OUTOFMEMORY);
    }

    // Fill struct w/ AdapterOrdinal and D3DDEVTYPE
    pDeviceInfo->AdapterOrdinal = pAdapterInfo->AdapterOrdinal;
    pDeviceInfo->DeviceType = devTypeArray[iDeviceType];

    // Store device caps
    if (FAILED(hr = m_pD3D->GetDeviceCaps(pAdapterInfo->AdapterOrdinal,
                 pDeviceInfo->DeviceType,
                 &pDeviceInfo->Caps)))
    {
      delete pDeviceInfo;
      continue;
    }

    if (pDeviceInfo->DeviceType != D3DDEVTYPE_HAL) {
      // Create a temp device to verify that it is really possible to create a
      // REF device [the developer DirectX redist has to be installed]
      IDirect3DDevice9 *pDevice = CreateRefDevice(GetForegroundWindow(), false);
      if (!pDevice) {
        delete pDeviceInfo;
        continue;
      }
      SAFE_RELEASE(pDevice);
    }

    // Get info for each devicecombo on this device
    if (FAILED(hr = EnumerateDeviceCombos(pAdapterInfo, pDeviceInfo,
                 pAdapterFormatList))) {
      delete pDeviceInfo;
      continue;
    }

    // If at least one devicecombo for this device is found,
    // add the deviceInfo to the list
    if (pDeviceInfo->deviceSettingsComboList.GetSize() > 0)
      pAdapterInfo->deviceInfoList.Add(pDeviceInfo);
    else
      delete pDeviceInfo;
  }

  return S_OK;
}

//--------------------------------------------------------------------------------------
// Enumerates DeviceCombos for a particular device.
//--------------------------------------------------------------------------------------
HRESULT CD3D9Enumeration::EnumerateDeviceCombos(
    CD3D9EnumAdapterInfo *pAdapterInfo,
    CD3D9EnumDeviceInfo  *pDeviceInfo,
    CGrowableArray<D3DFORMAT> *pAdapterFormatList
) {
  const D3DFORMAT backBufferFormatArray[] = {
      D3DFMT_A8R8G8B8,
      D3DFMT_X8R8G8B8,
      D3DFMT_A2R10G10B10,
      D3DFMT_R5G6B5,
      D3DFMT_A1R5G5B5,
      D3DFMT_X1R5G5B5
  };
  const UINT backBufferFormatArrayCount =
      sizeof(backBufferFormatArray) / sizeof(backBufferFormatArray[0]);

  // See which adapter formats are supported by this device
  for (int iFormat = 0; iFormat < pAdapterFormatList->GetSize(); iFormat++) {
    D3DFORMAT adapterFormat = pAdapterFormatList->GetAt(iFormat);

    for (UINT iBackBufferFormat = 0;
         iBackBufferFormat < backBufferFormatArrayCount; iBackBufferFormat++) {
      D3DFORMAT backBufferFormat = backBufferFormatArray[iBackBufferFormat];

      for (int nWindowed = 0; nWindowed < 2; nWindowed++) {
        if (!nWindowed && pAdapterInfo->displayModeList.GetSize() == 0)
          continue;

        if (FAILED(m_pD3D->CheckDeviceType(
                pAdapterInfo->AdapterOrdinal, pDeviceInfo->DeviceType,
                adapterFormat, backBufferFormat, nWindowed)))
        {
          continue;
        }

        if (m_bRequirePostPixelShaderBlending) {
          // If the backbuffer format doesn't support
          // D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING then alpha test, pixel fog,
          // render-target blending, color write enable, and dithering. are not
          // supported.
          if (FAILED(m_pD3D->CheckDeviceFormat(
                  pAdapterInfo->AdapterOrdinal, pDeviceInfo->DeviceType,
                  adapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                  D3DRTYPE_TEXTURE, backBufferFormat))) {
            continue;
          }
        }

        // If an application callback function has been provided, make sure this
        // device is acceptable to the app.
        if (m_IsD3D9DeviceAcceptableFunc != NULL) {
            if (!m_IsD3D9DeviceAcceptableFunc(
                    &pDeviceInfo->Caps, adapterFormat, backBufferFormat,
                    FALSE != nWindowed, m_pIsD3D9DeviceAcceptableFuncUserContext))
            continue;
        }

        // At this point, we have an
        // adapter/device/adapterformat/backbufferformat/iswindowed DeviceCombo
        // that is supported by the system and acceptable to the app. We still
        // need to find one or more suitable depth/stencil buffer format,
        // multisample type, and present interval.
        CD3D9EnumDeviceSettingsCombo pDeviceCombo = {
          .AdapterOrdinal   = pAdapterInfo->AdapterOrdinal,
          .DeviceType       = pDeviceInfo->DeviceType,
          .AdapterFormat    = adapterFormat,
          .BackBufferFormat = backBufferFormat,
          .Windowed         = (nWindowed != 0),
          .pAdapterInfo     = pAdapterInfo,
          .pDeviceInfo      = pDeviceInfo,
        };

        BuildDepthStencilFormatList(&pDeviceCombo);
        BuildMultiSampleTypeList(&pDeviceCombo);
        if (pDeviceCombo.multiSampleTypeList.GetSize() == 0) {
          delete &pDeviceCombo;
          continue;
        }
        BuildDSMSConflictList(&pDeviceCombo);
        BuildPresentIntervalList(pDeviceInfo, &pDeviceCombo);

        if (FAILED(pDeviceInfo->deviceSettingsComboList.Add(&pDeviceCombo))) {
          delete &pDeviceCombo;
        }
      }
    }
  }

  return S_OK;
}

//--------------------------------------------------------------------------------------
// Adds all depth/stencil formats that are compatible with the device
//       and app to the given D3DDeviceCombo.
//--------------------------------------------------------------------------------------
void CD3D9Enumeration::BuildDepthStencilFormatList(CD3D9EnumDeviceSettingsCombo *pDeviceCombo) {
  D3DFORMAT depthStencilFmt;
  for (int idsf = 0; idsf < m_DepthStencilPossibleList.GetSize(); idsf++) {
    depthStencilFmt = m_DepthStencilPossibleList.GetAt(idsf);
    if (SUCCEEDED(m_pD3D->CheckDeviceFormat(
            pDeviceCombo->AdapterOrdinal, pDeviceCombo->DeviceType,
            pDeviceCombo->AdapterFormat,  D3DUSAGE_DEPTHSTENCIL,
            D3DRTYPE_SURFACE, depthStencilFmt))) {
      if (SUCCEEDED(m_pD3D->CheckDepthStencilMatch(
              pDeviceCombo->AdapterOrdinal, pDeviceCombo->DeviceType,
              pDeviceCombo->AdapterFormat , pDeviceCombo->BackBufferFormat,
              depthStencilFmt))) {
        pDeviceCombo->depthStencilFormatList.Add(depthStencilFmt);
      }
    }
  }
}
//--------------------------------------------------------------------------------------
void CD3D9Enumeration::ResetPossibleDepthStencilFormats() {
  m_DepthStencilPossibleList.RemoveAll();
  m_DepthStencilPossibleList.Add(D3DFMT_D16);
  m_DepthStencilPossibleList.Add(D3DFMT_D15S1);
  m_DepthStencilPossibleList.Add(D3DFMT_D24X8);
  m_DepthStencilPossibleList.Add(D3DFMT_D24S8);
  m_DepthStencilPossibleList.Add(D3DFMT_D24X4S4);
  m_DepthStencilPossibleList.Add(D3DFMT_D32);
}

//--------------------------------------------------------------------------------------
// Adds all multisample types that are compatible with the device and app to
//       the given D3DDeviceCombo.
//--------------------------------------------------------------------------------------
void CD3D9Enumeration::BuildMultiSampleTypeList(CD3D9EnumDeviceSettingsCombo *pDeviceCombo) {
  D3DMULTISAMPLE_TYPE msType;
  DWORD msQuality;
  for (int imst = 0; imst < m_MultiSampleTypeList.GetSize(); imst++) {
    msType = m_MultiSampleTypeList.GetAt(imst);
    if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(
            pDeviceCombo->AdapterOrdinal  , pDeviceCombo->DeviceType,
            pDeviceCombo->BackBufferFormat, pDeviceCombo->Windowed, msType,
            &msQuality))) {
      pDeviceCombo->multiSampleTypeList.Add(msType);
      if (msQuality > m_nMultisampleQualityMax + 1)
        msQuality   = m_nMultisampleQualityMax + 1;
      pDeviceCombo->multiSampleQualityList.Add(msQuality);
    }
  }
}
//--------------------------------------------------------------------------------------
void CD3D9Enumeration::ResetPossibleMultisampleTypeList() {
  m_MultiSampleTypeList.RemoveAll();
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_NONE);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_NONMASKABLE);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_2_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_3_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_4_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_5_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_6_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_7_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_8_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_9_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_10_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_11_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_12_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_13_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_14_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_15_SAMPLES);
  m_MultiSampleTypeList.Add(D3DMULTISAMPLE_16_SAMPLES);
}

//--------------------------------------------------------------------------------------
// Find any conflicts between the available depth/stencil formats and
//       multisample types.
//--------------------------------------------------------------------------------------
void CD3D9Enumeration::BuildDSMSConflictList(CD3D9EnumDeviceSettingsCombo *pDeviceCombo) {
  CD3D9EnumDSMSConflict DSMSConflict {};

  for (int iDS = 0; iDS < pDeviceCombo->depthStencilFormatList.GetSize(); iDS++) {
    D3DFORMAT dsFmt = pDeviceCombo->depthStencilFormatList.GetAt(iDS);

    for (int iMS = 0; iMS < pDeviceCombo->multiSampleTypeList.GetSize(); iMS++) {
      D3DMULTISAMPLE_TYPE msType = pDeviceCombo->multiSampleTypeList.GetAt(iMS);

      if (FAILED(m_pD3D->CheckDeviceMultiSampleType(
              pDeviceCombo->AdapterOrdinal, pDeviceCombo->DeviceType, dsFmt,
              pDeviceCombo->Windowed, msType, NULL))) {
        DSMSConflict.DSFormat = dsFmt;
        DSMSConflict.MSType = msType;
        pDeviceCombo->DSMSConflictList.Add(DSMSConflict);
      }
    }
  }
}

//--------------------------------------------------------------------------------------
// Adds all present intervals that are compatible with the device and app
//       to the given D3DDeviceCombo.
//--------------------------------------------------------------------------------------
void CD3D9Enumeration::BuildPresentIntervalList(
    CD3D9EnumDeviceInfo *pDeviceInfo,
    CD3D9EnumDeviceSettingsCombo *pDeviceCombo
) {
  UINT pi;
  for (int ipi = 0; ipi < m_PresentIntervalList.GetSize(); ipi++) {
    pi = m_PresentIntervalList.GetAt(ipi);
    if (pDeviceCombo->Windowed) {
      if (pi == D3DPRESENT_INTERVAL_TWO || pi == D3DPRESENT_INTERVAL_THREE ||
          pi == D3DPRESENT_INTERVAL_FOUR) {
        // These intervals are not supported in windowed mode.
        continue;
      }
    }
    // Note that D3DPRESENT_INTERVAL_DEFAULT is zero, so you
    // can't do a caps check for it -- it is always available.
    if (pi == D3DPRESENT_INTERVAL_DEFAULT ||
        (pDeviceInfo->Caps.PresentationIntervals & pi)) {
      pDeviceCombo->presentIntervalList.Add(pi);
    }
  }
}
//--------------------------------------------------------------------------------------
void CD3D9Enumeration::ResetPossiblePresentIntervalList() {
  m_PresentIntervalList.RemoveAll();
  m_PresentIntervalList.Add(D3DPRESENT_INTERVAL_IMMEDIATE);
  m_PresentIntervalList.Add(D3DPRESENT_INTERVAL_DEFAULT);
  m_PresentIntervalList.Add(D3DPRESENT_INTERVAL_ONE);
  m_PresentIntervalList.Add(D3DPRESENT_INTERVAL_TWO);
  m_PresentIntervalList.Add(D3DPRESENT_INTERVAL_THREE);
  m_PresentIntervalList.Add(D3DPRESENT_INTERVAL_FOUR);
}

//--------------------------------------------------------------------------------------
// Release all the allocated CD3D9EnumAdapterInfo objects and empty the list
//--------------------------------------------------------------------------------------
void CD3D9Enumeration::ClearAdapterInfoList() {
  CD3D9EnumAdapterInfo* pAdapterInfo;
  for (int i = 0; i < m_AdapterInfoList.GetSize(); i++) {
    pAdapterInfo = m_AdapterInfoList.GetAt(i);
    delete pAdapterInfo;
  }
  m_AdapterInfoList.RemoveAll();
}


//
//
// class CD3D9EnumAdapterInfo
//
//

//--------------------------------------------------------------------------------------
CD3D9EnumAdapterInfo::~CD3D9EnumAdapterInfo( void ) {
  CD3D9EnumDeviceInfo* pDeviceInfo;
  for (int i = 0; i < deviceInfoList.GetSize(); i++) {
    pDeviceInfo = deviceInfoList.GetAt( i );
    delete pDeviceInfo;
  }
  deviceInfoList.RemoveAll();
}

//--------------------------------------------------------------------------------------
CD3D9EnumDeviceInfo::~CD3D9EnumDeviceInfo(void) {
  CD3D9EnumDeviceSettingsCombo* pDeviceCombo;
  for (int i = 0; i < deviceSettingsComboList.GetSize(); i++) {
    pDeviceCombo = deviceSettingsComboList.GetAt(i);
    delete pDeviceCombo;
  }
  deviceSettingsComboList.RemoveAll();
}
