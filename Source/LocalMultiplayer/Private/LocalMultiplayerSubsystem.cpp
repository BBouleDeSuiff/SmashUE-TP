// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerSubsystem.h"

#include "EnhancedInputSubsystems.h"
#include "LocalMultiplayerSettings.h"
#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerSubsystem::CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType)
{
	auto settings = GetDefault<ULocalMultiplayerSettings>();

	if(settings == nullptr) return;

	for (uint8 i = 0; i < settings->GetNbKeyBoardProfiles() + settings->NbMaxGamepads; i++)
	{
		UGameplayStatics::CreatePlayer(this, i);		
	}
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromKeyboardProfileIndex(int KeyboardProfileIndex)
{
	if(PlayerIndexFromKeyboardProfileIndex.Find(KeyboardProfileIndex))
		return PlayerIndexFromKeyboardProfileIndex[KeyboardProfileIndex];
	else
		return -1;

}

int ULocalMultiplayerSubsystem::AssignNewPlayerToKeyboardProfile(int KeyboardProfileIndex)
{
	auto nextIndext = LastAssignedPlayerIndex;
	PlayerIndexFromKeyboardProfileIndex.Add(KeyboardProfileIndex, nextIndext);
	LastAssignedPlayerIndex++;
	return nextIndext;
}

void ULocalMultiplayerSubsystem::AssignKeyboardMapping(int PlayerIndex, int KeyboardProfileIndex,
	ELocalMultiplayerInputMappingType MappingType) const
{
	FLocalMultiplayerProfileData KeyboardProfileData = GetDefault<ULocalMultiplayerSettings>()->KeyBoardProfilesData[KeyboardProfileIndex];
	UInputMappingContext* IMC = KeyboardProfileData.GetIMCFromType(MappingType);
	ULocalPlayer* LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(PlayerIndex);

	if(IMC && LocalPlayer)
	{
		if(UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->AddMappingContext(IMC, 1);
		}
	}
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromGamepadDeviceID(int DeviceID)
{
	if(PlayerIndexFromGamepadProfileIndex.Find(DeviceID))
		return PlayerIndexFromGamepadProfileIndex[DeviceID];
	else
		return -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToGamepadDeviceID(int DeviceID)
{
	LastAssignedPlayerIndex++;
	PlayerIndexFromGamepadProfileIndex.Add(DeviceID, LastAssignedPlayerIndex);
	return LastAssignedPlayerIndex;
}

void ULocalMultiplayerSubsystem::AssignGamepadInputMapping(int PlayerIndex,
	ELocalMultiplayerInputMappingType MappingType) const
{
	FLocalMultiplayerProfileData GamepadProfileData = GetDefault<ULocalMultiplayerSettings>()->GamepadProfilesData[PlayerIndex];
	UInputMappingContext* IMC = GamepadProfileData.GetIMCFromType(MappingType);
	ULocalPlayer* LocalPlayer = GetGameInstance()->GetLocalPlayerByIndex(PlayerIndex);

	if(IMC && LocalPlayer)
	{
		if(UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->AddMappingContext(IMC, 1);
		}
	}
}
