#include "Camera/CameraWorldSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraFollowTarget.h"
#include "Camera/CameraSettings.h"
#include "Kismet/GameplayStatics.h"

void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CameraSettings = GetDefault<UCameraSettings>();
	CameraMain = FindCameraByTag(CameraSettings->CameraMainTag);


	AActor* CameraBoundsActor = FindCameraBoundsActor();
	if(CameraBoundsActor != nullptr)
	{
		InitCameraBounds(CameraBoundsActor);
	}

	InitCameraZoomParameters();
	CameraPositionStart = CameraMain->GetComponentLocation();
	CameraPositionTarget = CameraMain->GetComponentLocation();
	CurrentPositionDampingInterpolant = 0;
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TickUpdateCameraZoom(DeltaTime);
	TickUpdateCameraPosition(DeltaTime);
}

void UCameraWorldSubsystem::AddFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Add(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(UObject* FollowTarget)
{
	FollowTargets.Remove(FollowTarget);
}

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	
	if(CurrentPositionDampingInterpolant >= 1)
	{
		CurrentPositionDampingInterpolant = 0;
		CameraPositionStart = CameraMain->GetComponentLocation();
		FVector NewLocation = CalculateAveragePositionBetweenTargets();
		ClampPositionIntoCameraBounds(NewLocation);
		CameraPositionTarget = FVector(NewLocation.X, CameraPositionTarget.Y, NewLocation.Z);
	}
	else
	{
		CurrentPositionDampingInterpolant += DeltaTime * CameraSettings->PositionDampingFactor;
	}

	CameraMain->SetWorldLocation(FMath::Lerp(CameraPositionStart, CameraPositionTarget, CurrentPositionDampingInterpolant));
}

void UCameraWorldSubsystem::TickUpdateCameraZoom(float DeltaTime)
{
	if(CameraMain == nullptr) return;

	if(CurrentSizeDampingInterpolant >= 1)
	{
		CurrentSizeDampingInterpolant = 0;
		float GreatestDistanceBetweenTargets = CalculateGreatestDistanceBetweenTargets();
		float newY = FMath::GetMappedRangeValueClamped(FVector2D(CameraSettings->DistanceBetweenTargetsMin, CameraSettings->DistanceBetweenTargetsMax),
													 FVector2D(CameraZoomYMin,CameraZoomYMax),
													              GreatestDistanceBetweenTargets);
		CameraPositionStart = CameraMain->GetComponentLocation();
		CameraPositionTarget = FVector(CameraPositionTarget.X, newY, CameraPositionTarget.Z);
	}
	else
	{
		CurrentSizeDampingInterpolant += DeltaTime * CameraSettings->SizeDampingFactor;
	}

	CameraMain->SetWorldLocation(FMath::Lerp(CameraPositionStart, CameraPositionTarget, CurrentSizeDampingInterpolant));
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets()
{
	FVector SumPosition = FVector::ZeroVector;
	for (auto Target : FollowTargets)
	{
		ICameraFollowTarget* FollowTarget = Cast<ICameraFollowTarget>(Target);
		if(FollowTarget != nullptr && FollowTarget->IsFollowable())
		{
			SumPosition += FollowTarget->GetFollowPosition();			
		}
	}
	return SumPosition/FollowTargets.Num();
}

float UCameraWorldSubsystem::CalculateGreatestDistanceBetweenTargets()
{
	float GreatestDistance = 0.f;
	for (auto i : FollowTargets)
	{
		ICameraFollowTarget* FollowTarget1 = Cast<ICameraFollowTarget>(i);
		if(FollowTarget1 != nullptr && FollowTarget1->IsFollowable())
		{
			for (auto j : FollowTargets)
			{
				ICameraFollowTarget* FollowTarget2 = Cast<ICameraFollowTarget>(j);
				if(FollowTarget2 != nullptr && FollowTarget2->IsFollowable())
				{
					float dist = (FollowTarget1->GetFollowPosition() - FollowTarget2->GetFollowPosition()).Length();
					GreatestDistance =  FMath::Max(GreatestDistance, dist);
				}
			}			
		}
	}
	return GreatestDistance;
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, Actors);

	return Actors[0]->FindComponentByClass<UCameraComponent>();
}

