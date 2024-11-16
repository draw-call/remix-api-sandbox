#pragma once
// Hand crafted types and helpers rebuilt from a CDC PDB
// watbulb

#include <Windows.h>

#include <d3d9.h>

/*
 * 
 * 
 * EVERYTHING BELOW THIS POINT IS CDC INTERNAL TYPE INFORMATION
 * DO NOT TOUCH UNLESS YOU KNOW WHAT YOU ARE DOING
 * 
 * 
 */

#define Vector3 Vector
#define Vector4 Vector
#define Euler   Vector

// Unsupported struct aliases
#define BaseInstance void
#define Instance void
#define Level void
#define Intro void
#define TerrainGroup void
#define StreamUnit void
#define MultiSpline void
#define VMObject void
#define EventAnimInfo void
#define SoundInstanceData void
#define ObjectEffectData16 void
#define Signal void
#define SavedIntro void
#define SavedBasic void
#define Object void
#define BGInstance void
#define Terrain void
#define TerrainLight void
#define MarkUpBox void
#define IndexedFace void
#define IRenderTerrain void
#define CapturedModel void

#define PCStaticPool void
#define PCImmediateIndexBuffer void
#define PCIndexPool void
#define PCBufferManager void
#define PCEffectManager void
#define PCMaterialManager void
#define PCDeferredShadow void
#define PCDeferredShadowRenderer void
#define PCCapturedModel void
#define PCStaticIndexBuffer void
#define PCRenderModelInstance void
#define PCRenderLight void
#define PCLightResource void
#define PCDynamicTexture void
#define PCShadowMap void
#define PCDeviceCubeTexture void

#define RenderVertex void
#define RenderVertexShaded void
#define RenderSprite void
#define RenderSpritePredator void
#define RenderFSEffect void
#define ParallelLight void
#define DistanceLight void
#define IRenderLight void
#define RenderShadow void
#define ProjectedTexture void
#define MemTag void
#define IShaderLib void
#define RenderMesh void
#define RenderModelInstance void
#define ILightResource void
#define DistanceLight void
#define RenderFSEffect_WaterParams void


namespace skel
{
  namespace dtp
  {
    const struct LightProperties
    {
      int  lightType;
      float range;
      float lightIntensity;
      float umbra;
      float penumbra;
      float parallelRadius;
      bool  bDiffuseEnabled;
      unsigned __int8 lightColor_0;
      unsigned __int8 lightColor_1;
      unsigned __int8 lightColor_2;
      float attenuation[256];
      bool  bAmbientEnabled;
      unsigned __int8 ambientColor_0;
      unsigned __int8 ambientColor_1;
      unsigned __int8 ambientColor_2;
      float ambientAttenuation[256];
      float ambientPercentage;
      bool  bEnableVariation;
      bool  bRandomStartVariation;
      float intensityVariationSpeed;
      float intensityVariation[256];
      bool  bEnableModulationTexture;
      unsigned int modulationTexture;
      float texScale;
      float texRotation;
      bool  bEnableModulationTextureAnimation;
      float xModAnimDuration;
      float xModAnimCurve[256];
      float yModAnimDuration;
      float yModAnimCurve[256];
      float zModAnimDuration;
      float zModAnimCurve[256];
      bool  shadowEnabled;
      bool  softStencilShadows;
      float softStencilBias;
      float shadowMapBias;
      int   shadowMapSize;
      bool  focusOnReceivers;
      bool  animEnabled;
      float animParamsPosX[4];
      float animParamsPosY[4];
      float animParamsPosZ[4];
      float animParamsDirX[4];
      float animParamsDirY[4];
      float animParamsDirZ[4];
      void *pLightResource;
    };
  } // namespace skel::dtp


  namespace cdc
  {
    // Forward declare supported structs
    struct RenderResource;
    struct PCInternalResource;
    struct PCExternalResource;

    struct PCVertexFormat;
    struct PCVertexBufferMemPool_Bin;
    struct PCVertexBufferMemPool;
    
    struct PCScene;
    struct PCDeferredScene;
    
    struct PCSurface;
    struct PCDepthSurface;
    
    struct PCRenderTarget;
    struct PCRenderContext;
    
    struct PCDrawable;
    struct PCDrawableQueue;
    struct PCDrawableList_Node;
    struct PCDrawableList;
    
    struct PCDeviceTexture;
    struct PCDeviceBaseTexture;
    struct PCDeviceTexture;
    struct TextureMap;
    struct PCTexture;

    struct PCStateManager;

    struct PCShaderId;
    struct PCShaderBinary;
    struct PCVertexShader;
    struct PCPixelShader;
    struct PCShaderTable;
    struct PCVertexShaderTable;
    struct PCPixelShaderTable;
    struct PCShaderManager;

    struct PCLightManager_UniqueIDManager;
    struct PCLightManager_LightSet;
    struct PCLightManager_LightSetCache;
    struct PCLightManager_LightState;
    struct PCLightManager;

    struct RenderDevice;
    struct PCRenderDevice_LineBatcher;
    struct PCRenderDevice_DeviceRenderQueue;
    struct PCRenderDevice;

    struct PCDeviceManager;

    struct Camera;
    struct CameraCore;
    struct GlobalData;


#pragma region Internal

    template < typename T >
      struct Array {
        unsigned int m_size;
        unsigned int m_cap;
        union ___u2 {
          unsigned int m_tag;
          unsigned int *m_data;
        };
      };

    template < typename T, typename B >
      struct Pair {
        T first;
        B second;
      };

    struct fv4 {
      float x;
      float y;
      float z;
      float w;
    };

    struct __m128 {
      float m128_f32[4];
    };

    union fv4_m128 {
      __m128 vec128;
      fv4 __s1;
    };

    struct Vector {
      fv4_m128 ___u0;
    };

    struct Matrix {
      Vector col0;
      Vector col1;
      Vector col2;
      Vector col3;
    };

    struct LineVertex {
      float vX;
      float vY;
      float vZ;
      unsigned int nDiffuse;
    };

    struct Color {
      float r;
      float g;
      float b;
      float a;
    };

    enum PCTextureFilter: __int32 {
      PC_TF_POINT = 0x0,
      PC_TF_BILINEAR = 0x1,
      PC_TF_TRILINEAR = 0x2,
      PC_TF_ANISOTROPIC_1X = 0x3,
      PC_TF_ANISOTROPIC_2X = 0x4,
      PC_TF_ANISOTROPIC_4X = 0x6,
      PC_TF_ANISOTROPIC_8X = 0xA,
      PC_TF_ANISOTROPIC_16X = 0x12,
      PC_TF_BEST = 0x100,
      PC_TF_DEFAULT = 0x12,
      PC_TF_INVALID = 0x200,
    };

    enum PCCombinerType: __int32 {
      PC_CT_DEFAULT = 0x0,
      PC_CT_LIGHTMAP = 0x1,
      PC_CT_REFLECTION = 0x2,
      PC_CT_MASKEDREFLECTION = 0x3,
      PC_CT_STENCILREFLECTION = 0x4,
      PC_CT_DIFFUSE = 0x5,
      PC_CT_MASKEDDIFFUSE = 0x6,
      PC_CT_IMMEDIATEDRAW = 0x7,
      PC_CT_IMMEDIATEDRAW_PREDATOR = 0x8,
      PC_CT_DEPTHOFFIELD = 0x9,
      PC_CT_COUNT = 0xA,
    };

    enum PCBlendMode: __int32 {
      kPCBlendModeOpaque = 0x0,
      kPCBlendModeAlphaTest = 0x1,
      kPCBlendModeAlphaBlend = 0x2,
      kPCBlendModeAdditive = 0x3,
      kPCBlendModeSubtract = 0x4,
      kPCBlendModeDestAlpha = 0x5,
      kPCBlendModeDestAdd = 0x6,
      kPCBlendModeModulate = 0x7,
      kPCBlendModeBlend5050 = 0x8,
      kPCBlendModeDestAlphaSrcOnly = 0x9,
      kPCBlendModeColorModulate = 0xA,
      kPCBlendModeMultipassAlpha = 0xD,
      kPCBlendModeLightPassAdditive = 0x14,
    };

    enum PCPass: __int32 {
      PC_PASS_DEPTH = 0x1,
      PC_PASS_DECAL = 0x2,
      PC_PASS_SHADOW = 0x4,
      PC_PASS_TRANSLUCENT = 0x8,
      PC_PASS_OPAQUE = 0x10,
      PC_PASS_PROJECTED = 0x20,
      PC_PASS_AMBIENT = 0x40,
      PC_PASS_MULTIPASSLIGHT = 0x80,
      PC_PASS_COMPOSITE = 0x100,
      PC_PASS_PREDATOR = 0x200,
      PC_PASS_HIGHLIGHT = 0x400,
      PC_PASS_WATER = 0x800,
      PC_PASS_DEPTHTOCOLOR = 0x1000,
      PC_PASS_COUNT = 0xD,
    };

    enum PCRenderTargetType: __int32 {
      DEFAULT = 0x0,
      TEXTURE = 0x1,
      BACKBUFFER = 0x2,
    };

    enum PCDeviceBaseTexture_Type: __int32 {
      kStatic = 0x0,
      kRenderTarget = 0x1,
      kDepthSurface = 0x2,
      kDynamic = 0x3,
    };

    enum RenderTextureType: __int32 {
      TEXTURETYPE_DUMMY = 0x0,
      TEXTURETYPE_16BIT_ARGB = 0x1,
      TEXTURETYPE_16BIT_RGB = 0x2,
      TEXTURETYPE_24BIT_RGB = 0x3,
      TEXTURETYPE_32BIT_RGB = 0x4,
      TEXTURETYPE_32BIT_ARGB = 0x5,
      TEXTURETYPE_PALETTISED_16_16BIT = 0x6,
      TEXTURETYPE_PALETTISED_256_16BIT = 0x7,
      TEXTURETYPE_PALETTISED_16_32BIT = 0x8,
      TEXTURETYPE_PALETTISED_256_32BIT = 0x9,
      TEXTURETYPE_COMPRESSED_1BITALPHA = 0xA,
      TEXTURETYPE_COMPRESSED_FULLALPHA = 0xB,
    };

    enum ScreenClearMode: __int32 {
      CLS_NONE = 0x0,
      CLS_FRAMEBUFFER = 0x1,
      CLS_FRAMEBUFFER_RGB = 0x2,
      CLS_FRAMEBUFFER_ALPHA = 0x3,
      CLS_ZBUFFER = 0x4,
      CLS_ALL = 0x5,
      CLS_CLEAR_FRAMEBUFFER_SCALE_ZBUFFER = 0x6,
      CLS_SCALE_FRAMEBUFFER_CLEAR_ZBUFFER = 0x7,
      CLS_SCALE_ALL = 0x8,
    };

    const struct __declspec(align(4)) RenderFogParams {
      float m_vFogNear;
      float m_vFogFar;
      float m_vFogR;
      float m_vFogG;
      float m_vFogB;
      bool m_isFogEnabled;
    };

