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
        
        InputTexLast.Bind(Initializer.ParameterMap, TEXT("InputTexLast"));
        InputSamplerLast.Bind(Initializer.ParameterMap, TEXT("InputTexLastSampler"));
        
        InputSize.Bind(Initializer.ParameterMap, TEXT("InputTexSizes"));
        InputOffset.Bind(Initializer.ParameterMap, TEXT("Offset"));
    }
    FAccumulatePS() {}
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }
    void SetParameters(
        FRHICommandList& RHICmdList,
        FVector4 Offset,
        FVector4 TextureSize,
        FTextureRHIParamRef InputTexture,
        FSamplerStateRHIParamRef& SamplerStateLinear,
        FTextureRHIParamRef InputTextureLast,
        FSamplerStateRHIParamRef& SamplerStateLast)
    {
        FPixelShaderRHIParamRef PixelShaderRHI = GetPixelShader();
        SetShaderValue(RHICmdList, PixelShaderRHI, InputOffset, Offset);
        SetShaderValue(RHICmdList, PixelShaderRHI, InputSize, TextureSize);
        SetTextureParameter(RHICmdList, PixelShaderRHI, InputTex, InputSampler, SamplerStateLinear, InputTexture);
        SetTextureParameter(RHICmdList, PixelShaderRHI, InputTexLast, InputSamplerLast, SamplerStateLast, InputTextureLast);
    }
    virtual bool Serialize(FArchive& Ar)
    {
        bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
        Ar << InputTex << InputSampler << InputTexLast << InputSamplerLast << InputSize << InputOffset;
        return bShaderHasOutdatedParameters;
    }
private:
    FShaderResourceParameter    InputTex;
    FShaderResourceParameter    InputSampler;

    FShaderResourceParameter    InputTexLast;
    FShaderResourceParameter    InputSamplerLast;

    FShaderParameter            InputOffset;
    FShaderParameter            InputSize;
};