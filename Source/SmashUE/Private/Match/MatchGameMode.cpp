#include "Match/MatchGameMode.h"

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"
#include "Characters/SmashCharacter.h"
#include "Arena/ArenaPlayerStart.h"
#include "Arena/ArenaSettings.h"
#include "Characters/SmashCharacterSettings.h"
#include "Kismet/GameplayStatics.h"

void AMatchGameMode::FindPlayerStartActorsInArena(TArray<AArenaPlayerStart*>& ResultsActors)
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArenaPlayerStart::StaticClass(), FoundActors);

    for (int i = 0; i < FoundActors.Num(); ++i)
    {
        AArenaPlayerStart* ArenaPlayerStartActor = Cast<AArenaPlayerStart>(FoundActors[i]);
        if (ArenaPlayerStartActor == nullptr) continue;

        ResultsActors.Add(ArenaPlayerStartActor);
    }
}

void AMatchGameMode::SpawnCharacters(const TArray<AArenaPlayerStart*>& SpawnPoints)
{
    USmashCharacterInputData* InputData = LoadInputDataFromConfig();
    UInputMappingContext* InputMappingContext = LoadInputMappingContextFromConfig();
    
    for (AArenaPlayerStart* SpawnPoint : SpawnPoints)
    {
        EAutoReceiveInput::Type InputType = SpawnPoint->AutoReceiveInput.GetValue();
        TSubclassOf<ASmashCharacter> SmashCharacterClass = GetSmashCharacterClassFromInputType(InputType);
        if (SmashCharacterClass == nullptr) continue;

        ASmashCharacter* NewCharacter = GetWorld()->SpawnActorDeferred<ASmashCharacter>(
            SmashCharacterClass,
            SpawnPoint->GetTransform()
        );

        if (NewCharacter == nullptr) continue;
        NewCharacter->InputData = InputData;
        NewCharacter->InputMappingContext = InputMappingContext;
        NewCharacter->AutoPossessPlayer = SpawnPoint->AutoReceiveInput;
        NewCharacter->SetOrientX(SpawnPoint->GetStartOrientX());
        NewCharacter->FinishSpawning(SpawnPoint->GetTransform());

        CharactersInsideArena.Add(NewCharacter);
    }
}

void AMatchGameMode::BeginPlay()
{
    Super::BeginPlay();
    CreateAndInitPlayers();
    TArray<AArenaPlayerStart*> PlayerStartsPoints;
    FindPlayerStartActorsInArena(PlayerStartsPoints);
    SpawnCharacters(PlayerStartsPoints);
}

USmashCharacterInputData* AMatchGameMode::LoadInputDataFromConfig()
{
    const USmashCharacterSettings* CharacterSettings = GetDefault<USmashCharacterSettings>();
    if (CharacterSettings == nullptr) return nullptr;
    return CharacterSettings->InputData.LoadSynchronous();
}

UInputMappingContext* AMatchGameMode::LoadInputMappingContextFromConfig()
{
    const USmashCharacterSettings* CharacterSettings = GetDefault<USmashCharacterSettings>();
    if (CharacterSettings == nullptr) return nullptr;
    return CharacterSettings->InputMappingContext.LoadSynchronous();
}

TSubclassOf<ASmashCharacter> AMatchGameMode::GetSmashCharacterClassFromInputType(EAutoReceiveInput::Type InputType) const
{
    const UArenaSettings* ArenaSettings = GetDefault<UArenaSettings>();
    switch (InputType)
    {
    case EAutoReceiveInput::Player0:
        return ArenaSettings->SmashCharacterClassP0;
    case EAutoReceiveInput::Player1:
        return ArenaSettings->SmashCharacterClassP1;
    case EAutoReceiveInput::Player2:
        return ArenaSettings->SmashCharacterClassP2;
    case EAutoReceiveInput::Player3:
        return ArenaSettings->SmashCharacterClassP3;
    default:
        return nullptr;
    }
}

void AMatchGameMode::CreateAndInitPlayers() const
{
    UGameInstance* GameInstance = GetWorld()->GetGameInstance();
    if (GameInstance == nullptr) return;
    
    ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstance->GetSubsystem<ULocalMultiplayerSubsystem>();
    if (LocalMultiplayerSubsystem == nullptr) return;

    LocalMultiplayerSubsystem->CreateAndInitPlayers(ELocalMultiplayerInputMappingType::InGame);
}
