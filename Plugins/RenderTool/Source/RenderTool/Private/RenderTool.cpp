#include "RenderTool.h"
#include "Misc/EngineVersion.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FRenderTool"

void FRenderToolModule::StartupModule()
{
//#if UE_ENGINE_VERSION
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("RenderTool"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/RenderTool"), PluginShaderDir);
//#endif
}

void FRenderToolModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRenderToolModule, RenderTool)