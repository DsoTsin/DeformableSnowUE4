#pragma once

#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RHICommandList.h"

extern RENDERTOOL_API void
Accumulate(
    FRHICommandList& RHICmdList,
    FTextureRenderTargetResource* InputTexture,
    FTextureRenderTargetResource* InputTextureLast,
    FTextureRHIRef OutputTexture,
    FVector4 Offset, FVector4 Sizes);
