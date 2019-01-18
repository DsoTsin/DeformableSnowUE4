

#include "SnowCapture.h"

#include "EngineModule.h"
#include "RendererInterface.h"

#include "Engine/World.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Components/SceneCaptureComponent2D.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Landscape.h"

#include "BlurPass.h"
#include "AccumulatePass.h"

int32 GEnableSnowProcess = 0;
static FAutoConsoleVariableRef CVarEnableSnowProcess(
    TEXT("r.EnableSnowProcess"),
    GEnableSnowProcess,
    TEXT("Enable Snow Process")
);

#pragma optimize("", off)

ASnowCapture::ASnowCapture() 
{
    CaptureSize = 2048;
    RenderSize = 256;
    PrimaryActorTick.bCanEverTick = true;
    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SnowCapture"));
    RootComponent = SceneCapture;
    SceneCapture->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90.0, 90.0)));
    SceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
    SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    SceneCapture->OrthoWidth = CaptureSize;
    SceneCapture->CaptureSource = ESceneCaptureSource::SCS_SceneDepth;
    SceneCapture->PostProcessBlendWeight = 1.0;
    SceneCapture->bCaptureEveryFrame = false;
}

void ASnowCapture::BeginPlay() {
    Super::BeginPlay();

    CaptureRTDepth = NewObject<UTextureRenderTarget2D>(this, TEXT("DepthRT_Snow"));
    CaptureRTDepth->RenderTargetFormat = RTF_RGBA16f;
    CaptureRTDepth->ClearColor = FLinearColor::Black;
    CaptureRTDepth->InitAutoFormat(RenderSize, RenderSize);
    CaptureRTDepth->UpdateResourceImmediate(true);

    SceneCapture->TextureTarget = CaptureRTDepth;

    AccumulatedRTDepth = NewObject<UTextureRenderTarget2D>(this, TEXT("DepthRT_Accum"));
    AccumulatedRTDepth->RenderTargetFormat = RTF_RGBA16f;
    AccumulatedRTDepth->ClearColor = FLinearColor::Black;
    AccumulatedRTDepth->InitAutoFormat(RenderSize, RenderSize);
    AccumulatedRTDepth->UpdateResourceImmediate(true);

    if (ProcessedRTDepth) {
        ProcessedRTDepth->UpdateResourceImmediate(true);
    }

    LastRenderSize = RenderSize;
    LastCaptureSize = CaptureSize;
    LastLocation = GetActorLocation();
}

void ASnowCapture::FollowActor(AActor* Actor) {
    FollowedActor = Actor;
}

void ASnowCapture::OnCaptureMoved() {
    LastLocation = GetActorLocation();
    float TexelSize = CaptureSize / RenderSize; // Per pixel world size

    auto FollowActorLoc = FollowedActor->GetActorLocation();

    auto FX = FMath::GridSnap(FollowActorLoc.X, TexelSize);
    auto FY = FMath::GridSnap(FollowActorLoc.Y, TexelSize);

    SetActorLocation(FVector(FX, FY, LastLocation.Z));
}

FVector ASnowCapture::GetLocationInTexture() const
{
    auto Loc = GetActorLocation();
    auto LocTex = Loc / (1.0 * SceneCapture->OrthoWidth);
    LocTex.Z = Loc.Z;
    return LocTex;
}

void ASnowCapture::CheckSizeChange() {
    if (RenderSize != LastRenderSize) {
        CaptureRTDepth->ResizeTarget(RenderSize, RenderSize);
        ProcessedRTDepth->ResizeTarget(RenderSize, RenderSize);
        AccumulatedRTDepth->ResizeTarget(RenderSize, RenderSize);
        LastRenderSize = RenderSize;
    }
    if (CaptureSize != LastCaptureSize) {
        SceneCapture->OrthoWidth = CaptureSize;
        LastCaptureSize = CaptureSize;
    }
}

void ASnowCapture::Tick(float Time) {
    Super::Tick(Time);
    if (!FollowedActor)
        return;
    CheckSizeChange();
    SceneCapture->CaptureScene();
    //OnCaptureMoved();
    auto Offset = GetActorLocation() - LastLocation;
    Offset /= (float)SceneCapture->OrthoWidth;
    ENQUEUE_RENDER_COMMAND(SnowPass)(
        [this, Offset](FRHICommandList& RHICmdList) {
        Accumulate_RenderThread(
            FVector4(Offset.X, Offset.Y, 0, 0), 
            RenderSize);
        if (GEnableSnowProcess) {
            Process_RenderThread();
        }
    });
}

void ASnowCapture::OnCaptureResized(uint32 Size) {
    
}

void ASnowCapture::Accumulate_RenderThread(FVector4 Offset, uint32 InRenderSize) {
    check(IsInRenderingThread());
    FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();
    Accumulate(RHICmdList, 
        CaptureRTDepth->GetRenderTargetResource(),
        AccumulatedRTDepth->GetRenderTargetResource(),
        Offset, FVector4(InRenderSize)
    );
}

void ASnowCapture::Process_RenderThread() {
    check(IsInRenderingThread());
    FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();
    if (ProcessedRTDepth) {
        BlurPass(RHICmdList, AccumulatedRTDepth->GetRenderTargetResource(), ProcessedRTDepth->GetRenderTargetResource(), RenderSize);
    }
}

UTextureRenderTarget2D* ASnowCapture::GetProcessedMap() const {
    return ProcessedRTDepth;
}

void ASnowCapture::OverrideMaterial(UMaterialInstanceDynamic* MI)
{
    MI->SetTextureParameterValue(TEXT("T_SnowMap"), GetProcessedMap());
}

void ASnowCapture::OverrideLandscapeMaterial(ALandscape * Landscape)
{
    //Landscape->SetLandscapeMaterialTextureParameterValue(TEXT("T_SnowMap"), GetProcessedMap());
}

#pragma optimize("", on)