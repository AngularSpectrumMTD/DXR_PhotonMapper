#include "DxrPhotonMapper.h"
#include <fstream>
#include <wincodec.h>

void DxrPhotonMapper::GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, u32 pathSize)
{
    if (path == nullptr)
    {
        throw std::exception();
    }

    DWORD size = GetModuleFileName(nullptr, path, pathSize);
    if (size == 0 || size == pathSize)
    {
        // Method failed or path was truncated.
        throw std::exception();
    }

    WCHAR* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash)
    {
        *(lastSlash + 1) = L'\0';
    }
}

std::wstring DxrPhotonMapper::GetAssetFullPath(LPCWSTR assetName)
{
    return mAssetPath + assetName;
}

void DxrPhotonMapper::CreateComputeShaderStateObject(const LPCWSTR& compiledComputeShaderName, ComPtr<ID3D12PipelineState>& computePipelineState, ComPtr<ID3D12RootSignature> rootSig)
{
    u32 fileSize = 0;
    UINT8* shaderCodePtr;
    utility::ReadDataFromFile(GetAssetFullPath(compiledComputeShaderName).c_str(), &shaderCodePtr, &fileSize);

    D3D12_COMPUTE_PIPELINE_STATE_DESC copmputePSODesc = {};
    copmputePSODesc.pRootSignature = rootSig.Get();
    copmputePSODesc.CS = CD3DX12_SHADER_BYTECODE((void*)shaderCodePtr, fileSize);

    HRESULT hr = mDevice->GetDevice()->CreateComputePipelineState(&copmputePSODesc, IID_PPV_ARGS(&computePipelineState));

    if (FAILED(hr)) {
        throw std::runtime_error("CreateComputePipelineStateObject failed.");
    }
}


void DxrPhotonMapper::CreateComputeRootSignatureAndPSO()
{
    {
        utility::RootSignatureCreater rsCreater;
        rsCreater.Push(utility::RootSignatureCreater::RootType::CBV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 0);
        mRsBitonicSortLDS = rsCreater.Create(mDevice, false, L"rsBitonic");
        CreateComputeShaderStateObject(ComputeShaders::BitonicSort, mBitonicSortLDSPSO, mRsBitonicSortLDS);

        rsCreater.Clear();
        rsCreater.Push(utility::RootSignatureCreater::RootType::CBV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 1);
        mRsTranspose = rsCreater.Create(mDevice, false, L"rsTranspose");
        CreateComputeShaderStateObject(ComputeShaders::Transpose, mTransposePSO, mRsTranspose);
    }

    {
        utility::RootSignatureCreater rsCreater;
        rsCreater.Push(utility::RootSignatureCreater::RootType::CBV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 0);
        mRsBitonicSortSimple = rsCreater.Create(mDevice, false, L"rsBitonic2");
        CreateComputeShaderStateObject(ComputeShaders::BitonicSort2, mBitonicSortSimplePSO, mRsBitonicSortSimple);
    }

    {
        utility::RootSignatureCreater rsCreater;
        rsCreater.Push(utility::RootSignatureCreater::RootType::CBV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 1);
        mRsBuildGrid = rsCreater.Create(mDevice, false, L"rsBuildGrid");
        CreateComputeShaderStateObject(ComputeShaders::BuildGrid, mBuildGridPSO, mRsBuildGrid);

        rsCreater.Clear();
        rsCreater.Push(utility::RootSignatureCreater::RootType::CBV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 1);
        mRsBuildGridIndices = rsCreater.Create(mDevice, false, L"rsBuildGridIndices");
        CreateComputeShaderStateObject(ComputeShaders::BuildGridIndices, mBuildGridIndicesPSO, mRsBuildGridIndices);

        rsCreater.Clear();
        rsCreater.Push(utility::RootSignatureCreater::RootType::CBV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 1);
        mRsCopy = rsCreater.Create(mDevice, false, L"rsCopy");
        CreateComputeShaderStateObject(ComputeShaders::Copy, mCopyPSO, mRsCopy);

        rsCreater.Clear();
        rsCreater.Push(utility::RootSignatureCreater::RootType::CBV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 0);
        mRsClearGridIndices = rsCreater.Create(mDevice, false, L"rsClearGridIndices");
        CreateComputeShaderStateObject(ComputeShaders::ClearGridIndices, mClearGridIndicesPSO, mRsClearGridIndices);

        rsCreater.Clear();
        rsCreater.Push(utility::RootSignatureCreater::RootType::CBV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 1);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 2);
        mRsRearrangePhoton = rsCreater.Create(mDevice, false, L"rsRearrangePhoton");
        CreateComputeShaderStateObject(ComputeShaders::RearrangePhoton, mRearrangePhotonPSO, mRsRearrangePhoton);
    }

    {
        utility::RootSignatureCreater rsCreater;
        rsCreater.Push(utility::RootSignatureCreater::RootType::CBV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 0);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 1);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 2);
        rsCreater.Push(utility::RootSignatureCreater::RangeType::UAV, 3);
        mRsDenoise = rsCreater.Create(mDevice, false, L"rsDenoise");
        CreateComputeShaderStateObject(ComputeShaders::Denoise, mDenoisePSO, mRsDenoise);
    }
}