    const struct RenderViewport {
      float m_vX;
      float m_vY;
      float m_vWidth;
      float m_vHeight;
      float m_vZBufNear;
      float m_vZBufFar;
      float m_vNearClip;
      float m_vFarClip;
      float m_vFOV;
      float m_vAspectRatio;
      float m_vOrthoWidth;
      float m_vOrthoHeight;
      Matrix m_cViewToWorld;
      ScreenClearMode m_eClearMode;
      Color m_clearColor;
      bool m_isRenderToTexture;
      bool m_disableBloom;
      unsigned int m_nTexWidth;
      unsigned int m_nTexHeight;
      bool m_isKeepTexture;
      bool m_isRenderWaterSurface;
      Vector4 m_waterPlane;
      Vector3 m_preLitModulate;
    };

    struct __declspec(align(4)) PCViewport {
      unsigned int x;
      unsigned int y;
      unsigned int width;
      unsigned int height;
      float zNear;
      float zFar;
      bool skySphereMode;
      bool disableSkySphereMode;
    };

    const struct IrradianceSample {
      Vector3 cIntensity;
      Vector3 cDirection;
    };

    struct PCIrradianceState {
      float x[3][4];
      float y[3][4];
      float z[3][4];
      float base[4];
    };

    struct PCDistanceLightState {
      float pos[3];
      float falloff;
      float color[4];
    };

    struct TransientHeapAllocator {
      unsigned __int8 *m_pHeap;
      unsigned int m_size;
      unsigned int m_freePtr;
    };

    //// RenderResource

    struct /*VFT*/ RenderResource_vtbl {
      void(__thiscall *Release)(struct RenderResource *);
      int (__thiscall *GetMemorySize)(struct RenderResource *);
      void(__thiscall *dtor)(void);
    };
    struct RenderResource {
      RenderResource_vtbl *__vftable;
    };


    //// PCInternalResource
   struct /*VFT*/ PCInternalResource_vtbl {
      void(__thiscall *PCInternalResource)(PCInternalResource *);
      bool(__thiscall *OnCreateDevice)(struct PCInternalResource *);
      void(__thiscall *OnDestroyDevice)(struct PCInternalResource *);
    };
    struct PCInternalResource {
      PCInternalResource_vtbl *__vftable /*VFT*/ ;
      PCInternalResource *m_pPrev;
      PCInternalResource *m_pNext;
    };
 

    //// PCExternalResource


    struct /*VFT*/ PCExternalResource_vtbl {
      void(__thiscall *dtor)(void);
    };
    struct PCExternalResource {
      PCExternalResource_vtbl *__vftable /*VFT*/ ;
      unsigned int m_allocId;
      PCExternalResource *m_pNext;
      PCExternalResource *m_pPrev;
      PCRenderDevice *m_pRenderDevice;
    };


    //// PCVertex*

    struct PCVertexFormat: PCInternalResource {
      _D3DVERTEXELEMENT9 *m_pVertexElements;
      IDirect3DVertexDeclaration9 *m_pD3DVertexDeclaration;
      unsigned __int16 m_stride;
      PCVertexFormat *m_pNextFormat;
    };

    struct PCVertexBufferMemPool_Bin {
      IDirect3DVertexBuffer9 *pD3DVertexBuffer;
      unsigned __int8 *pVertexData;
      unsigned int ptr;
      PCVertexBufferMemPool_Bin *pNext;
    };

    struct PCVertexBufferMemPool: PCInternalResource {
      unsigned int m_binSize;
      PCVertexBufferMemPool_Bin *m_pActiveBin;
      PCVertexBufferMemPool_Bin **m_pVertexBins;
      unsigned int m_nSwapTotal;
      unsigned int m_nCurrentSwapIndex;
    };


    //// PCScene*


    struct /*VFT*/ PCScene_vtbl {
      unsigned int(__thiscall *GetId)(PCScene *);
      void(__thiscall *Begin)(PCScene *);
      void(__thiscall *End)(PCScene *);
    };
    struct PCScene {
      PCScene_vtbl *__vftable /*VFT*/ ;
    };

    struct __declspec(align(16)) PCDeferredScene: PCScene {
      unsigned int m_id;
      RenderViewport m_viewport;
      Matrix m_viewMatrix;
      Matrix m_projectionMatrix;
      Matrix m_screenMatrix;
      unsigned int m_fogColor;
      Matrix m_cameraMatrix;
      PCRenderDevice *m_pRenderDevice;
    };


    //// PCDepthSurface / PCSurface


    struct /*VFT*/ PCSurface_vtbl {
      IDirect3DSurface9 *(__thiscall *GetD3DSurface)(PCSurface *);
    };
    struct PCSurface {
      PCSurface_vtbl *__vftable /*VFT*/ ;
    };

    struct __declspec(align(4)) PCDepthSurface: PCSurface, PCInternalResource {
      PCInternalResource *m_pOwner;
      IDirect3DSurface9 *m_pD3DSurface;
      unsigned int m_width;
      unsigned int m_height;
      bool m_useMultiSample;
    };


    //// PCRenderTarget


    struct PCRenderTarget: PCInternalResource, PCSurface {
      unsigned int m_width;
      unsigned int m_height;
      PCRenderTargetType m_type;
      _D3DFORMAT m_format;
      bool m_useMultiSample;
      IDirect3DSurface9 *m_pD3DRenderTarget;
      PCViewport m_viewport;
    };

    //// PCRenderContext

    struct /*VFT*/ PCRenderContext_vtbl: PCInternalResource_vtbl {
      void(__thiscall *Resize)(PCRenderContext *, unsigned int, unsigned int);
      void(__thiscall *Present)(PCRenderContext *, const tagRECT *, const tagRECT *, HWND__ *);
    };
    struct PCRenderContext: PCRenderContext_vtbl {
      PCRenderTarget *m_pBackBuffer;
      PCDepthSurface *m_pDepthSurface;
      IDirect3DSwapChain9 *m_pD3DSwapChain;
      HWND__ *m_hwnd;
    };


    //// PCDrawable* 

    struct /*VFT*/ PCDrawable_vtbl {
      void(__thiscall *dtor)(void);
      void(__thiscall *Draw)(PCDrawable *, PCPass, PCDrawable *);
      bool(__thiscall *Compare)(PCDrawable *, PCPass, PCDrawable *);
      unsigned int(__thiscall *GetPasses)(PCDrawable *);
      unsigned int(__thiscall *GetPolyFlags)(PCDrawable *);
    };
    struct PCDrawable {
      PCDrawable_vtbl *__vftable /*VFT*/ ;
      PCScene *m_pScene;
      float m_sortZ;
    };

    struct /*VFT*/ PCDrawableQueue_vtbl {
      void(__thiscall *Add)(PCDrawableQueue *, PCDrawable *);
    };
    struct PCDrawableQueue {
      PCDrawableQueue_vtbl *__vftable /*VFT*/ ;
    };

    struct PCDrawableList_Node {
      PCDrawableList_Node *pNext;
      PCDrawable *pElem;
    };

    struct PCDrawableList: PCDrawableQueue {
      TransientHeapAllocator *m_allocator;
      PCDrawableList_Node *m_pHead;
      unsigned int m_size;
    };

    //// PCTexture / PCDeviceTexture / TextureMap

    struct PCDeviceBaseTexture: PCInternalResource {
      PCTextureFilter m_filter;
    };

    struct /*VFT*/ PCDeviceTexture_vtbl: PCInternalResource_vtbl {
      IDirect3DBaseTexture9 *(__thiscall *GetD3DTexture)(struct PCDeviceTexture *);
      void(__thiscall *SetFilter)(struct PCDeviceTexture *, PCTextureFilter);
      void(__thiscall *AddRef)(PCDeviceTexture *);
      void(__thiscall *Release)(PCDeviceTexture *);
    };
    struct PCDeviceTexture: PCDeviceBaseTexture, PCSurface, PCDeviceTexture_vtbl {
      unsigned int m_pitch;
      unsigned int m_width;
      unsigned int m_height;
      _D3DFORMAT m_srcFormat;
      unsigned int m_numMipMaps;
      PCDeviceBaseTexture_Type m_type;
      void *m_pTextureData;
      IDirect3DSurface9 *m_pD3DSurface;
      IDirect3DTexture9 *m_pD3DTexture;
      PCRenderTarget *m_pRenderTarget;
      PCDepthSurface *m_pDepthSurface;
    };

    struct /*VFT*/ TextureMap_vtbl {
      void(__thiscall *Release)(struct TextureMap *);
      int(__thiscall *GetMemorySize)(struct TextureMap *);
      void(__thiscall *dtor)(void);
      void *(__thiscall *PlatformTextureData)(TextureMap *);
      int(__thiscall *GetWidth)(TextureMap *);
      int(__thiscall *GetHeight)(TextureMap *);
      RenderTextureType(__thiscall *GetTextureType)(TextureMap *);
    };
    struct TextureMap: RenderResource {};

    struct PCTexture: TextureMap, PCExternalResource {
      unsigned int m_flags;
      PCDeviceTexture *m_pDeviceTexture;
    };
 #pragma endregion Internal

 #pragma region PCStateManager
    struct PCStateManager: PCInternalResource {
      IDirect3DDevice9 *m_pD3DDevice;
      bool m_bInScene;
      IDirect3DIndexBuffer9 *m_pIndexBuffer;
      PCDeviceBaseTexture *m_pTextures[16];
      PCPixelShader *m_pPixelShader;
      IDirect3DVertexBuffer9 *m_pVertexBuffer;
      IDirect3DVertexDeclaration9 *m_pVertexDeclaration;
      unsigned __int16 m_vertexStride;
      PCVertexShader *m_pVertexShader;
      PCTextureFilter m_textureFilters[16];
      unsigned int m_textureWrapMode[16];
      unsigned int m_renderStates[210];
      IDirect3DStateBlock9 *m_pStartOfFrameStateBlock;
      IDirect3DStateBlock9 *m_pCombinerStateBlocks[10];
      PCCombinerType m_currentCombinerType;
      unsigned int m_currentPolyFlags;
      unsigned int m_overrideMask;
      unsigned int m_overrideFlags;
      PCBlendMode m_blendMode;
      _D3DCULL m_cullMode;
      Matrix m_projectionMatrix;
      Matrix m_viewMatrix;
      Matrix m_viewProjectMatrix;
      Matrix m_worldMatrix;
      bool m_viewOrProjectMatrixIsDirty;
      bool m_worldMatrixIsDirty;
      bool m_isWireFrame;
      unsigned int m_fogColor;
      bool m_isFogEnabled;
    };
#pragma endregion PCStateManager

#pragma region PCShader
    const struct PCShaderId {
      unsigned __int64 hi;
      unsigned __int64 lo;
    };

    struct __declspec(align(4)) PCShaderBinary {
      const PCShaderId *m_pId;
      const unsigned int *m_pFunction;
      const bool m_isOwner;
    };

    struct PCVertexShader: PCInternalResource {
      IDirect3DVertexShader9 *m_pD3DVertexShader;
      PCShaderBinary m_binary;
      unsigned int m_refCount;
    };

    struct PCPixelShader: PCInternalResource {
      IDirect3DPixelShader9 *m_pD3DPixelShader;
      PCShaderBinary m_binary;
      unsigned int m_refCount;
    };

