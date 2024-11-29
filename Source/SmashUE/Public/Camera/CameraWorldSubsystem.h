// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/Class.h"
#include "CameraWorldSubsystem.generated.h"


class UCameraSettings;

UCLASS()
class SMASHUE_API UCameraWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region Subsystem Overrides
public:
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return TStatId(); };

#pragma endregion

#pragma region Subsystem MainCamera
protected:
	TObjectPtr<UCameraComponent> CameraMain;
	float CurrentPositionDampingInterpolant = 0.0f;
	float CurrentSizeDampingInterpolant = 0.0f;
	FVector CameraPositionTarget;
	FVector CameraPositionStart;
	void TickUpdateCameraPosition(float DeltaTime);
	void TickUpdateCameraZoom(float DeltaTime);

#pragma endregion

#pragma region Follow Targets
public:
	void AddFollowTarget(UObject* FollowTarget);
	void RemoveFollowTarget(UObject* FollowTarget);

protected:
	UPROPERTY()
	TArray<UObject*> FollowTargets;
	
	FVector CalculateAveragePositionBetweenTargets();

	float CalculateGreatestDistanceBetweenTargets();

#pragma endregion

#pragma region Misc
	
protected:
	UCameraComponent* FindCameraByTag(const FName& Tag) const;
	
#pragma endregion

#pragma region Bounds

protected:
	UPROPERTY()
	FVector2D CameraBoundsMin;

	UPROPERTY()
	FVector2D CameraBoundsMax;

	UPROPERTY()
	float CameraBoundsYProjectionCenter;

	const UCameraSettings* CameraSettings;

	AActor* FindCameraBoundsActor();
	void InitCameraBounds(AActor* CameraBoundsActor);
	void ClampPositionIntoCameraBounds(FVector& Position);
	void GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax);
	FVector CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition);
	
#pragma endregion

#pragma region Zoom
protected:
	UPROPERTY()
	float CameraZoomYMin = 0.0f;
	UPROPERTY()
	float CameraZoomYMax = 0.0f;

	UFUNCTION()
	void InitCameraZoomParameters();
#pragma endregion
};
