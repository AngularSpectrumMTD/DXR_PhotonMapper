#pragma once

#include "AppBase.h"
#include "Camera.h"
#include "utility/Utility.h"
#include <DirectXMath.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <d3d12.h>
#include <dxgi1_6.h>

#include "d3dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#define BITONIC_BLOCK_SIZE 1024
#define TRANSPOSE_BLOCK_SIZE 16

#define BITONIC2_THREAD_NUM 64

#define GRID_SORT_THREAD_NUM 1024
#define GRID_DIMENSION 256//if increase this param, grid lines are occured in caustics. (Cause : Photon Accumuration)
#define PLANE_SIZE 40

#define PHOTON_NUM_1D 1024
#define DENOISE_ITE 2
#define MAX_RECURSION_DEPTH 31//0---31

namespace HitGroups {
    static const wchar_t* ReflectReflactMaterialSphere = L"hgReflectReflactSpheres";
    static const wchar_t* ReflectReflactMaterialBox = L"hgReflectReflactBoxes";
    static const wchar_t* PhongMaterialSphere = L"hgPhongSpheres";
    static const wchar_t* PhongMaterialBox = L"hgPhongBoxes";
    static const wchar_t* Floor = L"hgFloor";
    static const wchar_t* Glass = L"hgGlass";
    static const wchar_t* Metal = L"hgMetal";
    static const wchar_t* Light = L"hgLight";
}

namespace ComputeShaders {
    const LPCWSTR BitonicSort = L"BitonicSort_BitonicSort.cso";
    const LPCWSTR Transpose = L"BitonicSort_Transpose.cso";

    const LPCWSTR BitonicSort2 = L"BitonicSort.cso";

    const LPCWSTR BuildGrid = L"Grid3D_BuildGrid.cso";
    const LPCWSTR BuildGridIndices = L"Grid3D_BuildGridIndices.cso";
    const LPCWSTR ClearGridIndices = L"Grid3D_ClearGridIndices.cso";
    const LPCWSTR Copy = L"Grid3D_Copy.cso";
    const LPCWSTR RearrangePhoton = L"Grid3D_RearrangePhoton.cso";

