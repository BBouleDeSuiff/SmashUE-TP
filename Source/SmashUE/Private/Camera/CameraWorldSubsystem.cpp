// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraWorldSubsystem.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CameraMain = FindCameraByTag(TEXT("CameraMain"));
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickUpdateCameraPosition(DeltaTime);
}

void UCameraWorldSubsystem::AddFollowTarget(AActor* FollowTarget)
{
	FollowTargets.Add(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(AActor* FollowTarget)
{
	FollowTargets.Remove(FollowTarget);
}

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	FVector NewLocation = CalculateAveragePositionBetweenTargets();
	CameraMain->SetWorldLocation(FVector(NewLocation.X, CameraMain->GetComponentLocation().Y, NewLocation.Z));
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets()
{
	FVector SumPosition = FVector::ZeroVector;
	for (auto Target : FollowTargets)
	{
		SumPosition += Target->GetActorLocation();
	}
	return SumPosition/FollowTargets.Num();
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, Actors);

	return Actors[0]->FindComponentByClass<UCameraComponent>();
}
