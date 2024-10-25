## NMake Project Wrapper
# watbulb

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
LOCAL_LIB_DIR       = $(CURDIR)\lib
LOCAL_KANAN_DIR     = $(CURDIR)\kanan

LOCAL_KANAN_LIB     = $(LOCAL_LIB_DIR)\libkanan.lib
LOCAL_MINHOOK_LIB   = $(LOCAL_LIB_DIR)\libMinHook-x86-v141-mt.lib
LOCAL_DETOURS_LIB   = $(LOCAL_LIB_DIR)\detours.lib
#NOTE: MINHOOK not used right now

!IF "$(DXSDK_DIR)" == ""
!ERROR DXSDK_DIR not defined
!ENDIF

##
# Compiler Flags
##
CFLAGS = -I "$(LOCAL_INCLUDE_DIR)" \
         -I "$(LOCAL_INCLUDE_DIR)\detours" \
         -I "$(LOCAL_KANAN_DIR)"   \
         -I "$(DXSDK_DIR)\include" \
         /Zi
CPPFLAGS = $(CFLAGS) /EHsc /std:c++20
# may want to rethink this later
DXLIBS   = d3d9.lib d3dx9.lib d3dcompiler.lib
LFLAGS   = user32.lib $(DXLIBS) /link /LIBPATH:"$(DXSDK_DIR)\Lib\x86"

##
# Primary application sources
## 
APP_OBJ_TARGETS = \
  $(LOCAL_SOURCE_DIR)\app.obj \
  $(LOCAL_SOURCE_DIR)\state.obj \
  $(LOCAL_SOURCE_DIR)\timer.obj \
  $(LOCAL_SOURCE_DIR)\window.obj \
  $(LOCAL_SOURCE_DIR)\device.obj \
  $(LOCAL_SOURCE_DIR)\dxerror.obj \
  $(LOCAL_SOURCE_DIR)\meshloader.obj \
  $(LOCAL_KANAN_LIB)

##
# Primary DLL sources
##
DLL_OBJ_TARGETS = \
  $(LOCAL_SOURCE_DIR)\dll.obj \
  $(LOCAL_SOURCE_DIR)\dxerror.obj \
  $(LOCAL_SOURCE_DIR)\skel\d3d9ex.obj \
  $(LOCAL_KANAN_LIB) \
  $(LOCAL_DETOURS_LIB)

##
# libkanan.lib sources
##
LIBKANAN_OBJ_TARGETS = \
  $(LOCAL_KANAN_DIR)\core\Memory.obj \
  $(LOCAL_KANAN_DIR)\core\Module.obj \
  $(LOCAL_KANAN_DIR)\core\Patch.obj \
  $(LOCAL_KANAN_DIR)\core\Pattern.obj \
  $(LOCAL_KANAN_DIR)\core\Scan.obj \
  $(LOCAL_KANAN_DIR)\core\String.obj \
  $(LOCAL_KANAN_DIR)\core\Utility.obj

##
# Injector sources
##
INJECTOR_OBJ_TARGETS = $(LOCAL_SOURCE_DIR)\injector.obj $(LOCAL_DETOURS_LIB)


###
# BEGIN TARGETS
##

help:
  -@ ECHO DXVK/Bridge Remix Builder (+API,+SDK)
  -@ ECHO Usage:
  -@ ECHO   nmake build-remix        [options]
  -@ ECHO   nmake build-dxvk-remix   [options]
  -@ ECHO   nmake build-bridge-remix [options]
  -@ ECHO   nmake libkanan
  -@ ECHO   nmake app
  -@ ECHO   nmake dll
  -@ ECHO   nmake clean
  -@ ECHO Options:
  -@ ECHO   REMIX_DXVK_COMMIT=[sha256sum]
  -@ ECHO   REMIX_BRIDGE_COMMIT=[sha256sum]


##
# DXVK/Bridge Remix
##
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


##
# libkanan.lib
##
libkanan: $(LOCAL_LIB_DIR)\libkanan.lib
$(LOCAL_LIB_DIR)\libkanan.lib: $(LIBKANAN_OBJ_TARGETS)
  lib /nologo /out:$@ $**


##
# Object pattern Rule
##
.cpp.obj:
  -@ $(CPP) /nologo $(CPPFLAGS) /c /Fo:$@ $*.cpp


##
# Application / DLL Build Rules
##
magos.app app: $(LOCAL_GAME_DIR)\magos.app.exe
$(LOCAL_GAME_DIR)\magos.app.exe: $(APP_OBJ_TARGETS)
  -@ $(CPP) /nologo $(CPPFLAGS) \
    /Fd:$(LOCAL_GAME_DIR)\magos.app.pdb \
    /o $@ \
    $** $(LFLAGS)

magos.dll dll: $(LOCAL_GAME_DIR)\magos.dll $(LOCAL_GAME_DIR)\magos.injector.exe
$(LOCAL_GAME_DIR)\magos.dll: $(DLL_OBJ_TARGETS)
  -@ $(CPP) /nologo $(CPPFLAGS) \
    /Fd:$(LOCAL_GAME_DIR)\magos.dll.pdb \
    $** $(LFLAGS)  \
    /DLL /OUT:$@


##
# Injector Build Rules
##
magos.injector injector: $(LOCAL_GAME_DIR)\magos.injector.exe
$(LOCAL_GAME_DIR)\magos.injector.exe: $(INJECTOR_OBJ_TARGETS)
  -@ $(CPP) /nologo $(CPPFLAGS) \
    /Fd:$(LOCAL_GAME_DIR)\magos.injector.pdb \
    /o $@ \
    $** $(LFLAGS) Advapi32.lib

##
# Util
##
clean:
  DEL $(LOCAL_SOURCE_DIR)\*.obj
  DEL $(LOCAL_SOURCE_DIR)\skel\*.obj
  DEL $(LOCAL_GAME_DIR)\*.obj
  DEL $(LOCAL_GAME_DIR)\magos.pdb
  DEL $(LOCAL_GAME_DIR)\magos.dll
  DEL $(LOCAL_GAME_DIR)\magos.app.exe
  DEL $(LOCAL_GAME_DIR)\magos.app.pdb
  DEL $(LOCAL_GAME_DIR)\magos.injector.exe
  DEL $(LOCAL_GAME_DIR)\magos.injector.pdb
  DEL $(LOCAL_GAME_DIR)\*.ilk
  DEL $(LOCAL_GAME_DIR)\*.exp
  DEL $(LOCAL_GAME_DIR)\*.log
  DEL $(LOCAL_GAME_DIR)\*.dxvk-cache
  DEL $(LOCAL_GAME_DIR)\metrics.txt
  DEL $(LOCAL_LIB_DIR)\libkanan.lib
  DEL $(LOCAL_KANAN_DIR)\core\*.obj

.PHONY: \
  build-remix \
  build-dxvk-remix \
  build-bridge-remix \
  magos.app app \
  magos.dll dll \
  magos.injector injector \
  libkanan \
  clean$(LOCAL_SOURCE_DIR)\injector.obj

