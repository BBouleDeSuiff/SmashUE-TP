#include "LocalMultiplayerSettings.h"
#include "InputMappingContext.h"
//#include "../../../../../../Documents/UE/UE_5.3/Engine/Plugins/EnhancedInput/Source/EnhancedInput/Public/InputMappingContext.h"

int ULocalMultiplayerSettings::GetNbKeyBoardProfiles() const
{
	return KeyBoardProfilesData.Num();
}

int ULocalMultiplayerSettings::FindKeyBoardProfileIndexFromKey(const FKey Key,
	ELocalMultiplayerInputMappingType MappingType) const
{
	for (int i = 0; i < KeyBoardProfilesData.Num(); ++i)
	{
		const FLocalMultiplayerProfileData& ProfileData = KeyBoardProfilesData[i];
		if (ProfileData.GetIMCFromType(MappingType))
		{
			if(ProfileData.ContainsKey(Key, MappingType))
				return i;
		}
	}
	return -1;
}

bool FLocalMultiplayerProfileData::ContainsKey(const FKey Key, ELocalMultiplayerInputMappingType MappingType) const
{
	UInputMappingContext* IMC = GetIMCFromType(MappingType);
	if (IMC)
	{
		for (const auto& k : IMC->GetMappings())
		{
			if (k.Key == Key)
			{
				return true;
			}
		}
	}
	return false;
}

UInputMappingContext* FLocalMultiplayerProfileData::GetIMCFromType(ELocalMultiplayerInputMappingType MappingType) const
{
	switch(MappingType)
	{
		default:
		case ELocalMultiplayerInputMappingType::InGame:
			return IMCInGame.Get();
		case ELocalMultiplayerInputMappingType::Menu:
			return IMCMenu.Get();
	}
}