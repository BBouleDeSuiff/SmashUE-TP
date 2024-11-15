// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/SmashCharacterStateRun.h"

#include "Characters/SmashCharacter.h"
#include "Characters/SmashCharacterSettings.h"
#include "Characters/SmashCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"


ESmashCharacterStateID USmashCharacterStateRun::GetStateID()
{
	return ESmashCharacterStateID::Run;
}

void USmashCharacterStateRun::StateEnter(ESmashCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	Character->InputJumpEvent.AddDynamic(this, &USmashCharacterStateRun::OnInputJump);
}

void USmashCharacterStateRun::StateExit(ESmashCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	Character->InputJumpEvent.RemoveDynamic(this, &USmashCharacterStateRun::OnInputJump);
}

void USmashCharacterStateRun::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if(Character->GetMovementComponent()->IsFalling())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Fall);
		return;
	}
	
	if(FMath::Abs(Character->GetInputMoveX()) < GetDefault<USmashCharacterSettings>()->InputMoveXThreshold)
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Idle);
	}
	else
	{
		Character->Move(Character->GetInputMoveX(), MoveSpeedMax);
	}
}
void USmashCharacterStateRun::OnInputJump()
{
	if(!Character->GetMovementComponent()->IsFalling())
	{
		StateMachine->ChangeState(ESmashCharacterStateID::Jump);
	}
}