    const LPCWSTR Denoise = L"Denoise.cso";
}

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class DxrPhotonMapper : public AppBase {
public:
    DxrPhotonMapper(u32 width, u32 height);

    void Initialize() override;
    void Terminate() override;

    void Update() override;
    void Draw() override;

    void OnMouseDown(MouseButton button, s32 x, s32 y) override;
    void OnMouseUp(MouseButton button, s32 x, s32 y) override;
    void OnMouseMove(s32 dx, s32 dy) override;
    void OnKeyDown(UINT8 wparam) override;
    void OnMouseWheel(s32 rotate) override;

private:

    enum SphereTypeCount {
        NormalSpheres = 5,
        ReflectSpheres = 1,
        RefractSpheres = 1,
        SpheresAll = (NormalSpheres + ReflectSpheres + RefractSpheres),
    };

    enum GlassTypeCount {
        NormalGlasses = 1
    };

    enum MetalTypeCount {
        NormalMetals = 1
    };

    enum BoxTypeCount {
        NormalBoxes = 1,
        ReflectBoxes = 1,
        RefractBoxes = 1,
        BoxesAll = (NormalBoxes + ReflectBoxes + RefractBoxes),
    };

    enum LightTypeCount {
        NormalLight = 1
    };

    struct MaterialParam
    {
        XMVECTOR albedo;
        XMVECTOR specular;
    };

    struct SceneParam
    {
        XMMATRIX mtxView;
        XMMATRIX mtxProj;
        XMMATRIX mtxViewInv;
        XMMATRIX mtxProjInv;
        XMVECTOR lightColor;
        XMVECTOR backgroundColor;
        XMVECTOR spotLightPosition;
        XMVECTOR spotLightDirection;
        XMUINT4 flags;
        XMFLOAT4 photonParams;//x unused
        XMVECTOR cameraParams;//near far reserved reserved
        XMVECTOR gatherParams;
        XMVECTOR gatherParams2;
        XMVECTOR spotLightParams;
        XMVECTOR viewVec;
    };

    struct PhotonInfo
    {
        XMFLOAT3 throughput;
        XMFLOAT3 position;
        XMFLOAT3 inDir;
    };

    struct BitonicSortCB
    {
        u32 mLevel;
        u32 mLevelMask;
        u32 mWidth;
        u32 mHeight;

        void set(u32 level, u32 levelMask, u32 width, u32 height)
        {
            mLevel = level;
            mLevelMask = levelMask;
            mWidth = width;
            mHeight = height;
        }
    };

    struct BitonicSortCB2
    {
        s32 inc;
        s32 dir;
    };

    struct GridCB
    {
        s32 numPhotons;
        XMFLOAT3 gridDimensions;
        f32 gridH;
        f32 photonExistRange;
    };

    struct DenoiseCB
    {
        u32 stepScale;
        f32 sigmaScale;
        f32 clrSigma;
        f32 nmlSigma;
        f32 posSigma;
    };

    enum ModelType
    {
        ModelType_Crab,
        ModelType_TwistCube,
        ModelType_Teapot,
        ModelType_LikeWater,
        ModelType_Ocean,
        ModelType_Ocean2,
        ModelType_Diamond,
        ModelType_Skull
    };

    enum Spectrum
    {
        Spectrum_D65,
        Spectrum_Stabdard,
        Spectrum_Fluorescent,
        Spectrum_IncandescentBulb,
        Spectrum_Count,
    };

    enum CausticsQuality
    {
        CausticsQuality_LOW = 256,
        CausticsQuality_MIDDLE = 512,
        CausticsQuality_HIGH = 1024
    };

    void InitializeCamera();

    void SetupMeshMaterialAndPos();
    void CreateSceneBLAS();
    void CreateSceneTLAS();
    void CreateStateObject();
    void CreateResultBuffer();
    void CreatePhotonMappingBuffer();
    void CreateDepthBuffer();
    void CreateDenoisedColorBuffer();
    void CreatePositionBuffer();
    void CreateNormalBuffer();
    void CreateRootSignatureGlobal();
    void CreateRootSignatureLocal();
    void CreateLocalRootSignatureRayGen();
    void CreateSphereLocalRootSignature();
    void CreateFloorLocalRootSignature();
    void CreateGlassLocalRootSignature();
    void CreateShaderTable();
    void CreateComputeRootSignatureAndPSO();
    void CreateBitonicSortCBs();
    void CreateGridSortCB();
    void CreateDenoiseCBs();
    void CreateComputeShaderStateObject(const LPCWSTR& compiledComputeShaderName, ComPtr<ID3D12PipelineState>& computePipelineState, ComPtr<ID3D12RootSignature> rootSig);
    //�� combine
    void CreateAccelerationStructure();
    void CreateRaytracingRootSignatureAndPSO();
    void CreateRegularBuffer();
    void CreateConstantBuffer();

    void UpdateSceneTLAS();
    void UpdateSceneParams();
    void UpdateWindowText();

    void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, u32 pathSize);
    std::wstring GetAssetFullPath(LPCWSTR assetName);
    f32 Clamp(f32 min, f32 max, f32 src);
    f32 getFrameRate();
    utility::TextureResource LoadTextureFromFile(const std::wstring& fileName);
    
    void SetupMeshInfo(std::vector<D3D12_RAYTRACING_INSTANCE_DESC>& instanceDescs);

    void Grid3DSort();
    void BitonicSortLDS();
    void BitonicSortSimple();

    void Denoise();

    ComPtr<ID3D12GraphicsCommandList4> mCommandList;
    static const u32 BackBufferCount = dx12::RenderDeviceDX12::BackBufferCount;
    
    //Meshes
    utility::PolygonMesh mMeshStage;
    utility::PolygonMesh mMeshSphere;
    utility::PolygonMesh mMeshBox;
    utility::PolygonMesh mMeshGlass;
    utility::PolygonMesh mMeshMetal;
    utility::PolygonMesh mMeshLightSphere;
    std::wstring mMetalFileName;
    std::wstring mGlassFileName;
    std::wstring mStageTextureFileName;
    std::wstring mCubeMapTextureFileName;

    //ObjectAttributes of TLAS
    std::array<XMMATRIX, ReflectSpheres> mSpheresReflectTbl;
    std::array<XMMATRIX, RefractSpheres> mSpheresRefractTbl;
    std::array<XMMATRIX, NormalSpheres> mSpheresNormalTbl;
    std::array<XMMATRIX, ReflectBoxes> mBoxesReflectTbl;
    std::array<XMMATRIX, RefractBoxes> mBoxesRefractTbl;
    std::array<XMMATRIX, NormalBoxes> mBoxesNormalTbl;
    std::array<XMMATRIX, NormalGlasses> mGlasssNormalTbl;
    std::array<XMMATRIX, NormalMetals> mMetalsNormalTbl;
    std::array<XMMATRIX, NormalLight> mLightTbl;
    std::array<MaterialParam, NormalSpheres> mNormalSphereMaterialTbl;
    std::array<MaterialParam, NormalBoxes> mNormalBoxMaterialTbl;
    ComPtr<ID3D12Resource> mNormalSphereMaterialCB;
    ComPtr<ID3D12Resource> mNormalBoxMaterialCB;

    SceneParam mSceneParam;
    utility::TextureResource mGroundTex;
    utility::TextureResource mCubeMapTex;

    Camera mCamera;

    //Acceleration Structures
    ComPtr<ID3D12Resource> mTLAS;
    ComPtr<ID3D12Resource> mTLASUpdate;
    dx12::Descriptor mTLASDescriptor;
    ComPtr<ID3D12Resource> mInstanceDescsBuffer;

    //Buffers
    ComPtr <ID3D12Resource> mDXRMainOutput;
    dx12::Descriptor mMainOutputDescriptorUAV;
    ComPtr<ID3D12Resource> mDXROutput;
    dx12::Descriptor mOutputDescriptorUAV;
    ComPtr<ID3D12Resource> mPhotonMap;
    dx12::Descriptor mPhotonMapDescriptorSRV;
    dx12::Descriptor mPhotonMapDescriptorUAV;
    ComPtr<ID3D12Resource> mPhotonMapSorted;
    dx12::Descriptor mPhotonMapSortedDescriptorSRV;
    dx12::Descriptor mPhotonMapSortedDescriptorUAV;
    ComPtr<ID3D12Resource> mPhotonGrid;
    dx12::Descriptor mPhotonGridDescriptorSRV;
    dx12::Descriptor mPhotonGridDescriptorUAV;
    ComPtr<ID3D12Resource> mPhotonGridTmp;
    dx12::Descriptor mPhotonGridTmpDescriptorSRV;
    dx12::Descriptor mPhotonGridTmpDescriptorUAV;
    ComPtr<ID3D12Resource> mPhotonGridId;
    dx12::Descriptor mPhotonGridIdDescriptorSRV;
    dx12::Descriptor mPhotonGridIdDescriptorUAV;
    std::vector < ComPtr<ID3D12Resource>> mDepthBufferTbl;
    std::vector < dx12::Descriptor> mDepthBufferDescriptorSRVTbl;
    std::vector < dx12::Descriptor> mDepthBufferDescriptorUAVTbl;
    ComPtr<ID3D12Resource> mDenoisedColorBuffer;
    dx12::Descriptor mDenoisedColorBufferDescriptorSRV;
    dx12::Descriptor mDenoisedColorBufferDescriptorUAV;
    ComPtr<ID3D12Resource> mPositionBuffer;
    dx12::Descriptor mPositionBufferDescriptorSRV;
    dx12::Descriptor mPositionBufferDescriptorUAV;
    ComPtr<ID3D12Resource> mNormalBuffer;
    dx12::Descriptor mNormalBufferDescriptorSRV;
    dx12::Descriptor mNormalBufferDescriptorUAV;

    //ConstantBuffers
    std::vector<ComPtr<ID3D12Resource>> mBitonicLDSCB0Tbl;
    std::vector<ComPtr<ID3D12Resource>> mBitonicLDSCB1Tbl;
    std::vector<ComPtr<ID3D12Resource>> mBitonicLDSCB2Tbl;
    std::vector<ComPtr<ID3D12Resource>> mBitonicSimpleCBTbl;
    ComPtr<ID3D12Resource> mGridSortCB;
    std::vector<ComPtr<ID3D12Resource>> mDenoiseCBTbl;
    ComPtr<ID3D12Resource> mSceneCB;

    //Pipeline State
    ComPtr<ID3D12StateObject> mRTPSO;
    ComPtr<ID3D12Resource> mShaderTable;
    D3D12_DISPATCH_RAYS_DESC mDispatchRayDesc;
    ComPtr<ID3D12RootSignature> mGrs;
    ComPtr<ID3D12RootSignature> mRsRGS;
    ComPtr<ID3D12RootSignature> mRsFloor;
    ComPtr<ID3D12RootSignature> mRsSphereRR;
    ComPtr<ID3D12RootSignature> mRsSpherePhong;
    ComPtr<ID3D12RootSignature> mRsGlass;

    ComPtr<ID3D12StateObject> mRTPSOPhoton;
    ComPtr<ID3D12Resource> mShaderPhotonTable;
    D3D12_DISPATCH_RAYS_DESC mDispatchPhotonRayDesc;
    ComPtr<ID3D12RootSignature> mGrsPhoton;
    ComPtr<ID3D12RootSignature> mRsRGSPhoton;
    ComPtr<ID3D12RootSignature> mRsFloorPhoton;
    ComPtr<ID3D12RootSignature> mRsSphereRRPhoton;
    ComPtr<ID3D12RootSignature> mRsSpherePhongPhoton;
    ComPtr<ID3D12RootSignature> mRsGlassPhoton;

    ComPtr<ID3D12RootSignature> mRsBitonicSortLDS;
    ComPtr<ID3D12RootSignature> mRsTranspose;
    ComPtr<ID3D12PipelineState> mBitonicSortLDSPSO;
    ComPtr<ID3D12PipelineState> mTransposePSO;

    ComPtr<ID3D12RootSignature> mRsBitonicSortSimple;
    ComPtr<ID3D12PipelineState> mBitonicSortSimplePSO;

    ComPtr<ID3D12RootSignature> mRsBuildGrid;
    ComPtr<ID3D12RootSignature> mRsBuildGridIndices;
    ComPtr<ID3D12RootSignature> mRsCopy;
    ComPtr<ID3D12RootSignature> mRsClearGridIndices;
    ComPtr<ID3D12RootSignature> mRsRearrangePhoton;
    ComPtr<ID3D12PipelineState> mBuildGridPSO;
    ComPtr<ID3D12PipelineState> mBuildGridIndicesPSO;
    ComPtr<ID3D12PipelineState> mCopyPSO;
    ComPtr<ID3D12PipelineState> mClearGridIndicesPSO;
    ComPtr<ID3D12PipelineState> mRearrangePhotonPSO;

    ComPtr<ID3D12RootSignature> mRsDenoise;
    ComPtr<ID3D12PipelineState> mDenoisePSO;

    u32 mRenderFrame = 0;
    u32 mMoveFrame = 0;

    std::wstring mAssetPath;

    f32 mGlassObjYOfsset;
    XMFLOAT3 mGlassObjScale;
    f32 mMetalObjYOfsset;
    XMFLOAT3 mMetalObjScale;
    f32 mLightRange;
    f32 mIntenceBoost;
    f32 mGatherRadius;
    u32 mGatherBlockRange;
    f32 mStandardPhotonNum;
    bool mIsDebug;

    f32 mLightPosX;
    f32 mLightPosY;
    f32 mLightPosZ;

    f32 mPhi;
    f32 mTheta;

    f32 mCausticsBoost;

    f32 mGlassRotateRange;

    f32 mTmpAccumuRatio;

    ModelType mGlassModelType;
    ModelType mMetalModelType;

    bool mVisualizeLightRange;

    bool mReverseMove;
    bool mIsApplyCaustics;
    bool mIsUseDenoise;
    s32 mSpectrumMode;
    bool mIsMoveModel;
    bool mIsUseTexture;

    LARGE_INTEGER mCpuFreq;
    LARGE_INTEGER mStartTime;
    LARGE_INTEGER mEndTime;

    u32 mLightLambdaNum;

    u32 mPhotonMapSize1D;
};
