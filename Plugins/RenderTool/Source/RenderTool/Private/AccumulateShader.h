#pragma once
#include "ShaderParameterUtils.h"
#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"

class FAccumulatePS : public FGlobalShader {
    DECLARE_SHADER_TYPE(FAccumulatePS, Global);
public:
    FAccumulatePS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
        : FGlobalShader(Initializer)
    {
        InputTex.Bind(Initializer.ParameterMap, TEXT("InputTex"));
        InputSampler.Bind(Initializer.ParameterMap, TEXT("InputTexSampler"));
        OffsetAndSize.Bind(Initializer.ParameterMap, TEXT("OffsetAndSize"));
    }
    FAccumulatePS() {}
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }
    void SetParameters(
        FRHICommandList& RHICmdList,
        FVector4 InOffsetSize,
        FTextureRHIParamRef InputTexture,
        FSamplerStateRHIParamRef& SamplerStateLinear)
    {
        FPixelShaderRHIParamRef PixelShaderRHI = GetPixelShader();
        SetShaderValue(RHICmdList, PixelShaderRHI, OffsetAndSize, InOffsetSize);
        SetTextureParameter(RHICmdList, PixelShaderRHI, InputTex, InputSampler, SamplerStateLinear, InputTexture);
    }
    virtual bool Serialize(FArchive& Ar)
    {
        bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
        Ar << InputTex << InputSampler << OffsetAndSize;
        return bShaderHasOutdatedParameters;
    }
private:
    FShaderResourceParameter    InputTex;
    FShaderResourceParameter    InputSampler;

    FShaderParameter            OffsetAndSize;
};