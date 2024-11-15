// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateJump.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"


ESmashCharacterStateID USmashCharacterStateJump::GetStateID()
{
	return ESmashCharacterStateID::Jump;
}

void USmashCharacterStateJump::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	float t = JumpDuration;
	float h = JumpMaxHeight;
	Character->GetCharacterMovement()->JumpZVelocity = 2 * h / t;
	Character->GetCharacterMovement()->GravityScale = (-2 * h / (t * t)) / -981;

	Character->Jump();

	AnimationInstance->OnMontageEnded.AddDynamic(this, &USmashCharacterStateJump::OnFall);
}

void USmashCharacterStateJump::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
}

void USmashCharacterStateJump::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Magenta,
		FString::Printf(TEXT("Move Y : %d"), Character->GetInputMoveY())
	);
	if(Character->GetInputMoveY() < 0)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
		return;
	}
	
	if(!Character->GetMovementComponent()->IsFalling())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Idle);
		return;
	}

	if(FMath::Abs(Character->GetInputMoveX())> GetDefault<USmashCharacterSettings>()->InputMoveXThreshold)
	{
		Character->Move(Character->GetInputMoveX(), JumpAirControl * JumpWalkSpeed);
	}
}

void USmashCharacterStateJump::OnFall(UAnimMontage* Montage, bool bInterrupted)
{
	if(Character->GetMovementComponent()->IsFalling())
	{
		AnimationInstance->OnMontageEnded.RemoveDynamic(this, &USmashCharacterStateJump::OnFall);
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
	}
}

