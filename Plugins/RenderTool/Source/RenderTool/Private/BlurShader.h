#pragma once
#include "ShaderParameterUtils.h"
#include "GlobalShader.h"
#include "UniformBuffer.h"
#include "RHICommandList.h"

BEGIN_UNIFORM_BUFFER_STRUCT(FBlurTextureParameters, )
UNIFORM_MEMBER(FVector4, InputOutputSize)
END_UNIFORM_BUFFER_STRUCT(FBlurTextureParameters)

typedef TUniformBufferRef<FBlurTextureParameters> FBlurParametersRef;

template<bool bHorizontalBlur>
class FBlurCSShader : public FGlobalShader
{
    DECLARE_SHADER_TYPE(FBlurCSShader, Global);
public:
    FBlurCSShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
        : FGlobalShader(Initializer)
    {
        InputTex.Bind(Initializer.ParameterMap, TEXT("InputTex"));
        OutputTex.Bind(Initializer.ParameterMap, TEXT("OutComputeTex"));
    }
    FBlurCSShader() {}

    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
    {
        FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
        OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
    }

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }

    virtual bool Serialize(FArchive& Ar) override
    {
        bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
        Ar << InputTex << OutputTex;
        return bShaderHasOutdatedParameters;
    }

    template <typename TRHICmdList>
    void UnsetParameters(TRHICmdList& RHICmdList)
    {
        const FComputeShaderRHIParamRef ShaderRHI = GetComputeShader();
        RHICmdList.SetUAVParameter(ShaderRHI, OutputTex.GetBaseIndex(), NULL);
        RHICmdList.SetShaderResourceViewParameter(ShaderRHI, InputTex.GetBaseIndex(), NULL);
    }

    template <typename TRHICmdList>
    void SetUniform(TRHICmdList& RHICmdList, const FBlurTextureParameters& Params) {
        BlurParams = FBlurParametersRef::CreateUniformBufferImmediate(Params, UniformBuffer_SingleDraw);
        SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FBlurTextureParameters>(), BlurParams);
    }

    template <typename TRHICmdList>
    void SetInTex(TRHICmdList& RHICmdList, FTextureRHIParamRef InTex)
    {
        FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
        if (InputTex.IsBound()) {
            SetTextureParameter(RHICmdList, ComputeShaderRHI, InputTex, InTex);
        }
    }

    template <typename TRHICmdList>
    void SetOutTex(TRHICmdList& RHICmdList, FUnorderedAccessViewRHIRef OutTex)
    {
        FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
        if (OutputTex.IsBound())
            RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputTex.GetBaseIndex(), OutTex);
    }

private:
    FBlurParametersRef	        BlurParams;
    FShaderResourceParameter    InputTex;
    FShaderParameter	        OutputTex;
};

class FQuadVS : public FGlobalShader
{
    DECLARE_SHADER_TYPE(FQuadVS, Global);
public:
    FQuadVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
        : FGlobalShader(Initializer)
    {}
    FQuadVS() {}
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }
};

class FBlurPS : public FGlobalShader
{
    DECLARE_SHADER_TYPE(FBlurPS, Global);
public:
    FBlurPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
        : FGlobalShader(Initializer)
    {
        InputTex.Bind(Initializer.ParameterMap, TEXT("InputTex"));
        InputSampler.Bind(Initializer.ParameterMap, TEXT("InputSampler"));
        InvInputSize.Bind(Initializer.ParameterMap, TEXT("InvInputSize"));
    }
    FBlurPS() {}
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }
    void SetParameters(
        FRHICommandList& RHICmdList, 
        FTextureRHIParamRef InputTexture, 
        FVector4 TextureSize, 
        FSamplerStateRHIParamRef& SamplerStateLinear
    )
    {
        FPixelShaderRHIParamRef PixelShaderRHI = GetPixelShader();
        SetShaderValue(RHICmdList, PixelShaderRHI, InvInputSize, TextureSize);
        SetTextureParameter(RHICmdList, PixelShaderRHI, this->InputTex, InputSampler, SamplerStateLinear, InputTexture);
    }
    virtual bool Serialize(FArchive& Ar)
    {
        bool bShaderHasOutdatedParameters = FGlobalShader::Serialize(Ar);
        Ar << InputTex << InputSampler << InvInputSize;
        return bShaderHasOutdatedParameters;
    }
private:
    FShaderResourceParameter    InputTex;
    FShaderResourceParameter    InputSampler;
    FShaderParameter            InvInputSize;
};
