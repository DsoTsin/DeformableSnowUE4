// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SnowTrailCapture.generated.h"

UCLASS()
class SNOW_API ASnowTrailCapture : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASnowTrailCapture();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Category = Rendering, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(Category = Rendering, EditAnywhere, BlueprintReadOnly)
	class UMaterial* SnapToPersistentMaterial;

	UPROPERTY(Category = "Rendering", EditAnywhere, BlueprintReadOnly)
	class UTextureRenderTarget2D* PersistentRT;

	UPROPERTY(Category = "Rendering", VisibleAnywhere, BlueprintReadOnly)
	FVector MoveOffset;

private:
	void MoveCapture();
	void SnapToPersistent();
	FVector GetFirstPlayerLocation();
	class ACharacter* GetFirstCharacter();

	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* PersistentDrawMaterial;

	UPROPERTY(Transient)
	class UCanvas* SnowCanvas;

};
