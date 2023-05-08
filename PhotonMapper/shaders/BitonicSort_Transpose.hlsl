#include "BitonicSort_Header.hlsli"

RWStructuredBuffer<uint2> gInput : register(u0);
RWStructuredBuffer<uint2> gOutput : register(u1);

groupshared uint2 transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];

[numthreads(TRANSPOSE_BLOCK_SIZE, TRANSPOSE_BLOCK_SIZE, 1)]
void matrixTranspose(uint3 dtid : SV_DispatchThreadID, uint3 gtid : SV_GroupThreadID, uint gid : SV_GroupIndex)
{
    transpose_shared_data[gid] = gInput[dtid.y * gBitonicParam.width + dtid.x];
    GroupMemoryBarrierWithGroupSync();
    uint2 XY = dtid.yx - gtid.yx + gtid.xy;
    gOutput[XY.y * gBitonicParam.height + XY.x] = transpose_shared_data[gtid.x * TRANSPOSE_BLOCK_SIZE + gtid.y];
}