    struct /*VFT*/ PCShaderTable_vtbl {
      void(__thiscall *dtor)(void);
      void(__thiscall *PreWarmShader)(PCShaderTable *, unsigned int);
    };
    struct PCShaderTable {
      PCShaderTable_vtbl *__vftable;
      unsigned __int8 *m_pBasePtr;
      unsigned int *m_pOffsetTable;
      unsigned int m_tableSize;
      unsigned int m_dataSize;
    };

    struct __declspec(align(4)) PCVertexShaderTable: PCShaderTable {
      PCVertexShader **m_pVertexShaders;
      bool m_copyData;
    };

    struct __declspec(align(4)) PCPixelShaderTable: PCShaderTable {
      PCPixelShader **m_pPixelShaders;
      bool m_copyData;
    };

    struct /*VFT*/ PCShaderManager_vtbl {
      void(__thiscall *dtor)(void);
    };
    struct PCShaderManager {
      PCShaderManager_vtbl *__vftable;
      unsigned int m_numPixelShaders;
      unsigned int m_numVertexShaders;
      unsigned int m_peakPixelShaders;
      unsigned int m_peakVertexShaders;
      // SList<cdc::PCPixelShader *> m_pixelShaderList;
      PCPixelShader *m_pixelShaderList;
      // SList<cdc::PCVertexShader *> m_vertexShaderList;
      PCVertexShader *m_vertexShaderList;
    };
#pragma endregion PCShader

#pragma region PCLightManager
    struct PCLightManager_UniqueIDManager {
      unsigned __int16 nextFreeID;
    };

    const struct PCLightManager_LightSet {
      unsigned int uniqueID;
      unsigned __int8 numLights;
      unsigned __int8 numSpot;
      unsigned __int8 numPoint;
      unsigned __int8 numDirectional;
      const PCRenderLight *pLights[7];
    };

    struct __declspec(align(4)) PCLightManager_LightSetCache {
      Array < PCLightManager_LightSet
      const *> m_table[256];
      PCLightManager_UniqueIDManager m_uniqueIDs;
    };

    struct __declspec(align(4)) PCLightManager_LightState {
      const PCLightManager_LightSet *pAmbient;
      const PCLightManager_LightSet *pSinglePass;
      const PCRenderLight *pMultiPass[16];
      unsigned int numMultiPass;
      bool isValid;
    };

    // TODO VTABLE
    struct PCLightManager: PCInternalResource {
      PCRenderDevice *m_pRenderDevice;
      TransientHeapAllocator *m_frameHeap;
      Array < PCRenderLight *> m_registeredLights;
      PCLightResource *m_sceneLightResources[128];
      unsigned int m_numActiveLightResources;
      PCDynamicTexture *m_pAttenuationBuffer;
      PCRenderLight *m_pCurRenderLight;
      Array < PCRenderLight *> m_lights;
      unsigned int m_numAmbient;
      bool m_projectorsActive;
      bool m_shadowsActive;
      bool m_softStencilEnabled;
      PCLightManager_UniqueIDManager m_lightIDs;
      PCLightManager_LightSetCache m_lightSetCache;
      PCLightManager_LightState m_activeLightStates[3];
      Array < Pair < unsigned int, PCShadowMap *> > m_shadowMaps;
      Array < Pair < unsigned int, PCDeviceCubeTexture *> > m_shadowMapCubeTextures;
      Array < PCRenderTarget *> m_shadowMapRenderTargets;
      Array < PCDepthSurface *> m_shadowMapDepthSurfaces;
      bool m_isHardwareShadowMapsEnabled;
      bool m_isNullRenderTargetEnabled;
      _D3DFORMAT m_cubeShadowMapFormat;
      _D3DFORMAT m_shadowMapFormat;
    };
#pragma endregion PCLightManager

#pragma region PCRenderDevice
    struct /*VFT*/ RenderDevice_vtbl {
      void(__thiscall *dtor)(RenderDevice *);
      void(__thiscall *Release)(RenderDevice *);
      bool(__thiscall *IsReadyForBeginFrame)(RenderDevice *);
      bool(__thiscall *BeginFrame)(RenderDevice *, float);
      bool(__thiscall *EndFrame)(RenderDevice *);
      TextureMap *(__thiscall *BeginScene)(RenderDevice *, RenderViewport *);
      const RenderViewport *(__thiscall *GetCurViewport)(RenderDevice *);
      int(__thiscall *GetCurSceneIndex)(RenderDevice *);
      bool(__thiscall *EndScene)(RenderDevice *);
      bool(__thiscall *IsInScene)(RenderDevice *);
      unsigned int(__thiscall *SetFrameLock)(RenderDevice *, unsigned int);
      void(__thiscall *DumpResources)(RenderDevice *);
      void(__thiscall *WorldToScreenPos)(RenderDevice *, Vector3 *);
      void(__thiscall *DrawPrimitive)(RenderDevice *, TextureMap *, RenderVertex *, int, unsigned int, float);
      void(__thiscall *DrawPrimitiveShaded)(RenderDevice *, TextureMap *, RenderVertexShaded *, int, unsigned int, float);
      void(__thiscall *DrawPrimitive2D)(RenderDevice *, TextureMap *, RenderVertex *, int, unsigned int, float);
      void(__thiscall *DrawPrimitiveShaded2D)(RenderDevice *, TextureMap *, RenderVertexShaded *, int, unsigned int, float);
      void(__thiscall *DrawSprites)(RenderDevice *, TextureMap *, RenderSprite *, unsigned int, unsigned int, float);
      void(__thiscall *DrawSpritesPredator)(RenderDevice *, TextureMap *, RenderSpritePredator *, unsigned int, unsigned int, float);
      void(__thiscall *DrawRenderEffect)(RenderDevice *, RenderFSEffect *, float);
      void(__thiscall *DrawLineList)(RenderDevice *, LineVertex *, unsigned int, unsigned int);
      void(__thiscall *DrawLineList2D)(RenderDevice *, LineVertex *, unsigned int, unsigned int);
      bool(__thiscall *CullSphere)(RenderDevice *, const Matrix *, const Vector3 *, float);
      bool(__thiscall *CullSphereAgainstProjectionFrustrum)(RenderDevice *, const Matrix *, const Vector3 *, float);
      bool(__thiscall *CullBoxAgainstProjectionFrustrum)(RenderDevice *, const Matrix *, const Vector3 *, const Vector3 *);
      void(__thiscall *SetPortalWindow)(RenderDevice *, float, float, float, float);
      void(__thiscall *SetAmbient)(RenderDevice *, float, float, float);
      void(__thiscall *SetParallelLight)(RenderDevice *, int, const ParallelLight *);
      void(__thiscall *SetLightModulate)(RenderDevice *, float, float, float);
      void(__thiscall *SetDistanceLight)(RenderDevice *, unsigned int, const DistanceLight *);
      void(__thiscall *CalcIrradiance)(RenderDevice *, const IrradianceSample *, unsigned int, const Vector3 *, Vector3 *);
      void(__thiscall *SetIrradianceCoefficients)(RenderDevice *, Vector3 *);
      void(__thiscall *ClearLights)(RenderDevice *);
      void(__thiscall *AddLights)(RenderDevice *, IRenderLight **, unsigned int);
      void(__thiscall *BeginShadow)(RenderDevice *, const RenderShadow *);
      void(__thiscall *EndShadow)(RenderDevice *);
      void(__thiscall *BeginProjected)(RenderDevice *, const ProjectedTexture *);
      void(__thiscall *EndProjected)(RenderDevice *);
      void(__thiscall *BeginProjectedCapture)(RenderDevice *, const ProjectedTexture *);
      CapturedModel *(__thiscall *EndProjectedCapture)(RenderDevice *);
      void(__thiscall *SetBendVector)(RenderDevice *, unsigned int, Vector3 *);
      void(__thiscall *SetFogParams)(RenderDevice *, const RenderFogParams *);
      void *(__thiscall *AllocateMemory)(RenderDevice *, unsigned int, unsigned int, const MemTag *);
      void(__thiscall *FreeMemory)(RenderDevice *, void *);
      TextureMap *(__thiscall *CreateTexture)(RenderDevice *, struct TextureData *);
      IShaderLib *(__thiscall *CreateShaderLibrary)(RenderDevice *, struct ShaderLibData *);
      RenderMesh *(__thiscall *CreateRenderModel)(RenderDevice *, struct RenderMeshData *, TextureMap **, IShaderLib **, RenderMesh *);
      RenderModelInstance *(__thiscall *CreateRenderModelInstance)(RenderDevice *, const RenderMesh *);
      IRenderTerrain *(__thiscall *CreateRenderTerrain)(RenderDevice *, struct RenderTerrainData *, TextureMap **, IShaderLib **, bool);
      ILightResource *(__thiscall *CreateLightResource)(RenderDevice *, const dtp::LightProperties *);
      IRenderLight *(__thiscall *CreateLight)(RenderDevice *, ILightResource *, const Vector3 *, const Vector3 *);
      void(__thiscall *AddWaterDistortion)(RenderDevice *, const Vector4 *);
      bool(__thiscall *GetWaterSurface)(RenderDevice *, Vector3 *, Vector3 *, IRenderTerrain **);
      void(__thiscall *SetFullScreenAlpha)(RenderDevice *, float);
      void(__thiscall *SetFullScreenColor)(RenderDevice *, float, float, float);
      void(__thiscall *BlendWaterParams)(RenderDevice *, const RenderFSEffect_WaterParams *, float);
      void(__thiscall *SetDebugCamera)(RenderDevice *, const Matrix *);
      unsigned int *(__thiscall *CaptureScreen)(RenderDevice *, unsigned int *, unsigned int *);
      unsigned int(__thiscall *GetNextViewIndex)(RenderDevice *);
      unsigned int(__thiscall *GetLastViewIndex)(RenderDevice *);
    };

    struct RenderDevice {
      RenderDevice_vtbl *vftable;
    };

    struct PCRenderDevice_LineBatcher {
      LineVertex m_lineVertices[4000];
      unsigned int m_numLines;
      unsigned int m_polyFlags;
      PCRenderDevice *m_pRenderDevice;
    };

    struct PCRenderDevice_DeviceRenderQueue: PCDrawableQueue {
      PCRenderDevice *m_pRenderDevice;
    };

