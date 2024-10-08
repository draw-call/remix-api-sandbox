CURDIR    = $(MAKEDIR)
THISDIR   = $(MAKEDIR)
SHELLTYPE = CMD
MAKETYPE  = nmake

REMIX_BUILD_TYPE    = release
REMIX_API_MODE      = bridge
REMIX_SDK_TARGET    = "_Comp_$(REMIX_BUILD_TYPE)_SDK"
REMIX_DXVK_ROOT     = $(CURDIR)\dxvk-remix
REMIX_DXVK_COMMIT   = main
REMIX_BRIDGE_ROOT   = $(CURDIR)\bridge-remix
REMIX_BRIDGE_COMMIT = main

LOCAL_SOURCE_DIR    = $(CURDIR)\src
LOCAL_INCLUDE_DIR   = $(CURDIR)\include
LOCAL_GAME_DIR      = $(CURDIR)\game

!IF "$(DXSDK_DIR)" == ""
!ERROR DXSDK_DIR not defined
!ENDIF

CFLAGS   = -I "$(LOCAL_INCLUDE_DIR)" -I "$(DXSDK_DIR)\include" /Zi
CPPFLAGS = $(CFLAGS) /EHsc /std:c++20
LFLAGS   = user32.lib /link /LIBPATH:"$(DXSDK_DIR)\Lib\x86"

APP_OBJ_TARGETS = \
  $(LOCAL_SOURCE_DIR)\state.obj \
  $(LOCAL_SOURCE_DIR)\timer.obj \
  $(LOCAL_SOURCE_DIR)\window.obj \
  $(LOCAL_SOURCE_DIR)\device.obj \
  $(LOCAL_SOURCE_DIR)\dxerror.obj \
  $(LOCAL_SOURCE_DIR)\meshloader.obj \
  $(LOCAL_SOURCE_DIR)\app.obj


help:
  -@ ECHO DXVK/Bridge Remix Builder (+API,+SDK)
  -@ ECHO Usage:
  -@ ECHO   nmake build-remix        [options]
  -@ ECHO   nmake build-dxvk-remix   [options]
  -@ ECHO   nmake build-bridge-remix [options]
  -@ ECHO   nmake app
  -@ ECHO Options:
  -@ ECHO   REMIX_DXVK_COMMIT=[sha256sum]
  -@ ECHO   REMIX_BRIDGE_COMMIT=[sha256sum]

build-dxvk-remix:
  -@ ECHO [+] Sync submods
  -@ CD "$(REMIX_DXVK_ROOT)" && \
     git submodule update --init --recursive

  -@ ECHO [+] Setup outputs
  -@ IF NOT EXIST "$(REMIX_DXVK_ROOT)\$(REMIX_SDK_TARGET)" \
            MKDIR "$(REMIX_DXVK_ROOT)\$(REMIX_SDK_TARGET)"
  -@ IF EXIST "$(REMIX_DXVK_ROOT)\_output" \
       DEL /Q "$(DKVK_REMIX_ROOT)\_output"

  -@ ECHO [+] Checkout and compile
  -@ CD "$(REMIX_DXVK_ROOT)" && \
     git checkout $(REMIX_DXVK_COMMIT)
  -@ CD "$(REMIX_DXVK_ROOT)" && \
     meson setup --reconfigure --buildtype $(REMIX_BUILD_TYPE) $(REMIX_SDK_TARGET)
  -@ CD "$(REMIX_DXVK_ROOT)\$(REMIX_SDK_TARGET)" && \
     meson compile copy_sdk
  -@ CD "$(REMIX_DXVK_ROOT)\$(REMIX_SDK_TARGET)" && \
     meson compile copy_output

  -@ ECHO [+] Copy outputs
  -@ IF NOT EXIST $(LOCAL_GAME_DIR)\.trex \
            MKDIR $(LOCAL_GAME_DIR)\.trex
  -@ (Robocopy $(REMIX_DXVK_ROOT)\_output\ $(LOCAL_GAME_DIR)\.trex /E /NJH /NJS /np) ^& \
    IF %ERRORLEVEL% LEQ 4 exit /B 0

