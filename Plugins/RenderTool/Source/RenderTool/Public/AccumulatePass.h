#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RHICommandList.h"

extern RENDERTOOL_API void
Accumulate(
    FRHICommandList& RHICmdList,
    FTextureRenderTargetResource* InputTexture,
    FTextureRenderTargetResource* OutputTexture,
    FVector4 OffsetAndSize, FVector4 Sizes);
