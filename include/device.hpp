#pragma once

#include <d3dx9.h>
#include "cgarray.hpp"
#include "statecb.hpp"

struct CD3D9EnumDSMSConflict;
struct CD3D9EnumDeviceSettingsCombo;
class  CD3D9EnumDeviceInfo;
class  CD3D9EnumAdapterInfo;
class  CD3D9Enumeration;

//--------------------------------------------------------------------------------------
// A depth/stencil buffer format that is incompatible with a
// multisample type.
//--------------------------------------------------------------------------------------
struct CD3D9EnumDSMSConflict
{
    D3DFORMAT DSFormat;
    D3DMULTISAMPLE_TYPE MSType;
};

//--------------------------------------------------------------------------------------
// A struct describing device settings that contains a unique combination of 
// adapter format, back buffer format, and windowed that is compatible with a 
// particular Direct3D device and the app.
//--------------------------------------------------------------------------------------
struct CD3D9EnumDeviceSettingsCombo
{
    UINT AdapterOrdinal;
    D3DDEVTYPE DeviceType;
    D3DFORMAT AdapterFormat;
    D3DFORMAT BackBufferFormat;
    BOOL Windowed;

    CGrowableArray <D3DFORMAT> depthStencilFormatList; // List of D3DFORMATs
    CGrowableArray <D3DMULTISAMPLE_TYPE> multiSampleTypeList; // List of D3DMULTISAMPLE_TYPEs
    CGrowableArray <DWORD> multiSampleQualityList; // List of number of quality levels for each multisample type
    CGrowableArray <UINT> presentIntervalList; // List of D3DPRESENT flags
    CGrowableArray <CD3D9EnumDSMSConflict> DSMSConflictList; // List of CD3D9EnumDSMSConflict

    CD3D9EnumAdapterInfo* pAdapterInfo;
    CD3D9EnumDeviceInfo* pDeviceInfo;
};

//--------------------------------------------------------------------------------------
// A class describing a Direct3D device that contains a
// unique supported device type
//--------------------------------------------------------------------------------------
class CD3D9EnumDeviceInfo {
public:
  ~CD3D9EnumDeviceInfo();
  UINT AdapterOrdinal;
  D3DDEVTYPE DeviceType;
  D3DCAPS9 Caps;

  // List of CD3D9EnumDeviceSettingsCombo* with a unique set 
  // of AdapterFormat, BackBufferFormat, and Windowed
  CGrowableArray <CD3D9EnumDeviceSettingsCombo*> deviceSettingsComboList;
};

//--------------------------------------------------------------------------------------
// A class describing an adapter which contains a unique adapter ordinal
// that is installed on the system
//--------------------------------------------------------------------------------------
class CD3D9EnumAdapterInfo {
public:
  ~CD3D9EnumAdapterInfo();

  UINT                   AdapterOrdinal;
  D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
  WCHAR                  szUniqueDescription[256];

  CGrowableArray<D3DDISPLAYMODE> displayModeList;     // Array of supported D3DDISPLAYMODEs
  CGrowableArray<CD3D9EnumDeviceInfo*> deviceInfoList; // Array of CD3D9EnumDeviceInfo* with unique supported DeviceTypes
};

//--------------------------------------------------------------------------------------
// Enumerates available Direct3D9 adapters, devices, modes, etc.
// Use DXUTGetD3D9Enumeration() to access global instance
//--------------------------------------------------------------------------------------
class CD3D9Enumeration {
public:

  CD3D9Enumeration(IDirect3D9 *d3d9Context, bool doEnumeration);
  ~CD3D9Enumeration();

  IDirect3DDevice9* WINAPI CreateRefDevice(HWND hWnd, bool bNullRef);

  // These should be called before Enumerate().
  //
  // Use these calls and the IsDeviceAcceptable to control the contents of
  // the enumeration object, which affects the device selection and the device settings dialog.
  void SetRequirePostPixelShaderBlending(bool bRequire)
  {
    m_bRequirePostPixelShaderBlending = bRequire;
  }
  void SetResolutionMinMax(UINT nMinWidth, UINT nMinHeight, UINT nMaxWidth, UINT nMaxHeight)
  {
    m_nMinWidth  = nMinWidth;
    m_nMinHeight = nMinHeight;
    m_nMaxWidth  = nMaxWidth;
    m_nMaxHeight = nMaxHeight;
  }
  void SetRefreshMinMax(UINT nMin, UINT nMax)
  {
    m_nRefreshMin = nMin;
    m_nRefreshMax = nMax;
  }
  void SetMultisampleQualityMax(UINT nMax)
  {
    if (nMax > 0xFFFF) {
      nMax = 0xFFFF;
    }
    m_nMultisampleQualityMax = nMax;
  }