!IF "$(REMIX_API_MODE)" == "dxvk"
  -@ ECHO [+] Link SDK / API
  -@ IF EXIST "$(LOCAL_INCLUDE_DIR)\remix" RMDIR "$(LOCAL_INCLUDE_DIR)\remix"
  MKLINK /D "$(LOCAL_INCLUDE_DIR)\remix" "$(REMIX_DXVK_ROOT)\public\include\remix"
!ENDIF


build-bridge-remix:
  -@ ECHO [+] Sync submods
  -@ CD "$(REMIX_BRIDGE_ROOT)" && \
     git submodule update --init --recursive

  -@ ECHO [+] Checkout and compile
  -@ CD "$(REMIX_BRIDGE_ROOT)" && \
     git checkout $(REMIX_BRIDGE_COMMIT)
  -@ CD "$(REMIX_BRIDGE_ROOT)" && \
     powershell -command \
       "& { . .\build_bridge.ps1; Build -Platform x86 -BuildFlavour $(REMIX_BUILD_TYPE) -BuildSubDir $(REMIX_SDK_TARGET)_x86 ; exit $$LastExitCode }
  -@ CD "$(REMIX_BRIDGE_ROOT)" && \
     powershell -command \
       "& { . .\build_bridge.ps1; Build -Platform x64 -BuildFlavour $(REMIX_BUILD_TYPE) -BuildSubDir $(REMIX_SDK_TARGET)_x64 ; exit $$LastExitCode }

  -@ ECHO [+] Copy outputs
  -@ (Robocopy $(REMIX_BRIDGE_ROOT)\_output\ $(LOCAL_GAME_DIR) /E /NFL /NJH /NJS /np) ^& \
    IF %ERRORLEVEL% LEQ 4 exit /B 0

!IF "$(REMIX_API_MODE)" == "bridge"
  -@ ECHO [+] Link SDK / API
  -@ IF EXIST "$(LOCAL_INCLUDE_DIR)\remix"    RMDIR  "$(LOCAL_INCLUDE_DIR)\remix"
  -@ IF EXIST "$(LOCAL_INCLUDE_DIR)\remixapi" RMDIR  "$(LOCAL_INCLUDE_DIR)\remixapi"
  MKLINK /D "$(LOCAL_INCLUDE_DIR)\remix"    "$(REMIX_BRIDGE_ROOT)\ext\remix"
  MKLINK /D "$(LOCAL_INCLUDE_DIR)\remixapi" "$(REMIX_BRIDGE_ROOT)\public\include\remixapi"
!ENDIF


build-remix: \
  build-dxvk-remix \
  build-bridge-remix


.cpp.obj:
  -@ $(CPP) $(CPPFLAGS) /c /Fo:$@ $*.cpp

app $(LOCAL_GAME_DIR)\app.exe: $(APP_OBJ_TARGETS)
  -@ $(CPP) \
    $(CPPFLAGS) \
    /Fo:$(LOCAL_SOURCE_DIR)\$@.obj \
    /Fd:$(LOCAL_GAME_DIR)\$@.pdb   \
    /o  $(LOCAL_GAME_DIR)\$@.exe   \
    $** $(LFLAGS)


clean:
  DEL $(LOCAL_SOURCE_DIR)\*.obj
  DEL $(LOCAL_GAME_DIR)\*.exe
  DEL $(LOCAL_GAME_DIR)\*.obj
  DEL $(LOCAL_GAME_DIR)\*.pdb
  DEL $(LOCAL_GAME_DIR)\*.ilk
  DEL $(LOCAL_GAME_DIR)\*.log
  DEL $(LOCAL_GAME_DIR)\*.dxvk-cache
  DEL $(LOCAL_GAME_DIR)\metrics.txt
  DEL *.obj

.PHONY: \
  build-remix \
  build-dxvk-remix \
  build-bridge-remix \
  app \
  clean