    struct __declspec(align(16)) PCRenderDevice: RenderDevice, PCInternalResource {
      PCRenderDevice_LineBatcher m_lineBatcher;
      PCVertexFormat *m_drawVertexFormats[4];
      bool m_isInScene;
      bool m_isInFrame;
      bool m_isFrameFailed;
      float m_frameTimeDelta;
      unsigned int m_currentTime;
      PCDeferredScene *m_pCurrentScene;
      PCDeferredScene m_backupScene;
      unsigned int m_nextSceneId;
      bool m_isOrtho;
      float m_portalMinX;
      float m_portalMinY;
      float m_portalMaxX;
      float m_portalMaxY;
      unsigned int m_viewIndex;
      unsigned int m_sceneIndex;
      float m_zNoClip;
      RenderFogParams m_fogParams;
      unsigned int m_fogColor;
      Vector4 m_fogScaleOffset;
      Vector4 m_fogColorVector;
      Color m_ambientLight;
      float m_bendVectors[64][4];
      Color m_globalModulateColor;
      PCIrradianceState m_cachedIrradianceState[2];
      PCIrradianceState *m_pCurrentIrradianceState[2];
      Vector3 m_parallelLightAmbient;
      IrradianceSample m_parallelLightSamples[3];
      bool m_updateParallelLightState;
      PCDistanceLightState m_cachedDistanceLightState[3];
      PCDistanceLightState *m_pCurrentDistanceLightState[3];
      bool m_distanceLightEnabled[3];
      PCRenderContext *m_pOverrideContext;
      PCRenderContext *m_pDefaultContext;
      PCVertexBufferMemPool *m_pVertexMemPool;
      PCStaticPool *m_pStaticPool;
      PCImmediateIndexBuffer *m_pDynamicIndexBuffer;
      PCIndexPool *m_pIndexPool;
      PCVertexShaderTable m_LegacyVST;
      PCVertexShaderTable m_immediateDrawShaderTable;
      PCVertexShaderTable m_zPassVST;
      PCVertexShaderTable m_stencilPassVST;
      PCVertexShaderTable m_shadowMapPassVST;
      PCVertexShaderTable *m_pAmbientVST;
      PCPixelShaderTable m_zPassPST;
      PCPixelShaderTable m_stencilPassPST;
      PCPixelShaderTable m_shadowMapPassPST;
      PCPixelShaderTable *m_pAmbientPST;
      PCPixelShaderTable *m_pMultiPassLightPST;
      TransientHeapAllocator m_frameHeap;
      PCExternalResource *m_pFirstResource;
      PCExternalResource *m_pLastResource;
      Array < PCExternalResource *> m_releaseList;
      PCRenderDevice_DeviceRenderQueue m_renderQueue;
      PCDrawableList m_depthDrawList;
      PCDrawableList m_ambientDrawList;
      PCDrawableList m_compositeDrawList;
      PCDrawableList m_secondPassDrawList;
      PCDrawableList m_waterDrawList;
      PCDrawableList m_opaqueDrawList;
      PCDrawableList m_decalDrawList;
      PCDrawableList m_translucentDrawList;
      PCDrawableList m_translucentPreWaterDrawList;
      PCDrawableList m_preAlphaPredatorDrawList;
      PCDrawableList m_postAlphaPredatorDrawList;
      PCBufferManager *m_pBufferManager;
      PCEffectManager *m_pEffectManager;
      bool m_isEffectsEnabled;
      PCMaterialManager *m_pMaterialManager;
      PCTexture *m_pDummyTexture;
      PCTexture *m_pDummyBlackTexture;
      PCTexture *m_pDummyNormalMap;
      PCDeferredShadow *m_pCurrentShadow;
      PCDeferredShadowRenderer *m_pShadowRenderer;
      PCCapturedModel *m_pCurrentCapturedModel;
      unsigned __int16 *m_pQuadsIndexData;
      PCStaticIndexBuffer *m_pQuadsIndexBuffer;
      PCLightManager m_lightManager;
      Array < PCRenderModelInstance *> m_instanceDataCommitList;
      Array < Vector4 > m_waterDistortions;
      bool m_isWaterSurface;
      bool m_isRenderingReflection;
      float m_curWaterDistance;
      Vector4 m_prevWaterPlane;
      Vector4 m_curWaterPlane;
      Vector3 m_prevWaterLocalPos;
      Vector3 m_curWaterLocalPos;
      PCDeviceTexture *m_pReflectionBuffer;
      Matrix m_reflectToWorld;
      IRenderTerrain *m_pWaterTerrain;
      _D3DFORMAT m_shadowMapFormat;
      _D3DFORMAT m_depthTextureFormat;
      char m_gameRoot[260];
      char m_cdcRoot[260];
    };


#pragma endregion PCRenderDevice

#pragma region PCDeviceManager
    struct PCDeviceManager_vtbl {
      void *(__thiscall *CreateRenderContext)(PCDeviceManager *, HWND__ *, unsigned int, unsigned int);
    };

    struct PCDeviceManager {
      enum Status {
        kStatusOk = 0x0,
        kStatusNotInitialized = 0xFFFFFFFF,
        kStatusDeviceLost = 0xFFFFFFFE,
        kStatusInvalidSettings = 0xFFFFFFFD,
        kStatusCreateResourceFailed = 0xFFFFFFFC,
        kStatusCreateDeviceFailed = 0xFFFFFFFB,
      };
      struct Settings {
        unsigned int adapterId;
        unsigned int fullscreenModeId;
        bool fullscreen;
        bool enableVSync;
        bool enableFSAA;
        bool enableFullscreenEffects;
        bool enableDepthOfField;
        bool useLowResDepthOfField;
        bool enableShadows;
        bool disableHardwareVP;
        bool disable32BitTextures;
        bool disableHardwareDXTC;
        bool disablePureDevice;
        bool useRefDevice;
        bool useD3DFPUPreserve;
        bool disableDriverManagement;
        bool disableDynamicTextures;
        bool disableNonPow2Textures;
        bool disableHardwareShadowMaps;
        bool disableNullRenderTargets;
        bool dontDeferShaderCreation;
        bool alwaysRenderZPassFirst;
        bool createGameFourCC;
        float brightness;
        float contrast;
        bool useShader20Features;
        bool useShader30Features;
        bool enableReflection;
        bool enableWaterFX;
        _GUID adapterIdentifier;
        PCTextureFilter bestTextureFilter;
      };
      struct AdapterInfo {
        unsigned int ordinal;
        char name[512];
        _D3DADAPTER_IDENTIFIER9 d3dAdapterId;
        unsigned int caps;
        PCTextureFilter bestTextureFilter;
        _D3DFORMAT displayFormat;
        _D3DFORMAT backBufferFormat;
        _D3DFORMAT depthStencilFormat;
        unsigned int maxMultiSampleQuality;
        _D3DTEXTUREFILTERTYPE stretchRectFilter;
        HMONITOR__ *monitor;
        void *displayModes;
        unsigned int supportedVsVer;
        unsigned int supportedPsVer;
      };

      PCDeviceManager_vtbl *__vftable;
      PCInternalResource *m_pFirstResource;
      PCInternalResource *m_pLastResource;
      int m_refCount;
      PCDeviceManager::Status m_status;
      HWND__ *m_hFocusWindow;
      HINSTANCE__ *m_pD3DLib;
      IDirect3D9 *m_pD3D;
      IDirect3DDevice9 *m_pD3DDevice;
      _D3DCAPS9 m_d3dCaps;
      bool m_bIsRecreatingResources;
      PCDeviceManager::Settings m_settings;
      bool m_isPixelShader20;
      bool m_isPixelShader30;
      _D3DPRESENT_PARAMETERS_ m_d3dPresentParams;
      bool m_hasStencil;
      _D3DDEVTYPE m_d3dDevType;
      unsigned int m_d3dBehaviorFlags;
      PCRenderContext *m_pCurrentContext;
      PCRenderTarget *m_pCurrentRenderTarget;
      IDirect3DSurface9 *m_pCurrentD3DRenderTarget;
      PCDepthSurface *m_pCurrentDepthSurface;
      IDirect3DSurface9 *m_pCurrentD3DDepthSurface;
      Array<PCRenderTarget *> m_renderTargetStack;
      Array<PCDepthSurface *> m_depthSurfaceStack;
      IDirect3DTexture9 *m_pTRLBuffer;
      PCShaderManager *m_pShaderManager;
      PCStateManager *m_pStateManager;
      Array<PCDeviceManager::AdapterInfo> m_adapters;
      const PCDeviceManager::AdapterInfo *m_pCurrentAdapter;
    };
#pragma endregion PCDeviceManager

#pragma region Camera
    struct __declspec(align(16)) CameraCore {
      Vector position;
      Vector effectPosition;
      Matrix vvNormalWorVecMatf[2];
      Matrix defaultvvNormalWorVecMatf[2];
      Matrix wcTransformf;
      Matrix wcTransformNoShakef;
      Matrix wcTransform2f;
      Matrix cwTransform2f;
      Vector viewVolumeNormal[5];
      Euler Rotation;
      Vector debugPos;
      Vector nondebugPos;
      float leftX;
      float rightX;
      float topY;
      float bottomY;
      float farPlane;
      float projDistance;
      float newProjDistance;
      float projDistanceSpd;
      float screenXRatio;
      float screenYRatio;
      float screenFullXRatio;
      float screenFullYRatio;
      float nearPlane;
      int waterBlurFlag;
      Euler debugRot;
      IndexedFace *tiltTface;
      Terrain *tiltTfaceTerrain;
    };

    struct CameraKey {
      __int16 x;
      __int16 y;
      __int16 z;
      __int16 id;
      __int16 rx;
      __int16 ry;
      __int16 rz;
      __int16 flags;
      __int16 tx;
      __int16 ty;
      __int16 tz;
      __int16 pad;
    };

    struct __declspec(align(16)) SplinecamData
    {
      cdc::Vector3 sourceStartPoint;
      cdc::Vector3 targetStartPoint;
      cdc::Vector3 thirdStartPoint;
      MultiSpline *sourceSpline;
      MultiSpline *targetSpline;
      MultiSpline *thirdSpline;
      __int16 useStartPoint;
      __int16 sourceNumber;
      __int16 targetNumber;
      __int16 thirdNumber;
    };

    struct SplinecamParams
    {
      float sourceSplineWidth;
      float targetSplineWidth;
      float sourceSplineWidthZ;
      float targetSplineWidthZ;
      float combatCamDist;
      float combatCamMinDist;
      float combatCamZOffset;
      float zoomDist;
      float cameraVelocity;
      float cameraDampening;
      float targetVelocity;
      float targetDampening;
      float combatFraming;
      float followDistance;
      float followTilt;
      float followRotation;
      float followSmooth;
      float followZOffset;
      float switchToFollowCamDist;
      float followHighTiltDistance;
      float followHighTiltAngle;
      float followMedTiltDistance;
      float followMedTiltAngle;
      float followZeroTiltDistance;
      float followZeroTiltAngle;
      float followLowTiltDistance;
      float followLowTiltAngle;
      float followVerticalBias;
      char followCamOverrideEnabled;
      char combatCamLockedOut;
      __int16 pad;
      float interestInstTune;
      Instance *interestInst;
    };

    struct FollowTransitionData
    {
      float startTilt;
      float startRotation;
      float startDistance;
      float startVerticalBias;
      float startHorizontalBias;
      float transition;
      float transitionLinear;
      float transitionVel;
      float transitionAccl;
    };

    struct __declspec(align(16)) FollowSplinecamData
    {
      cdc::Vector3 sourceStartPoint;
      cdc::Vector3 targetStartPoint;
      MultiSpline *sourceSpline;
      MultiSpline *targetSpline;
      __int16 sourceNumber;
      __int16 targetNumber;
      __int16 splinecam_currkey;
      __int8 splineInUse : 4;
      __int8 lockInPlace : 4;
      __int8 splineAdjustDist : 4;
      __int8 splineAdjustTilt : 2;
      __int8 pad : 2;
      float sourceSplineWidth;
      float targetSplineWidth;
      float sourceSplineWidthZ;
      float targetSplineWidthZ;
      float splineTune;
    };