  void GetPossibleVertexProcessingList(bool *pbSoftwareVP, bool *pbHardwareVP, bool *pbPureHarewareVP, bool *pbMixedVP)
  {
    *pbSoftwareVP     = m_bSoftwareVP;
    *pbHardwareVP     = m_bHardwareVP;
    *pbPureHarewareVP = m_bPureHarewareVP;
    *pbMixedVP        = m_bMixedVP;
  }
  void SetPossibleVertexProcessingList(bool bSoftwareVP, bool bHardwareVP, bool bPureHarewareVP, bool bMixedVP)
  {
    m_bSoftwareVP     = bSoftwareVP;
    m_bHardwareVP     = bHardwareVP;
    m_bPureHarewareVP = bPureHarewareVP;
    m_bMixedVP        = bMixedVP;
  }
  CGrowableArray<D3DFORMAT>* GetPossibleDepthStencilFormatList()
  {
    return &m_DepthStencilPossibleList;
  }
  CGrowableArray<D3DMULTISAMPLE_TYPE>* GetPossibleMultisampleTypeList()
  {
    return &m_MultiSampleTypeList;
  }
  CGrowableArray<UINT>* GetPossiblePresentIntervalList()
  {
    return &m_PresentIntervalList;
  }

  void ResetPossibleDepthStencilFormats();
  void ResetPossibleMultisampleTypeList();
  void ResetPossiblePresentIntervalList();

  // Call Enumerate() to enumerate available D3D adapters, devices, modes, etc.
  HRESULT Enumerate(
      LPAPPSTATECALLBACKISD3D9DEVICEACCEPTABLE IsD3D9DeviceAcceptableFunc = NULL,
      void *pIsD3D9DeviceAcceptableFuncUserContext = NULL
  );

  bool HasEnumerated()
  {
    return m_bHasEnumerated;
  }

  // These should be called after Enumerate() is called
  CGrowableArray<CD3D9EnumAdapterInfo *> GetAdapterInfoList()
  {
    return m_AdapterInfoList;
  }
  
  CD3D9EnumAdapterInfo *GetAdapterInfo(UINT AdapterOrdinal)
  {
    for (int i = 0; i < m_AdapterInfoList.GetSize(); i++) {
      CD3D9EnumAdapterInfo* pAdapterInfo = m_AdapterInfoList.GetAt(i);
      if (pAdapterInfo->AdapterOrdinal == AdapterOrdinal) return pAdapterInfo;
    }
    return NULL;
  }

  CD3D9EnumDeviceInfo *GetDeviceInfo(UINT AdapterOrdinal, D3DDEVTYPE DeviceType)
  {
    CD3D9EnumAdapterInfo* pAdapterInfo = GetAdapterInfo(AdapterOrdinal);
    if (pAdapterInfo) {
      for (int i = 0; i < pAdapterInfo->deviceInfoList.GetSize(); i++) {
        CD3D9EnumDeviceInfo* pDeviceInfo = (pAdapterInfo)->deviceInfoList.GetAt(i);
        if (pDeviceInfo->DeviceType == DeviceType) return pDeviceInfo;
      }
    }
    return NULL;
  }

private:
  bool m_bHasEnumerated;
  bool  m_bRequirePostPixelShaderBlending;

  IDirect3D9 *m_pD3D;

  LPAPPSTATECALLBACKISD3D9DEVICEACCEPTABLE m_IsD3D9DeviceAcceptableFunc;
  void *m_pIsD3D9DeviceAcceptableFuncUserContext;

  CGrowableArray<D3DFORMAT>              m_DepthStencilPossibleList;
  CGrowableArray<D3DMULTISAMPLE_TYPE>    m_MultiSampleTypeList;
  CGrowableArray<UINT>                   m_PresentIntervalList;
  CGrowableArray<CD3D9EnumAdapterInfo *> m_AdapterInfoList;

  bool m_bSoftwareVP;
  bool m_bHardwareVP;
  bool m_bPureHarewareVP;
  bool m_bMixedVP;

  UINT m_nMinWidth;
  UINT m_nMaxWidth;
  UINT m_nMinHeight;
  UINT m_nMaxHeight;
  UINT m_nRefreshMin;
  UINT m_nRefreshMax;
  UINT m_nMultisampleQualityMax;

  void BuildDepthStencilFormatList(CD3D9EnumDeviceSettingsCombo *pDeviceCombo);
  void BuildMultiSampleTypeList(CD3D9EnumDeviceSettingsCombo *pDeviceCombo);
  void BuildDSMSConflictList(CD3D9EnumDeviceSettingsCombo *pDeviceCombo);
  void BuildPresentIntervalList(CD3D9EnumDeviceInfo *pDeviceInfo, CD3D9EnumDeviceSettingsCombo *pDeviceCombo);

  HRESULT EnumerateDevices(CD3D9EnumAdapterInfo *pAdapterInfo, CGrowableArray<D3DFORMAT> *pAdapterFormatList);
  HRESULT EnumerateDeviceCombos(
    CD3D9EnumAdapterInfo      *pAdapterInfo,
    CD3D9EnumDeviceInfo       *pDeviceInfo,
    CGrowableArray<D3DFORMAT> *pAdapterFormatList
  );

  void ClearAdapterInfoList();
};