AActor* UCameraWorldSubsystem::FindCameraBoundsActor()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CameraSettings->CameraBoundsTag, Actors);

	return Actors[0];
}

void UCameraWorldSubsystem::InitCameraBounds(AActor* CameraBoundsActor)
{
	FVector BoundsCenter;
	FVector BoundsExtents;
	CameraBoundsActor->GetActorBounds(false, BoundsCenter, BoundsExtents);

	CameraBoundsYProjectionCenter = BoundsCenter.Y;
	CameraBoundsMin = FVector2D( BoundsCenter.X - BoundsExtents.X, BoundsCenter.Z - BoundsExtents.Z );
	CameraBoundsMax = FVector2D( BoundsCenter.X + BoundsExtents.X, BoundsCenter.Z + BoundsExtents.Z );
}

void UCameraWorldSubsystem::ClampPositionIntoCameraBounds(FVector& Position)
{
	FVector2D ViewportBoundsMin,ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin, ViewportBoundsMax);
    
	FVector WorldBoundsMin = CalculateWorldPositionFromViewportPosition(ViewportBoundsMin);
	FVector WorldBoundsMax = CalculateWorldPositionFromViewportPosition(ViewportBoundsMax);

	FVector2D WorldBoundSize(
		(WorldBoundsMax.X - WorldBoundsMin.X) / 2.f,
		(WorldBoundsMin.Z - WorldBoundsMax.Z) / 2.f
	);
	
	Position.X = FMath::Clamp(Position.X, CameraBoundsMin.X + WorldBoundSize.X, CameraBoundsMax.X - WorldBoundSize.X);
	Position.Z = FMath::Clamp(Position.Z, CameraBoundsMin.Y + WorldBoundSize.Y, CameraBoundsMax.Y - WorldBoundSize.Y);
	
}

void UCameraWorldSubsystem::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax)
{
	//Find Viewport
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if(ViewportClient != nullptr) return;

	FViewport* Viewport = ViewportClient->Viewport;
	if(Viewport != nullptr) return;

	//Calculate Viewport Rect according to Camera Aspect Ratio and Viewport ViewRect

	FIntRect ViewRect(
		Viewport->GetInitialPositionXY(),
		Viewport->GetInitialPositionXY() + Viewport->GetSizeXY()
	);

	FIntRect ViewportRect = Viewport->CalculateViewExtents(CameraMain->AspectRatio, ViewRect);

	//Fill Output parameters with ViewportRect

	OutViewportBoundsMin.X = ViewportRect.Min.X;
	OutViewportBoundsMin.Y = ViewportRect.Min.Y;
	
	OutViewportBoundsMax.X = ViewportRect.Max.X;
	OutViewportBoundsMax.Y = ViewportRect.Max.Y;	
}

FVector UCameraWorldSubsystem::CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition)
{
	if(CameraMain == nullptr) return FVector::Zero();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(PlayerController == nullptr) return FVector::Zero();

	float YDistanceToCenter = CameraMain->GetComponentLocation().Y - CameraBoundsYProjectionCenter;

	FVector CameraWorldProjectDir;
	FVector WorldPosition;
	UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,
		ViewportPosition,
		WorldPosition,
		CameraWorldProjectDir
	);

	WorldPosition += CameraWorldProjectDir * YDistanceToCenter;

	return WorldPosition;
}

void UCameraWorldSubsystem::InitCameraZoomParameters()
{
	TArray<AActor*> ActorsMin;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CameraSettings->CameraDistanceMinTag, ActorsMin);
	TArray<AActor*> ActorsMax;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CameraSettings->CameraDistanceMaxTag, ActorsMax);

	if(!ActorsMin.IsEmpty())
	{
		CameraZoomYMin = ActorsMin[0]->GetActorLocation().Y;		
	}
	if(!ActorsMax.IsEmpty())
	{
		CameraZoomYMax = ActorsMax[0]->GetActorLocation().Y;		
	}
}
