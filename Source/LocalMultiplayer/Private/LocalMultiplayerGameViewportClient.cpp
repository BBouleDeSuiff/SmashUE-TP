// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerGameViewportClient.h"

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerGameViewportClient::PostInitProperties()
{
	MaxSplitscreenPlayers = 8;
	Super::PostInitProperties();
}

bool ULocalMultiplayerGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	const FKey& Key = EventArgs.Key;
	ELocalMultiplayerInputMappingType MappingType = ELocalMultiplayerInputMappingType::InGame;

	ULocalMultiplayerSubsystem* MultiplayerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();
	const ULocalMultiplayerSettings* MultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();

	if (EventArgs.IsGamepad())
	{
		int DeviceID = EventArgs.ControllerId;
		int PlayerIndex = MultiplayerSubsystem->GetAssignedPlayerIndexFromGamepadDeviceID(DeviceID);
		if (PlayerIndex == -1)
		{
			PlayerIndex = MultiplayerSubsystem->AssignNewPlayerToGamepadDeviceID(DeviceID);
			MultiplayerSubsystem->AssignGamepadInputMapping(PlayerIndex, MappingType);
		}

		const FInputKeyParams Params(EventArgs.Key, EventArgs.Event, EventArgs.AmountDepressed, true, EventArgs.InputDevice);
		return UGameplayStatics::GetPlayerController(this, PlayerIndex)->InputKey(Params);
	}
	else
	{
		int KeyboardProfileIndex = MultiplayerSettings->FindKeyBoardProfileIndexFromKey(Key, MappingType);
		if (KeyboardProfileIndex != -1)
		{
			int PlayerIndex = MultiplayerSubsystem->GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardProfileIndex);
			if (PlayerIndex == -1)
			{
				PlayerIndex = MultiplayerSubsystem->AssignNewPlayerToKeyboardProfile(KeyboardProfileIndex);
				MultiplayerSubsystem->AssignKeyboardMapping(PlayerIndex, KeyboardProfileIndex, MappingType);
			}

			FInputKeyParams Params(EventArgs.Key, EventArgs.Event, EventArgs.AmountDepressed, false, EventArgs.InputDevice);
			return UGameplayStatics::GetPlayerController(this, PlayerIndex)->InputKey(Params);
		}
	}

	return Super::InputKey(EventArgs);
}

bool ULocalMultiplayerGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	ULocalMultiplayerSubsystem* MultiplayerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();
	ELocalMultiplayerInputMappingType MappingType = ELocalMultiplayerInputMappingType::InGame;

	if (bGamepad)
	{
		int DeviceID = InputDevice.GetId();
		int PlayerIndex = MultiplayerSubsystem->GetAssignedPlayerIndexFromGamepadDeviceID(DeviceID);
		if (PlayerIndex == -1)
		{
			PlayerIndex = MultiplayerSubsystem->AssignNewPlayerToGamepadDeviceID(DeviceID);
			MultiplayerSubsystem->AssignGamepadInputMapping(PlayerIndex, MappingType);
		}

		FInputKeyParams Params(Key, Delta, DeltaTime, NumSamples, true, InputDevice);
		return UGameplayStatics::GetPlayerController(this, PlayerIndex)->InputKey(Params);
	}
	else
	{
		int KeyboardID = InputDevice.GetId();
		int PlayerIndex = MultiplayerSubsystem->GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardID);
		if (PlayerIndex == -1)
		{
			PlayerIndex = MultiplayerSubsystem->AssignNewPlayerToKeyboardProfile(KeyboardID);
			MultiplayerSubsystem->AssignKeyboardMapping(PlayerIndex, KeyboardID, MappingType);
		}

		FInputKeyParams Params(Key, Delta, DeltaTime, NumSamples, true, InputDevice);
		return UGameplayStatics::GetPlayerController(this, PlayerIndex)->InputKey(Params);
	}

	return Super::InputAxis(InViewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
}