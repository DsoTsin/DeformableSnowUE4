#pragma once
#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "Engine/TextureRenderTarget2D.h"

extern RENDERTOOL_API 
void BlurPass(FRHICommandList& RHICmdList,
    FTextureRenderTargetResource* InputTexture,
    FTextureRenderTargetResource* OutputTexture,
    int32 TextureSize);