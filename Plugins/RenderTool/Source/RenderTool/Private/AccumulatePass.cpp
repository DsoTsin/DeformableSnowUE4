#include "AccumulatePass.h"
#include "AccumulateShader.h"

#include "Engine.h"
#include "RHIStaticStates.h"
#include "StaticBoundShaderState.h"
#include "RenderingThread.h"
#include "PipelineStateCache.h"

#include "RenderTool.h"
#include "BlurShader.h"

#pragma optimize("", off)

void Accumulate(
    FRHICommandList& RHICmdList,
    FTextureRenderTargetResource* InputTexture,
    FTextureRenderTargetResource* OutputTexture,
    FVector4 OffsetAndSize, FVector4 Sizes)
{
    SCOPED_DRAW_EVENT(RHICmdList, AccumulatePass);
    FQuadVertex Quad[4] = { { FVector4(-1.0f, 1.0f, 0.0f, 1.0f),FVector2D(0.0f,0.0f) } ,
                            { FVector4(1.0f, 1.0f, 0.0f, 1.0f),FVector2D(1.0f,0.0f) },
                            { FVector4(-1.0f, -1.0f, 0.0f, 1.0f),FVector2D(0.0f,1.0f) } ,
                            { FVector4(1.0f, -1.0f, 0.0f, 1.0f),FVector2D(1.0f,1.0f) } };
    ERHIFeatureLevel::Type FeatureLevel = ERHIFeatureLevel::SM5;
    FRHIRenderTargetView ColorView = FRHIRenderTargetView(OutputTexture->TextureRHI,
        0, -1, ERenderTargetLoadAction::ELoad, ERenderTargetStoreAction::EStore);
    FRHIDepthRenderTargetView DepthView;
    RHICmdList.SetRenderTargetsAndClear({ 1, &ColorView, DepthView });
    TShaderMapRef<FQuadVS>          AccumuVS(GetGlobalShaderMap(FeatureLevel));
    TShaderMapRef<FAccumulatePS>    AccumuPS(GetGlobalShaderMap(FeatureLevel));
    RHICmdList.SetViewport(0, 0, 0.0f, Sizes.X, Sizes.Y, 1.0f);
    FGraphicsPipelineStateInitializer GraphicsPSOInit;
    RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
    GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
    GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
    GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
    GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
    GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GQuadVertexDeclaration.VertexDeclarationRHI;
    GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*AccumuVS);
    GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*AccumuPS);
    SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
    FSamplerStateRHIParamRef SamplerStateLinear = TStaticSamplerState<SF_Bilinear>::GetRHI();
    AccumuPS->SetParameters(RHICmdList, OffsetAndSize, InputTexture->TextureRHI, SamplerStateLinear);
    DrawPrimitiveUP(RHICmdList, PT_TriangleStrip, 2, Quad, sizeof(Quad[0]));
}
#pragma optimize("", on)