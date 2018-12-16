// Fill out your copyright notice in the Description page of Project Settings.

#include "SnowTrailCapture.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"

// Sets default values
ASnowTrailCapture::ASnowTrailCapture()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootNode"));
    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
    SceneCapture->SetupAttachment(RootComponent);
    SceneCapture->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90.0, 90.0)));
    SceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
    SceneCapture->OrthoWidth = 2048;
    SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    SceneCapture->PostProcessBlendWeight = 1.0;
	PersistentDrawMaterial = nullptr;
}

void ASnowTrailCapture::BeginPlay()
{
	Super::BeginPlay();

	if (!SnowCanvas)
	{
		SnowCanvas = NewObject<UCanvas>(GetTransientPackage(), NAME_None);
	}
	if (SnapToPersistentMaterial) {
		PersistentDrawMaterial = UMaterialInstanceDynamic::Create(SnapToPersistentMaterial, this, FName("SnowSnapPP"));
	}
	// clean up render targets
	if (PersistentRT) {
		PersistentRT->UpdateResourceImmediate(true);
	}
	if (SceneCapture && SceneCapture->TextureTarget) {
        SceneCapture->TextureTarget->UpdateResourceImmediate(true);
	}
	ACharacter* C = GetFirstCharacter();

	C->GetMesh()->SetRenderCustomDepth(true);
}

void ASnowTrailCapture::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ASnowTrailCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveCapture();
	SnapToPersistent();
}

FVector SnapToPixelWorldSize(float X, float Y, float PixelWorldSize) {
	return FVector((FMath::FloorToInt(X / PixelWorldSize) + 0.5) * PixelWorldSize, 
		(FMath::FloorToInt(Y / PixelWorldSize) + 0.5) * PixelWorldSize, 
		0.0);
}

void ASnowTrailCapture::MoveCapture()
{
	FVector FirstLocation = GetFirstPlayerLocation();
	FVector Pos = SnapToPixelWorldSize(FirstLocation.X, FirstLocation.Y,
        SceneCapture->OrthoWidth / 256.0);
	MoveOffset = Pos - GetActorLocation();
	MoveOffset.Z = 0;

	AddActorWorldOffset(MoveOffset);
	// Update CaptureLocation in Material
}

void ASnowTrailCapture::SnapToPersistent()
{
	GetWorld()->SendAllEndOfFrameUpdates();
	UTextureRenderTarget2D* RT = PersistentRT;
	if (!RT || !SceneCapture->TextureTarget || !PersistentDrawMaterial)
		return;
	FCanvas RenderCanvas(
		RT->GameThread_GetRenderTargetResource(),
		nullptr,
		GetWorld(),
		GetWorld()->FeatureLevel);

	//PersistentDrawMaterial->SetTextureParameterValue("T_SnowCapture", CaptureComponent2D->TextureTarget);
	PersistentDrawMaterial->SetVectorParameterValue("V_Offset", FLinearColor(MoveOffset / SceneCapture->OrthoWidth));

	SnowCanvas->Init(RT->SizeX, RT->SizeY, nullptr, &RenderCanvas);
	SnowCanvas->Update();

	TDrawEvent<FRHICommandList>* DrawMaterialToTargetEvent = new TDrawEvent<FRHICommandList>();

	FName RTName = RT->GetFName();
	ENQUEUE_RENDER_COMMAND(BeginDrawEventCommand)(
		[RTName, DrawMaterialToTargetEvent](FRHICommandList& RHICmdList)
	{
		BEGIN_DRAW_EVENTF(
			RHICmdList,
			SnapSnow,
			(*DrawMaterialToTargetEvent),
			*RTName.ToString());
	});

	SnowCanvas->K2_DrawMaterial(PersistentDrawMaterial, FVector2D(0, 0), FVector2D(RT->SizeX, RT->SizeY), FVector2D(0, 0));

	RenderCanvas.Flush_GameThread();
	SnowCanvas->Canvas = NULL;

	FTextureRenderTargetResource* RenderTargetResource = RT->GameThread_GetRenderTargetResource();
	ENQUEUE_RENDER_COMMAND(CanvasRenderTargetResolveCommand)(
		[RenderTargetResource, DrawMaterialToTargetEvent](FRHICommandList& RHICmdList)
	{
		RHICmdList.CopyToResolveTarget(RenderTargetResource->GetRenderTargetTexture(), RenderTargetResource->TextureRHI, FResolveParams());
		STOP_DRAW_EVENT((*DrawMaterialToTargetEvent));
		delete DrawMaterialToTargetEvent;
	}
	);
}

FVector ASnowTrailCapture::GetFirstPlayerLocation()
{
	ACharacter* C = GetFirstCharacter();
	return C ? C->GetActorLocation() : FVector();
}

ACharacter* ASnowTrailCapture::GetFirstCharacter()
{
	UWorld* world = GetWorld();
	if (world) {
		APlayerController* PC = nullptr;
		uint32 Index = 0;
		for (FConstPlayerControllerIterator Iterator = world->GetPlayerControllerIterator();
			Iterator; ++Iterator) {
			if (Index == 0) {
				PC = Iterator->Get();
				ACharacter* pawn = Cast<ACharacter>(PC->GetPawn());
				if (pawn) {
					return pawn;
				}
			}
			Index++;
		}
	}
	return nullptr;
}
