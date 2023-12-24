# DXRPhotonMapper
Photon Mapper By Use Of DirectX Raytracing

![reflacted](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/d52618b2-4474-4ae5-bd3e-ecab16ec214d)

![reflected](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/299ce763-56ab-441d-8026-97616b8cd88d)

![transparent](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/4858ab5c-ba06-4526-b9fb-b1dddeac4742)

![metallic](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/3f97fb0c-5980-477d-8e73-8ab642776e72)

![rough](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/8a75a7b8-14b1-4805-93d5-54ecb47f6938)

### Directional Light / Cube Map

| Directional Light | Cube Map |
| ---- | ---- |
| ![Directional](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/85d5195e-f664-4037-824f-36c812d78b3d) | ![CubeMap](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/19fc8f4f-9e6a-425c-8542-239c02c7ebc7) |

### Caustics
Caustics are rendered by spectral rendering

![spectral](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/d7580bab-4ba6-4408-8c1a-788c410f7536)

### Algorithm
Photon Mapping : Hashed Grid

Denoiser : SVGF

Shading : GGX

### How To Use
**NOTE : This program can only handle triangular polygons**

#### Without Sponza
1.Build and Execute

![sample](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/6ab59527-27d9-4606-9d9f-465ba5dd9673)

#### With Sponza
1. Download model(sponza) from "McGuire Computer Graphics Archive"(https://casual-effects.com/data/)

![Exp1](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/0b4f954b-4875-4a5f-816b-26174ce90bea)

2. Open sponza.obj on blender, and do these actions.

![Exp2](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/317aa562-2fbb-4605-badc-c04a505ff24d)

3. Export 3D model as obj file(Name the file "sponza.obj")

![Exp3](https://github.com/AngularSpectrumMTD/DXR_PhotonMapper/assets/65929274/211b09be-de71-4165-b8bb-a9a78914ebd8)

4.Create a new folder (ex. "sponza") in "DXRPhotonMapper\PhotonMapper\model" and copy the "textures" folder and "sponza.mtl" file from the downloaded folder and the obj file you just exported to the folder you just created.

5.Set ｍOBJFileName and mOBJFolderName at void DxrPhotonMapper::Setup()[DxrPhotonMapper.cpp] like this

    mOBJFileName = "sponza.obj";
    mOBJFolderName = "model/sponza";

6.Build and Execute

### Graphics API
DirectX12

### Action

- A : enable / disable temporal accumulation
- I : determine other button meaning (+ / -)
- J : start / stop model
- H : enable / disable Directional Light
- G : increase / decrease photon gather radius
- X / Y / Z : light position
- L : increase / decrease light emission range
- T / P : light emission angle(theta / phi)
- K : increase / decrease light intensity
- B : increase / decrease photon gathering block num
- V : visualize DI emission element
- N : visualize caustics
- D : enable / disable SVGF(A-Trous wavelet filtering)
- Q : increase / decrease caustics boost
- U : enable / disable texture
- R : increase / decrease roughness
- S : increase / decrease translucency
- M : increase / decrease metallic