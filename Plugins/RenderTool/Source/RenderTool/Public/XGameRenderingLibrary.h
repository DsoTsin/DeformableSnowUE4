#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "RHI.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "XGameRenderingLibrary.generated.h"


UCLASS(MinimalAPI, meta = (ScriptName = "XRenderingLibrary"))
class UXGameRenderingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	/**
		* Clears the specified render target with the given ClearColor.
		*/
	UFUNCTION(BlueprintCallable, Category = "Rendering", meta = (Keywords = "Blur", WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static RENDERTOOL_API void BlurTexture(UObject* WorldContextObject, UTextureRenderTarget2D* TextureRenderTarget);
};