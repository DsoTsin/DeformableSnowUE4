#include "BlurShader.h"

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FBlurTextureParameters, TEXT("InputOutputSize"))
IMPLEMENT_SHADER_TYPE(template<>, FBlurCSShader<true>, TEXT("/Plugin/RenderTool/Private/BlurPass.usf"), TEXT("HorzBlurCS"), SF_Compute);
IMPLEMENT_SHADER_TYPE(template<>, FBlurCSShader<false>, TEXT("/Plugin/RenderTool/Private/BlurPass.usf"), TEXT("VertBlurCS"), SF_Compute);

IMPLEMENT_SHADER_TYPE(, FQuadVS, TEXT("/Plugin/RenderTool/Private/QuadVS.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FBlurPS, TEXT("/Plugin/RenderTool/Private/BlurShader.usf"), TEXT("MainPS"), SF_Pixel);
