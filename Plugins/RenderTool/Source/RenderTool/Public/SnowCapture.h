#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SnowCapture.generated.h"

UCLASS()
class RENDERTOOL_API ASnowCapture : public AActor
{
    GENERATED_BODY()
public:
    ASnowCapture();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float Time) override;

    virtual void OnCaptureResized(uint32 Size);
    virtual void OnCaptureMoved();

    UPROPERTY(Category = Rendering, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class USceneCaptureComponent2D* SceneCapture;

    UPROPERTY()
    uint32 CaptureSize;

    UPROPERTY()
    uint32 RenderSize;

    UFUNCTION(BlueprintCallable)
    class UTextureRenderTarget2D* GetProcessedMap() const;

    UFUNCTION(BlueprintCallable)
    void FollowActor(AActor* Actor);

    UFUNCTION(BlueprintCallable)
    void OverrideMaterial(class UMaterialInstanceDynamic* MI);

    UFUNCTION(BlueprintCallable)
    void OverrideLandscapeMaterial(class ALandscape* Landscape);

    UFUNCTION(BlueprintCallable)
    FVector GetLocationInTexture() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rendering)
    class UTextureRenderTarget2D* ProcessedRTDepth;

private:
    void CheckSizeChange();
    void Accumulate_RenderThread(FVector4 Offset, uint32 RenderSize);
    void Process_RenderThread();

    UPROPERTY(Transient)
    class UTextureRenderTarget2D* CaptureRTDepth;
    UPROPERTY(Transient)
    class UTextureRenderTarget2D* AccumulatedRTDepth;

    UPROPERTY(Transient)
    class UMaterialInstanceDynamic* AccumMID;

    uint32 LastCaptureSize; // World Pixels
    uint32 LastRenderSize; // Render Size
    
    UPROPERTY(Transient)
    class AActor* FollowedActor;

    FVector LastLocation;
};