    struct NextCamData
    {
      SplinecamData materialSplinecamData;
      SplinecamData alternateSplinecamData;
      SplinecamParams splinecamParams;
      float followCamDistance;
      float followCamTilt;
      float followCamRotation;
      float followCamSmooth;
      float ZOffset;
      float cutAngle;
      float leadAmount;
      char cutFlag;
      char lookaroundDisabled;
      __int16 crossfade;
      int Cinematic_Camera_StreamID;
    };

    struct __declspec(align(16)) Camera_data_Cinematic {
      cdc::Vector3 lastSplinePos;
      SplinecamData materialSplinecamData;
      SplinecamData alternateSplinecamData;
      SplinecamParams splinecamParams;
      char splinePointAhead;
      char lookat_spline;
      char cinema_done;
      char pad;
      int splinecam_currkey;
      int splinecam_helpkey;
      int Cinematic_Camera_StreamID;
    };

    struct __declspec(align(16)) SavedCinematic
    {
      cdc::Vector3 position;
      cdc::Vector3 focusPoint;
      cdc::Vector3 base;
      cdc::Vector3 targetPos;
      cdc::Vector3 targetFocusPoint;
      cdc::Vector3 targetBase;
      cdc::Vector3 baseRot;
      cdc::Euler focusRotation;
      cdc::Vector3 focusPointVel;
      cdc::Vector3 focusPointAccl;
      cdc::Euler targetFocusRotation;
      cdc::Vector3 baseVel;
      cdc::Vector3 baseAccl;
      float targetFocusDistance;
      float focusDistance;
      float maxVel;
      float velocityLag;
      float velocityLagVel;
      float velocityLagAccl;
      SplinecamData materialSplinecamData;
      SplinecamData alternateSplinecamData;
      SplinecamParams splinecamParams;
      int Cinematic_Camera_StreamID;
      char lookat_spline;
      Level *level;
      float smooth;
      float leadAmount;
      int splinecam_currkey;
    };

    struct $B38B72A2597991B96719C5B153BADEB4 {
      float stopTimer;
      int stopHit;
      IndexedFace *tface;
      IndexedFace *oldTFace;
      int hit;
    };

    struct __declspec(align(16)) Camera_data {
      Camera_data_Cinematic Cinematic;
      $B38B72A2597991B96719C5B153BADEB4 Follow;
    };

