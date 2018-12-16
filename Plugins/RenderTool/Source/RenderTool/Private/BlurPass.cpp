#include "BlurPass.h"
#include "Engine.h"
#include "RHIStaticStates.h"
#include "EngineModule.h"
#include "RendererInterface.h"
#include "ShaderParameterUtils.h"
#include "StaticBoundShaderState.h"
#include "RenderingThread.h"
#include "PipelineStateCache.h"
#include "RenderTool.h"
#include "BlurShader.h"

TGlobalResource<FQuadVertexDeclaration> GQuadVertexDeclaration;

void BlurPass(FRHICommandList& RHICmdList, 
    FTextureRenderTargetResource* InputTexture,
    FTextureRenderTargetResource* OutputTexture,
    int32 TextureSize)
{
    SCOPED_DRAW_EVENT(RHICmdList, BlurPass);
    FQuadVertex Quad[4] = { { FVector4(-1.0f, 1.0f, 0.0f, 1.0f),FVector2D(0.0f,0.0f) } ,
                            { FVector4(1.0f, 1.0f, 0.0f, 1.0f),FVector2D(1.0f,0.0f) },
                            { FVector4(-1.0f, -1.0f, 0.0f, 1.0f),FVector2D(0.0f,1.0f) } ,
                            { FVector4(1.0f, -1.0f, 0.0f, 1.0f),FVector2D(1.0f,1.0f) } };
    ERHIFeatureLevel::Type FeatureLevel = ERHIFeatureLevel::SM5;
    FRHIRenderTargetView ColorView = FRHIRenderTargetView(OutputTexture->TextureRHI,
        0, -1, ERenderTargetLoadAction::ENoAction, ERenderTargetStoreAction::EStore);
    FRHIDepthRenderTargetView DepthView;
    FRHISetRenderTargetsInfo Info(1, &ColorView, DepthView);
    RHICmdList.SetRenderTargetsAndClear(Info);
    TShaderMapRef<FQuadVS> BlurVS(GetGlobalShaderMap(FeatureLevel));
    TShaderMapRef<FBlurPS> BlurPS(GetGlobalShaderMap(FeatureLevel));
    RHICmdList.SetViewport(0, 0, 0.0f, TextureSize, TextureSize, 1.0f);
    FGraphicsPipelineStateInitializer GraphicsPSOInit;
    RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
    GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
    GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
    GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
    GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
    GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GQuadVertexDeclaration.VertexDeclarationRHI;
    GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*BlurVS);
    GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*BlurPS);
    SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);
    float InvTextureSize = 1.0f / TextureSize;
    FSamplerStateRHIParamRef SamplerStateLinear = TStaticSamplerState<SF_Point>::GetRHI();
    SamplerStateLinear = TStaticSamplerState<SF_Bilinear>::GetRHI();
    BlurPS->SetParameters(RHICmdList, InputTexture->TextureRHI, FVector4(InvTextureSize), SamplerStateLinear);
    DrawPrimitiveUP(RHICmdList, PT_TriangleStrip, 2, Quad, sizeof(Quad[0]));
}