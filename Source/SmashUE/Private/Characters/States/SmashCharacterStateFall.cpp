// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateFall.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"


ESmashCharacterStateID USmashCharacterStateFall::GetStateID()
{
	return ESmashCharacterStateID::Fall;
}

void USmashCharacterStateFall::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	Character->GetCharacterMovement()->GravityScale = FallGravityScale;
}

void USmashCharacterStateFall::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
}

void USmashCharacterStateFall::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	Character->GetCharacterMovement()->GravityScale = Character->GetInputMoveY() < 0 ? FallFastGravityScale : FallGravityScale;
	
	if(!Character->GetMovementComponent()->IsFalling())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Idle);
		return;
	}

	if(FMath::Abs(Character->GetInputMoveX()) > GetDefault<USmashCharacterSettings>()->InputMoveXThreshold)
	{
		Character->Move(Character->GetInputMoveX(), FallHorizontalMoveSpeed * FallAirControl);
	}
}