    struct __declspec(align(16)) Camera {
      CameraCore core;
      __int16 shake;
      __int16 shakeFrame;
      float shakeScale;
      __int16 shakeFadeIn;
      __int16 shakeFadeOut;
      int flags;
      int lock;
      __int16 mode;
      unsigned __int16 specialModeBits;
      char posState;
      char rotState;
      char tiltState;
      char distanceState;
      __int8 rotationRangePlayerRelative : 4;
      __int8 cameraUnderwater : 4;
      __int8 followCameraCombatCamDisable : 2;
      __int8 followCameraGrappleCamDisable : 2;
      __int8 followCameraLookaroundDisabled : 2;
      __int8 followCameraTimedTransition : 2;
      __int16 pad2;
      float lagZ;
      float tilt;
      float ZOffset;
      cdc::Vector3 focusPoint;
      float focusDistance;
      float followCombatCamTiltOverride;
      float followCombatCamDistOverride;
      float followTiltReturnSpeed;
      float followDragToRotateSpeed;
      float followUserControlTimeout;
      float followHighTiltDistance;
      float followHighTiltAngle;
      float followMedTiltDistance;
      float followMedTiltAngle;
      float followZeroTiltDistance;
      float followZeroTiltAngle;
      float followLowTiltDistance;
      float followLowTiltAngle;
      float followHighTiltLimit;
      float followLowTiltLimit;
      float followHighTiltFOV;
      float followHighTiltZOffset;
      float followHighTiltVerticalBias;
      float followMedTiltFOV;
      float followMedTiltZOffset;
      float followMedTiltVerticalBias;
      float followZeroTiltFOV;
      float followZeroTiltZOffset;
      float followZeroTiltVerticalBias;
      float followLowTiltFOV;
      float followLowTiltZOffset;
      float followLowTiltVerticalBias;
      float followShapeVerticalBiasOffset;
      float followShapeFOVOffset;
      float followShapeZOffsetOffset;
      float followRotationRangeStart;
      float followRotationRangeEnd;
      float followRotationRangeSoftRange;
      float followMinimumDistance;
      float followMaximumDistance;
      float followHighSpringDegree;
      float followLowSpringDegree;
      float followAutoCenterMultiplier;
      float followAutoCenterMaxSpeed;
      float followVerticalBias;
      float followDampedVerticalBias;
      float followVBiasVel;
      float followVBiasAcc;
      float followHorizontalBias;
      float followDampedHorizontalBiasRotation;
      float followHBiasVel;
      float followHBiasAcc;
      FollowTransitionData followTransitionData;
      FollowSplinecamData followSplineData;
      Instance *followInterestInst;
      float followInterestInstTune;
      Instance *focusInstance;
      cdc::Vector3 oldFocusInstancePos;
      cdc::Vector3 newFocusInstancePos;
      cdc::Euler OldFocusInstanceRot;
      cdc::Euler NewFocusInstanceRot;
      cdc::Vector3 focusInstanceVelVec;
      cdc::Vector3 focusLeadPivot;
      cdc::Vector3 focusOffset;
      cdc::Euler FocusRotation;
      float tiltVel;
      float tiltAccl;
      cdc::Vector3 positionVel;
      cdc::Vector3 positionAccl;
      cdc::Vector3 focusPointVel;
      cdc::Vector3 focusPointAccl;
      cdc::Vector3 targetPointVel;
      cdc::Vector3 targetPointAccl;
      float focusDistanceVel;
      float focusDistanceAccl;
      cdc::Vector3 focusRotVel;
      cdc::Vector3 focusRotAccl;
      float maxVel;
      float velocityLag;
      float velocityLagVel;
      float velocityLagAccl;
      cdc::Vector3 targetPos;
      float targetTilt;
      float targetFocusDistance;
      cdc::Vector3 targetFocusPoint;
      float collisionTargetFocusDistance;
      cdc::Euler TargetFocusRotation;
      float tfaceTilt;
      cdc::Euler CollisionTargetFocusRotation;
      float smooth;
      CameraKey *cameraKey;
      __int16 lookTimer;
      __int16 lookSavedMode;
      __int16 CameraLookStickyFlag;
      __int16 panic_count;
      bool bAccAimFlag;
      __int16 stack;
      __int16 targetStack;
      int savedMode[3];
      float savedTargetFocusDistance[3];
      cdc::Euler savedFocusRotation;
      float savedFocusDistance;
      float signalFocusDistance;
      SavedCinematic savedCinematic[3];
      cdc::Vector3 signalPos;
      cdc::Vector3 signalRot;
      int lookThrowMode;
      int centerTimer;
      __int16 forced_movement;
      __int16 last_forced_movement;
      __int16 rotDirection;
      cdc::Euler LookRot;
      NextCamData nextCamDeferred;
      NextCamData nextCam;
      char deferredNextCamMode;
      char restoreToSplineFlag;
      char lookaroundDisabled;
      char cutFlag;
      float cutAngle;
      cdc::Euler splinecam_helprot;
      Camera_data data;
      float teleportZRot;
      float extraZRot;
      float extraXRot;
      __int16 leftTimer;
      __int16 rightTimer;
      __int16 collision_lastPush;
      __int16 collision_timer;
      unsigned int prev_instance_mode;
      unsigned int instance_mode;
      float instance_vel;
      float instance_xyvel;
      float instance_prev_xyvel;
      float instance_zvel;
      float instanceXYVelCameraDot;
      cdc::Vector3 real_focuspoint;
      __int16 always_rotate_flag;
      __int16 lock_control_angle;
      float actual_vel_x;
      float actual_acc_x;
      float focuspoint_fallz;
      float leadAmount;
      float lead_angle;
      float lead_vel;
      float lead_accl;
      float lead_timer;
      cdc::Vector3 camera_plane;
      cdc::Vector3 camera_plane_vert;
      cdc::Vector3 left_point;
      cdc::Vector3 right_point;
      cdc::Vector3 left_point2;
      cdc::Vector3 right_point2;
      cdc::Vector3 bottom_point;
      cdc::Vector3 bottom_point2;
      cdc::Vector3 top_point;
      cdc::Vector3 top_point2;
      float ACE_amount;
      __int16 shorten_count;
      char shorten_flag;
      char camera_still;
      char globalCameraMode;
      char alternateCameraFlag;
      float Camera_lookDist;
      float combat_cam_distance;
      float combat_cam_angle;
      float combat_cam_weight;
      int combat_cam_debounce;
      float newCombatCamDist;
      float newCombatCamSpeed;
      unsigned __int16 newLookMaxRot;
      unsigned __int16 newLookMaxTilt;
      unsigned __int16 newLookMaxSpeed;
      unsigned __int16 newLookMaxAccl;
      float CameraCenterDelay;
      float CenterFlag;
      float CenterVel;
      float CenterAccl;
      float markupCenter;
      float forcedRotation;
      unsigned __int16 LockBehindMode;
      unsigned __int16 LastLockBehindMode;
      float roll_target;
      float current_roll_amount;
      float roll_inc;
      cdc::Vector3 ThrowLookPoint;
      cdc::Vector3 lookaccl;
      cdc::Vector3 lookvel;
      int matrixmode;
      int splineCamLead;
      cdc::Vector3 lastCombatPos;
      float targetTimeMult;
    };

#pragma endregion Camera

#pragma region GlobalData
    struct __declspec(align(16)) GlobalData {
      Matrix matrix0;
      Matrix matrix1;
      Vector vector0;
      Vector vector1;
      Vector vector2;
      Vector vector3;
      Vector rotation0;
      Vector rotation1;
      unsigned int versionNumber;
      Camera *camera;
      int *eventVars;
      unsigned int(__cdecl *INSTANCE_Query)(Instance *, unsigned int);
      void(__cdecl *INSTANCE_Post)(Instance *, int, int);
      void(__cdecl *INSTANCE_EventIntroduceInstance)(Intro *);
      Intro *(__cdecl *INSTANCE_FindIntro)(int, int);
      Instance *(__cdecl *INSTANCE_Find)(int);
      Level *(__cdecl *STREAM_GetLevelWithID)(int);
      TerrainGroup *(__cdecl *STREAM_GetTerrainGroup)(StreamUnit *, int);
      StreamUnit *(__cdecl *STREAM_GetStreamUnitWithID)(int);
      void(__cdecl *VM_VMObjectSetTable)(Level *, VMObject *, int);
      void(__cdecl *VM_VMObjectSetColor)(Level *, VMObject *);
      int(__cdecl *EvtSetAnimationInstanceSwitchData)(Instance *, EventAnimInfo *);
      int(__cdecl *EvtSetActionPlayHostAnimationData)(Instance *, Instance *, EventAnimInfo *);
      unsigned int(__cdecl *SCRIPT_CountFramesInSpline)(Instance *);
      void(__cdecl *EVENT_InstanceSplineSet)(Instance *, int);
      MultiSpline *(__cdecl *SCRIPT_GetMultiSpline)(Instance *);
      unsigned int(__cdecl *SCRIPT_GetSplineFrameNumber)(const MultiSpline *);
      int(__cdecl *SFXPROG_IsEventInstanceSoundLoaded)(SoundInstanceData *, unsigned int);
      void( *DBG_Print)(const char *, ...);
      void(__cdecl *SFXPROG_StartEventInstanceSound)(SoundInstanceData *, unsigned int);
      void(__cdecl *SFXPROG_StopEventInstanceSound)(SoundInstanceData *, unsigned int);
      void(__cdecl *SFXPROG_SetEventInstanceSoundVolume)(SoundInstanceData *, unsigned int, int, int);
      void(__cdecl *SFXPROG_SetEventInstanceSoundPitch)(SoundInstanceData *, unsigned int, int, int);
      void(__cdecl *SFXPROG_StartEventInstancePadshock)(SoundInstanceData *, unsigned int);
      void(__cdecl *SFXPROG_StopEventInstancePadshock)(SoundInstanceData *, unsigned int);
      void(__cdecl *SFXPROG_SetEventInstancePadshockVolume)(SoundInstanceData *, unsigned int, int, int);
      void(__cdecl *SFXPROG_SetEventInstancePadshockPitch)(SoundInstanceData *, unsigned int, int, int);
      void(__cdecl *FX_StartInstanceEffectEvent)(Instance *, ObjectEffectData16 *, Instance *, int);
      void(__cdecl *FX_StartCinematicInstanceEffectEvent)(Instance *, ObjectEffectData16 *, Instance *, int);
      int(__cdecl *SetPositionDataV4f)(Vector *);
      void(__cdecl *ScriptKillInstance)(Instance *, int);
      void(__cdecl *SAVE_UndestroyInstance)(Instance *);
      int(__cdecl *SetObjectDataI)(int, int, int);
      void(__cdecl *CAMERA_SetShake)(Camera *, int, int);
      void(__cdecl *CAMERA_SetSmoothValue)(Camera *, int);
      void(__cdecl *CAMERA_SetMode)(Camera *, int);
      void(__cdecl *CAMERA_RestoreMode)(Camera *);
      void(__cdecl *CAMERA_Adjust_distance)(Camera *, int);
      void(__cdecl *CAMERA_Adjust_tilt)(Camera *, int);
      void(__cdecl *CAMERA_Adjust_roll)(int, int);
      void(__cdecl *CAMERA_Adjust_rotation)(Camera *, int);
      void(__cdecl *CAMERA_SetInstanceFocus)(Camera *, Instance *);
      void(__cdecl *INSTANCE_Broadcast)(Instance *, int, int, int);
      void(__cdecl *PADSHOCK_Shock)(int, int, int, int);
      void(__cdecl *HINT_StartHint)(int, int, int, int);
      void(__cdecl *HINT_KillSpecificHint)(int, int, int, int);
      void(__cdecl *FX_Start_Rain)(int);
      void(__cdecl *FX_Start_Snow)(int);
      void(__cdecl *EVENT_PlayMovie)(char *);
      Matrix *(__cdecl *TRANS_RotMatrixZYXf)(Euler *, Matrix *);
      float(__cdecl *PHYSOBS_CheckObjectAxisAlignment)(Matrix *, Matrix *, Vector *);
      bool(__cdecl *HINT_CheckCurrentHint)(int, int, int, int);
      Signal *(__cdecl *SIGNAL_FindSignal)(Level *, int);
      void( *FONT_Print)(char *, ...);
      void(__cdecl *SRMUSIC_SetMusicVariable)(int, int);
      int(__cdecl *rcos)(int);
      SavedIntro *(__cdecl *SAVE_HasSavedIntro)(int);
      void(__cdecl *SAVE_DeleteBlock)(SavedBasic *);
      void(__cdecl *INSTANCE_RelativePositionInstance)(Instance *, Vector *, Vector *);
      void(__cdecl *FX_EndFXInstanceEffectsGracefully)(BaseInstance *, int);
      void(__cdecl *INSTANCE_StartFade)(Instance *);
      void(__cdecl *GXFilePrintStr)(char *);
      int(__cdecl *EVENT_FP_GreaterThan)(float, float);
      int(__cdecl *EVENT_FP_LessThan)(float, float);
      int(__cdecl *EVENT_FP_GreaterEqual)(float, float);
      int(__cdecl *EVENT_FP_LessEqual)(float, float);
      int(__cdecl *EVENT_FP_Equal)(float, float);
      int(__cdecl *EVENT_FP_NotEqual)(float, float);
      void(__cdecl *EVENT_DisplayString)(char *, int);
      int(__cdecl *EVENT_CreateSoundStream)(char *, int);
      int(__cdecl *EVENT_IsSoundStreamPlaying)(int);
      void(__cdecl *EVENT_PlaySoundStream)(int);
      void(__cdecl *EVENT_StopSoundStream)(int);
      int(__cdecl *EVENT_IsSoundStreamCueing)(int);
      int(__cdecl *EVENT_CreateCinematic)(char *, int);
      int(__cdecl *EVENT_IsCinematicPlaying)(int);
      void(__cdecl *EVENT_PlayCinematic)(int);
      void(__cdecl *EVENT_StopCinematic)(int);
      int(__cdecl *EVENT_IsCinematicCueing)(int);
      void(__cdecl *EVENT_SetCinematicPosition)(Instance *, int, int, float *);
      void(__cdecl *FX_MotionBlur)(int, int);
      void(__cdecl *EVENT_TextureMorph)(TerrainGroup *, int, int);
      void(__cdecl *EVENT_HideTerrainGroup)(TerrainGroup *, int);
      void(__cdecl *EVENT_NoCollideTerrainGroup)(TerrainGroup *, int);
      void(__cdecl *EVENT_NoDrawTerrainGroup)(TerrainGroup *, int);
      void(__cdecl *SOUND_SetMicInstance)(Instance *, int);
      void(__cdecl *INSTANCE_RelativePositionIntro)(Intro *, Vector *, Vector *);
      void(__cdecl *MUSIC_SetVolumeReduction)(int, int);
      int(__cdecl *CAMERA_CinematicDone)(Camera *);
      void(__cdecl *CAMERA_SetSpline0)(Camera *, Intro *, int, int);
      void(__cdecl *CAMERA_SetSpline1)(Camera *, Intro *, int, int);
      void(__cdecl *EVENT_ClearAlarm)(Instance *, unsigned int, unsigned int);
      void(__cdecl *EVENT_SetAlarm)(Instance *, unsigned int, unsigned int);
      void(__cdecl *EVENT_Trace)(char *, int);
      int(__cdecl *EVENT_Random)(int, int);
      void(__cdecl *EVENT_LoadMusic)(int, char *, int, int);
      int(__cdecl *G2EmulationInstanceQueryMode)(Instance *, int);
      int(__cdecl *G2EmulationInstanceQueryFrame)(Instance *, int);
      int(__cdecl *G2EmulationInstanceQueryAnimLength)(Instance *, int);
      void(__cdecl *EVENT_SetBendMode)(int, int);
      void(__cdecl *EVENT_StartWaterMove)(TerrainGroup *, StreamUnit *, int, int);
      void(__cdecl *EVENT_PlayAmbient)(char *, int);
      int(__cdecl *G2EmulationInstanceQueryAnimation)(Instance *, int);
      int(__cdecl *EVENT_LinkToParent)(BaseInstance *, int, int);
      int(__cdecl *EVENT_DynamicLinkToParent)(BaseInstance *, int, int);
      void(__cdecl *INSTANCE_UnlinkFromParent)(BaseInstance *);
      void(__cdecl *EVENT_SetCameraProjDistance)(int, int);
      int(__cdecl *EVENT_AttachStartSound)(SoundInstanceData *, unsigned int, int, int);
      int(__cdecl *EVENT_AttachEndSound)(SoundInstanceData *, unsigned int, int, int);
      void(__cdecl *GAMELOOP_SetScreenWipe)(int, int, int);
      void(__cdecl *INSTANCE_ChangeLinkNode)(Instance *, int, int);
      void(__cdecl *INSTANCE_SetCurrentStreamUnitID)(Instance *, int);
      void(__cdecl *SOUND_SetSFXReverb)(int, int);
      void(__cdecl *EVENT_SetScale)(Instance *, int, int, int);
      int(__cdecl *G2EmulationInstanceEventAnimPlaying)(Instance *, int);
      void(__cdecl *G2EmulationInstanceSetEventAnimPlaying)(Instance *, int);
      void(__cdecl *CAMERA_SetVertigo)(int, int, int, int);
      void(__cdecl *CAMERA_SetShakeNoPadshock)(Camera *, int, int);
      void(__cdecl *CAMERA_SetShakeRamp)(int, int, int, int);
      void(__cdecl *CAMERA_SetSplineWidths)(int, int, int, int);
      void(__cdecl *CAMERA_SetValueByEvent)(Camera *, int, int);
      void(__cdecl *STREAM_ResetAttackWave)(Level *, unsigned int);
      int(__cdecl *STREAM_AttackWaveBornDone)(Level *, unsigned int);
      int(__cdecl *STREAM_AttackWaveDeadDone)(Level *, unsigned int);
      void(__cdecl *FX_StartUnitEffectEvent)(Level *, Object *, ObjectEffectData16 *);
      Object *(__cdecl *OBTABLE_FindObject)(int);
      BaseInstance *(__cdecl *MINIINSTANCE_Find)(int);
      void(__cdecl *CAMERA_SetSpline2)(Camera *, Intro *, int, int);
      void(__cdecl *EVENT_BgInstanceSetDirection)(BGInstance *, int);
      void(__cdecl *EVENT_BgInstanceSetFrame)(BGInstance *, int);
      unsigned int(__cdecl *EVENT_BgInstanceGetMaxFrames)(BGInstance *);
      void(__cdecl *EVENT_BgInstanceSetMode)(BGInstance *, int);
      void(__cdecl *EVENT_BgInstanceStart)(BGInstance *, int);
      void(__cdecl *EVENT_BgInstancePlayTo)(BGInstance *, int);
      void(__cdecl *EVENT_BgInstancePlayToForceDirection)(BGInstance *, int, int);
      void(__cdecl *EVENT_BgInstanceSetClip)(BGInstance *, int, int);
      BGInstance *(__cdecl *EVENT_FindBgInstance)(Terrain *, unsigned int);
      void(__cdecl *STREAM_TurnOnVMarkerID)(Level *, unsigned int);
      void(__cdecl *STREAM_TurnOffVMarkerID)(Level *, unsigned int);
      int(__cdecl *STREAM_IsVMarkerOn)(Level *, unsigned int);
      int(__cdecl *STREAM_AttackWaveNumBorn)(Level *, unsigned int);
      int(__cdecl *STREAM_AttackWaveNumDead)(Level *, unsigned int);
      void(__cdecl *EVENT_GUIBlockTrace)(unsigned int, unsigned int, unsigned int, unsigned int);
      void(__cdecl *EVENT_GUILineTrace)(unsigned int, unsigned int);
      void(__cdecl *EVENT_GUIVarTrace)(unsigned int, unsigned int);
      void(__cdecl *STREAM_TurnOnAttackWave)(Level *, unsigned int);
      void(__cdecl *STREAM_TurnOffAttackWave)(Level *, unsigned int);
      int(__cdecl *STREAM_IsAttackWaveOn)(Level *, unsigned int);
      int(__cdecl *CAMERA_SourceNumber)(Camera *);
      int(__cdecl *EVENT_InputActionOn)(int);
      int(__cdecl *EVENT_InputActionPressed)(int);
      void(__cdecl *STREAM_KillAttackWaveMonsters)(Level *, unsigned int);
      void(__cdecl *STREAM_SetAttackWaveDone)(Level *, unsigned int);
      int(__cdecl *EVENT_BGLinkToParent)(BGInstance *, int, int);
      int(__cdecl *EVENT_BGDynamicLinkToParent)(BGInstance *, int, int);
      void(__cdecl *BGOBJECT_UnlinkFromParent)(BGInstance *);
      void(__cdecl *STREAM_KillAttackWaveGroupMonsters)(Level *, unsigned int);
      void(__cdecl *STREAM_ResetAttackWaveGroup)(Level *, unsigned int);
      void(__cdecl *STREAM_SetAttackWaveGroupDone)(Level *, unsigned int);
      int(__cdecl *STREAM_AttackWaveGroupBornDone)(Level *, unsigned int);
      int(__cdecl *STREAM_AttackWaveGroupDeadDone)(Level *, unsigned int);
      void(__cdecl *STREAM_TurnOnAttackWaveGroup)(Level *, unsigned int);
      void(__cdecl *STREAM_TurnOffAttackWaveGroup)(Level *, unsigned int);
      int(__cdecl *STREAM_IsAttackWaveGroupOn)(Level *, unsigned int);
      int(__cdecl *STREAM_AttackWaveGroupNumBorn)(Level *, unsigned int);
      int(__cdecl *STREAM_AttackWaveGroupNumDead)(Level *, unsigned int);
      void(__cdecl *EVENT_SetSunPosition)(int, int);
      void(__cdecl *EVENT_InstanceSetPiece)(Instance *, int);
      void(__cdecl *EVENT_AttackWaveSetPiece)(Level *, unsigned int, int);
      void(__cdecl *EVENT_AttackWaveGroupSetPiece)(Level *, unsigned int, int);
      void(__cdecl *EVENT_InstanceSetDirection)(Instance *, int);
      void(__cdecl *EVENT_InstanceSetFrame)(Instance *, int);
      unsigned int(__cdecl *EVENT_InstanceGetMaxFrames)(Instance *);
      void(__cdecl *EVENT_InstanceSetMode)(Instance *, int);
      void(__cdecl *EVENT_InstanceStart)(Instance *, int);
      void(__cdecl *EVENT_InstancePlayTo)(Instance *, int);
      void(__cdecl *EVENT_InstancePlayToForceDirection)(Instance *, int, int);
      void(__cdecl *EVENT_InstanceSetClip)(Instance *, int, int);
      void(__cdecl *EVENT_SwitchPlayerCharacter)(char *);
      void(__cdecl *EVENT_RestorePlayerCharacter)(char *);
      void(__cdecl *STREAM_SetAttackWaveVar)(int);
      int(__cdecl *STREAM_GetAttackWaveVar)();
      void(__cdecl *CAMERA_SetSplineCameraFromLevelData)(Camera *, Level *, int);
      void(__cdecl *HINT_StartModalHint)(int, int, int, int);
      void(__cdecl *HINT_StartQueryHint)(int, int, int, int);
      int(__cdecl *HINT_GetQueryHintResult)(int, int, int, int);
      void(__cdecl *FX_StartInstanceEffectHostedEvent)(Instance *, Object *, ObjectEffectData16 *);
      void(__cdecl *FX_StartCinematicInstanceEffectHostedEvent)(Instance *, Object *, ObjectEffectData16 *);
      void(__cdecl *CHRONICLE_SwitchChapter)(char *);
      int(__cdecl *CHRONICLE_CheckChapter)(char *);
      void(__cdecl *CHRONICLE_SetLoadoutChapter)(int);
      int(__cdecl *EVENT_IsSoundStreamPastPosition)(int, int);
      void(__cdecl *EVENT_CueMusic)(int, char *, int);
      int(__cdecl *LOAD_AreAnyLoadsPending)();
      void(__cdecl *EVENT_SetTimeAdjustment)(int, int);
      void(__cdecl *CHRONICLE_SetChapterVars)(int);
      void(__cdecl *EVENT_LoadLevel)(char *);
      StreamUnit *(__cdecl *STREAM_GetStreamUnitWithName)(char *);
      void(__cdecl *EVENT_SetWarpUnit)(char *);
      void(__cdecl *EVENT_SetWarpDestinationUnit)(char *);
      void(__cdecl *EVENT_LoadWarpUnit)();
      void(__cdecl *EVENT_LoadWarpDestinationUnit)();
      void(__cdecl *EVENT_SetWarpUnitNoDump)();
      void(__cdecl *EVENT_ResetWarpUnitNoDump)();
      void(__cdecl *EVENT_SetWarpDestinationUnitNoDump)();
      void(__cdecl *EVENT_ResetWarpDestinationUnitNoDump)();
      void(__cdecl *EVENT_LoadAdjacentUnit)(char *);
      void(__cdecl *EVENT_ResetAdjacentUnit)(char *);
      bool(__cdecl *EVENT_IsAdjacentUnitLoaded)(char *);
      void(__cdecl *EVENT_MoveToUnit)(char *);
      void(__cdecl *EVENT_MoveSunPosition)(int, int, int);
      void(__cdecl *EVENT_PostDialogKillPlayer)(int);
      void(__cdecl *EVENT_SetGoal)(int);
      void(__cdecl *EVENT_SetSubGoal1)(int);
      void(__cdecl *EVENT_SetSubGoal2)(int);
      void(__cdecl *EVENT_SetSubGoal3)(int);
      void(__cdecl *EVENT_SetSubGoal4)(int);
      void(__cdecl *EVENT_SetSubGoal5)(int);
      void(__cdecl *EVENT_SetSubGoal6)(int);
      void(__cdecl *EVENT_SetSubGoal7)(int);
      void(__cdecl *EVENT_SetSubGoal8)(int);
      void(__cdecl *EVENT_SetSubGoal9)(int);
      void(__cdecl *EVENT_TrackDialog)(int);
      void(__cdecl *EVENT_SetGoalCompleted)(bool);
      void(__cdecl *EVENT_SetSubGoal1Completed)(bool);
      void(__cdecl *EVENT_SetSubGoal2Completed)(bool);
      void(__cdecl *EVENT_SetSubGoal3Completed)(bool);
      void(__cdecl *EVENT_SetSubGoal4Completed)(bool);
      void(__cdecl *EVENT_SetSubGoal5Completed)(bool);
      void(__cdecl *EVENT_SetSubGoal6Completed)(bool);
      void(__cdecl *EVENT_SetSubGoal7Completed)(bool);
      void(__cdecl *EVENT_SetSubGoal8Completed)(bool);
      void(__cdecl *EVENT_SetSubGoal9Completed)(bool);
      void(__cdecl *EVENT_OpenUIScreen)(const char *);
      void(__cdecl *EVENT_SetMapRoomNumber)(int);
      void(__cdecl *CHRONICLE_SetGoalPoint)(int);
      int(__cdecl *CHRONICLE_GetGoalPoint)();
      int(__cdecl *STREAM_AttackWaveDeadDoneLastFrame)(Level *, unsigned int);
      int(__cdecl *STREAM_AttackWaveGroupDeadDoneLastFrame)(Level *, unsigned int);
      void(__cdecl *CHRONICLE_MarkCinematicViewed)(unsigned int);
      int(__cdecl *CHRONICLE_GetCurrentDarkChronicle)();
      void(__cdecl *HINT_StartImportantHint)(int, int, int, int);
      void(__cdecl *EVENT_RigidBodyImpulseExplosion)(Instance *, int, int);
      int(__cdecl *SRMUSIC_GetMusicVariable)(int);
      void(__cdecl *INSTANCE_SetModel)(Instance *, int);
      VMObject *(__cdecl *EVENT_FindVMObject)(StreamUnit *, int);
      int(__cdecl *EVENT_GetVMObjectIndex)(VMObject *);
      void(__cdecl *EVENT_RigidBodyThrow)(Instance *, int);
      void(__cdecl *EVENT_RigidBodyReset)();
      void(__cdecl *EVENT_RigidBodyThrowRandom)();
      void(__cdecl *EVENT_PrintScalarExpression)(int, int);
      void(__cdecl *EVENT_ShowHudDrawGroup)(int);
      void(__cdecl *EVENT_HideHudDrawGroup)(int);
      int(__cdecl *EVENT_NumBonusPackages)(int);
      bool(__cdecl *EVENT_BonusPackageJustPickedUp)(int);
      int(__cdecl *SetMarkerData)(int, int, int, int);
      int(__cdecl *SetLookAtData)(int, int, int, int);
      void(__cdecl *STREAM_AddAttackWaveMovePosLimit)(int, int, int);
      void(__cdecl *STREAM_AddAttackWavePlaneMovePosLimit)(int, int, int);
      void(__cdecl *STREAM_ClearAttackWaveMovePosLimits)(int, int);
      int(__cdecl *SetMonsterSetPieceData)(int, int, int, int);
      void(__cdecl *INSTANCE_HideUnhideDrawGroup)(Instance *, int, int);
      void(__cdecl *INSTANCE_HideAllDrawGroups)(Instance *);
      int(__cdecl *SetModifiableStimulusData)(unsigned __int16, unsigned int, int);
      int(__cdecl *SetWaterLevelData)(int, int, int);
      int(__cdecl *SetWaterCurrentData)(int, int, int);
      int(__cdecl *SetElectrificationData)(__int16, __int16);
      int(__cdecl *SetMonsterNoticeData)(Instance *, int);
      void(__cdecl *EVENT_SetPostFilterFXParam)(int, int, int);
      void(__cdecl *EVENT_SpecifyPostFilterFXPalette)(int, int);
      void(__cdecl *EVENT_SetPostFilterFXPalette)(int, int, int, int);
      void(__cdecl *EVENT_SetPostFilterFXColorParam)(int, int, int, int);
      void(__cdecl *EVENT_ResetPostFilterFX)();
      int(__cdecl *EVENT_RigidBodyQuery)(Instance *, unsigned int, unsigned int);
      int(__cdecl *ENEMY_SetPuppetGoalData)(int, int, int, int);
      MarkUpBox *(__cdecl *MARKUP_Find)(int, int);
      void(__cdecl *EVENT_DisplayStringXY)(char *, int, int, int);
      void(__cdecl *ENEMY_PlaceNavigationBlocker)(int, int);
      void(__cdecl *ENEMY_RemoveNavigationBlocker)(int, int);
      void(__cdecl *EVENT_ApplyForceFromUnitMarker)(Instance *, Intro *, int);
      void(__cdecl *EVENT_ApplyForceFromModelMarker)(Instance *, int, int);
      void(__cdecl *EVENT_ApplyTorqueFromUnitMarker)(Instance *, Intro *, int);
      void(__cdecl *EVENT_ApplyTorqueFromModelMarker)(Instance *, int, int);
      void(__cdecl *EVENT_ApplyForceFromUnitMarkerToModelMarker)(Instance *, Intro *, int, int);
      void(__cdecl *EVENT_ApplyForceFromModelMarkerToModelMarker)(Instance *, int, int, int);
      void(__cdecl *EVENT_ApplyImpulseFromUnitMarker)(Instance *, Intro *, int);
      void(__cdecl *EVENT_ApplyImpulseFromModelMarker)(Instance *, int, int);
      void(__cdecl *EVENT_ApplyImpulseFromUnitMarkerToModelMarker)(Instance *, Intro *, int, int);
      void(__cdecl *EVENT_ApplyImpulseFromModelMarkerToModelMarker)(Instance *, int, int, int);
      void(__cdecl *EVENT_AtomApplyForceFromUnitMarker)(Instance *, int, Intro *, int);
      void(__cdecl *EVENT_AtomApplyForceFromModelMarker)(Instance *, int, int, int);
      void(__cdecl *EVENT_AtomApplyTorqueFromUnitMarker)(Instance *, int, Intro *, int);
      void(__cdecl *EVENT_AtomApplyTorqueFromModelMarker)(Instance *, int, int, int);
      void(__cdecl *EVENT_AtomApplyForceFromUnitMarkerToModelMarker)(int *);
      void(__cdecl *EVENT_AtomApplyForceFromModelMarkerToModelMarker)(int *);
      void(__cdecl *EVENT_AtomApplyImpulseFromUnitMarker)(Instance *, int, Intro *, int);
      void(__cdecl *EVENT_AtomApplyImpulseFromModelMarker)(Instance *, int, int, int);
      void(__cdecl *EVENT_AtomApplyImpulseFromUnitMarkerToModelMarker)(int *);
      void(__cdecl *EVENT_AtomApplyImpulseFromModelMarkerToModelMarker)(int *);
      void(__cdecl *EVENT_ApplyCentralForceTowardsModelMarker)(int *);
      int(__cdecl *EVENT_SegmentVsSegmentSphereCheck)(int *);
      int(__cdecl *EVENT_SegmentVsSegmentBoxCheck)(int *);
      int(__cdecl *EVENT_SegmentVsModelmarkerSphereCheck)(int *);
      int(__cdecl *EVENT_segmentVsModelmarkerBoxCheck)(int *);
      int(__cdecl *EVENT_StartBlockingCine)(char *, bool);
      int(__cdecl *EVENT_StartAmbientCine)(char *, bool);
      int(__cdecl *EVENT_StopCine)(char *);
      int(__cdecl *EVENT_StopAllCine)();
      void(__cdecl *EVENT_ClearCameraStack)();
      int(__cdecl *SAVE_IsUniqueIDDeadDead)(int);
      void(__cdecl *EVENT_SignalAction)(Signal *, Level *, int, int);
      int(__cdecl *SetPositionAtData)(Instance *, int, int, int, int, float);
      void(__cdecl *EVENT_DetachActorFromCineSpline)(char *, Instance *);
      void(__cdecl *INSTANCE_PlaySoundStreamFromEvent)(Instance *, int, int, int);
      unsigned int(__cdecl *EVENT_CreateSound)(unsigned int, int);
      void(__cdecl *EVENT_PlaySound)(unsigned int, Instance *);
      void(__cdecl *EVENT_StopSound)(unsigned int);
      int(__cdecl *EVENT_IsSoundPlaying)(unsigned int);
      void(__cdecl *SFXPROG_StartInstancePeriodicSound)(SoundInstanceData *, unsigned int);
      void(__cdecl *SFXPROG_StopInstancePeriodicSound)(SoundInstanceData *, unsigned int);
      void(__cdecl *EVENT_TestCommand1)(Instance *, int);
      void(__cdecl *EVENT_TestCommand2)(Instance *, Instance *);
      void(__cdecl *SFXPROG_StartInstanceEventOrPeriodicSound)(SoundInstanceData *, unsigned int);
      void(__cdecl *SFXPROG_StopInstanceEventOrPeriodicSound)(SoundInstanceData *, unsigned int);
      void(__cdecl *TRACKSTREAM_SetNextStreamSequence)(const char *);
      bool(__cdecl *TRACKSTREAM_IsStreamSequence)(const char *);
      bool(__cdecl *TRACKSTREAM_IsNextUnit)(const char *);
      void(__cdecl *EVENT_SpawnBiker)(Instance *, void *);
      void(__cdecl *EVENT_SpawnJeep)(Instance *, void *, int *);
      void(__cdecl *EVENT_SpawnHelicopter)(Instance *, void *);
      int(__cdecl *EVENT_PreloadCine)(char *);
      void(__cdecl *STREAM_SendMessageToAttackWaveInstances)(Level *, unsigned int, int, int, int);
      int(__cdecl *STREAM_EnemyDiedInPMarker)(Level *, unsigned int);
      TerrainLight *(__cdecl *LIGHT_Find)(int, int);
      void(__cdecl *VEHMOVEMENT_SetLevelProgress)(int, int);
      void(__cdecl *VEHMOVEMENT_AddLevelProgress)(int, int);
      bool(__cdecl *VEHMOVEMENT_IsDirectionForward)();
      void(__cdecl *VEHMOVEMENT_SetActiveDirectionControl)(const char *);
      void(__cdecl *VEHMOVEMENT_SetReverseAngle)(int);
      void(__cdecl *VEHMOVEMENT_SetDirectionHandled)();
      int(__cdecl *VEHMOVEMENT_GetDirectionLevel)();
      void(__cdecl *VEHMOVEMENT_SetActiveSpeedControl)(const char *);
      void(__cdecl *VEHMOVEMENT_SetSpeedHandled)();
      int(__cdecl *VEHMOVEMENT_GetSpeedLevel)();
      void(__cdecl *VEHMOVEMENT_EnterVehicleMode)();
      void(__cdecl *VEHMOVEMENT_ExitVehicleMode)();
      int(__cdecl *VEHMOVEMENT_GetRelativeSpeed)();
      void(__cdecl *VEHMOVEMENT_SetRelativeSpeed)(int);
      void(__cdecl *EVENT_NoBarks)(int);
      void(__cdecl *EVENT_StartTimeTrialTimer)(float);
      float(__cdecl *EVENT_TimeTrialTime)();
      int(__cdecl *EVENT_IsCinePlaying)(const char *);
      void(__cdecl *STREAM_ClearMessagesToAttackWaveInstances)(Level *, unsigned int);
      void(__cdecl *STREAM_AddAttackWaveRunAndGunPos)(int, int, int);
      void(__cdecl *STREAM_ClearAttackWaveRunAndGunPos)(int, int);
      int(__cdecl *STREAM_DoesAnyAttackWaveAttackerQueryReturnNonZero)(Level *, unsigned int, unsigned int);
      int(__cdecl *EVENT_IsBlockingCinePlaying)();
      void(__cdecl *EVENT_SpawnMeatTurret)(Instance *, void *, int *);
      void(__cdecl *EVENT_SpawnMeatTurretGround)(const char *, int, int, int *);
      bool(__cdecl *EVENT_SpawnEnemyIsDead)(int);
      bool(__cdecl *EVENT_GetPMarkerPos)(Vector *, int, int);
      void(__cdecl *STREAM_AddAttackWavePatrolPos)(int, int, int);
      void(__cdecl *STREAM_ClearAttackWavePatrolPos)(int, int);
      void(__cdecl *STREAM_SetAttackWavePatrolType)(int, int, int);
      void(__cdecl *EVENT_SoundGroupSetVolume)(int, int, int);
      void(__cdecl *EVENT_SoundGroupSetPitch)(int, int, int);
      void(__cdecl *EVENT_SoundGroupChangeVolume)(int, int, int);
      void(__cdecl *EVENT_SoundGroupChangePitch)(int, int, int);
      int(__cdecl *PlayerVsPlane)(Instance *, int);
      int(__cdecl *EVENT_Sqrt)(int);
      int(__cdecl *EVENT_Dist3)(Vector *, Vector *);
      int(__cdecl *EVENT_GetDistancePPToPP)(Instance *, Instance *);
      int(__cdecl *EVENT_GetDistanceMMToMM)(int *);
      int(__cdecl *EVENT_GetDistancePPToMM)(Instance *, Instance *, int);
      int(__cdecl *EVENT_GetDistancePPiToMM)(Intro *, Instance *, int);
      void(__cdecl *EVENT_GetPosFromModelMarker)(Vector *, Instance *, int);
      int(__cdecl *EVENT_FP_to_Int32)(float);
      void(__cdecl *EVENT_EnableBinocularsHUD)(int);
      void(__cdecl *EVENT_WetDirtyCommand)(int, int);
      bool(__cdecl *CDCCine_IsCinematicPlaying)();
      void(__cdecl *EVENT_DoDamageWithType)(int, int);
      int(__cdecl *EVENT_InputActionImageIndex)(int);
      void(__cdecl *EVENT_Reward)(int);
      void(__cdecl *EVENT_UIFadeGroupTrigger)(int);
      int(__cdecl *EVENT_GetRelativeZRotFromInstanceToInstance)(Instance *, Instance *);
      int(__cdecl *EVENT_GetRelativeXRotFromInstanceToInstance)(Instance *, Instance *);
      void(__cdecl *SOUND_SetMicDistance)(int, int);
      int(__cdecl *EVENT_GetInstanceZRotation)(Instance *);
      int(__cdecl *EVENT_QueryPlayerEvent)(int);
      int(__cdecl *EVENT_QueryPlayerEventory)(int);
      int(__cdecl *EVENT_PlayerBulletHitPosVsIntroSphereCheck)(int, int);
      void(__cdecl *EVENT_PlayerCopyToBackupInventory)();
      void(__cdecl *EVENT_PlayerCopyFromBackupInventory)();
      void(__cdecl *EVENT_SetSunShadowIntensity)(int);
      void(__cdecl *EVENT_PlayerEnableControl)();
      void(__cdecl *EVENT_PlayerDisableControl)();
      void(__cdecl *EVENT_SoundStreamForceSubtitle)(int, bool);
      void(__cdecl *EVENT_CinematicForceSubtitle)(bool);
      void(__cdecl *ENEMY_StopAnyBark)();
      void(__cdecl *HUD_ShowControllerImage)(bool);
      void(__cdecl *MARKUP_FindDebug)(int, int);
      void(__cdecl *EVENT_PlayerEnableDeathScream)(bool);
      int(__cdecl *EVENT_PlayerStateID)();
      bool(__cdecl *EVENT_PlayerInDeathState)();
      int(__cdecl *EVENT_DebugInputActionOn)(int);
      int(__cdecl *EVENT_DebugInputActionPressed)(int);
      bool(__cdecl *EVENT_IsReplayingLevel)();
      void(__cdecl *EVENT_InSuperActionCine)(bool);
    };

#pragma endregion GlobalData

  } // namespace skel::cdc
} // namespace skel