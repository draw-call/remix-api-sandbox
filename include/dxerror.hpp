#pragma once

#include <Windows.h>
#include <stdbool.h>

#define DEBUG

// ---- Prototypes ----
HRESULT DXTrace  (const WCHAR* strFile, WORD dwLine, HRESULT hr, const WCHAR* strMsg, bool bPopMsgBox);
HRESULT DXUTTrace(const CHAR* strFile, DWORD dwLine, HRESULT hr, const WCHAR* strMsg, bool bPopMsgBox);

const WCHAR* WINAPI      DXGetErrorStringW(HRESULT hr);
#define DXGetErrorString DXGetErrorStringW

VOID WINAPI DXUTOutputDebugStringW(LPCWSTR strMsg, ...);
#define DXUTOutputDebugString DXUTOutputDebugStringW

// ---- Helper definitions ----
#define DXTRACE_BUFFER_SIZE 3000

// ---- Debugging Macros ----
#if defined(DEBUG) || defined(_DEBUG)

// Value Checker Macros 
#ifndef V
#define V(x)        { hr = (x); if( FAILED(hr) ) { DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L## #x, true ); } }
#endif
#ifndef V_RETURN
#define V_RETURN(x) { hr = (x); if( FAILED(hr) ) { return DXUTTrace( __FILE__, (DWORD)__LINE__, hr, L## #x, true ); } }
#endif

// DXTrace Macros
#define DXTRACE_MSG(str)           DXTrace( __FILEW__, (DWORD)__LINE__, 0,  str, true )
#define DXTRACE_ERR(str,hr)        DXTrace( __FILEW__, (DWORD)__LINE__, hr, str, true )
#define DXTRACE_ERR_MSGBOX(str,hr) DXTrace( __FILEW__, (DWORD)__LINE__, hr, str, true )

// DXUTTrace Macros
#define DXUT_ERR(str,hr)           DXUTTrace( __FILE__, (DWORD)__LINE__, hr, str, true )
#define DXUT_ERR_MSGBOX(str,hr)    DXUTTrace( __FILE__, (DWORD)__LINE__, hr, str, true )
#define DXUTTRACE                  DXUTOutputDebugString

#else // Nullopt all above

#ifndef V
#define V(x)        { hr = (x); }
#endif
#ifndef V_RETURN
#define V_RETURN(x) { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

#define DXTRACE_MSG(str)           (0L)
#define DXTRACE_ERR(str,hr)        (hr)
#define DXTRACE_ERR_MSGBOX(str,hr) (hr)

#define DXUT_ERR(str,hr)           (hr)
#define DXUT_ERR_MSGBOX(str,hr)    (hr)
#define DXUTTRACE                  (__noop)

#endif // End Debugging Macros


// ---- Deletion Macros ----
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

// ---- Error checking macros ----
#define HRESULT_FROM_WIN32b(x) ((HRESULT)(x) <= 0 ? ((HRESULT)(x)) : ((HRESULT) (((x) & 0x0000FFFF) | (FACILITY_WIN32 << 16) | 0x80000000)))

#define CHK_ERR(hrchk, strOut) \
  case hrchk: \
    return L##strOut;

#define CHK_ERRA(hrchk) \
  case hrchk: \
    return L## #hrchk;

#define CHK_ERR_WIN32A(hrchk) \
  case HRESULT_FROM_WIN32b(hrchk): \
  case hrchk: \
    return L## #hrchk;

#define CHK_ERR_WIN32_ONLY(hrchk, strOut) \
  case HRESULT_FROM_WIN32b(hrchk): \
    return L##strOut;

