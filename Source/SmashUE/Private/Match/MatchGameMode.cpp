#include "Match/MatchGameMode.h"
#include "Characters/SmashCharacter.h"
#include "Arena/ArenaPlayerStart.h"
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
        NewCharacter->AutoPossessPlayer = SpawnPoint->AutoReceiveInput;
        NewCharacter->SetOrientX(SpawnPoint->GetStartOrientX());
        NewCharacter->FinishSpawning(SpawnPoint->GetTransform());

        CharactersInsideArena.Add(NewCharacter);
    }
}

void AMatchGameMode::BeginPlay()
{
    Super::BeginPlay();

    TArray<AArenaPlayerStart*> PlayerStartsPoints;
    FindPlayerStartActorsInArena(PlayerStartsPoints);
    SpawnCharacters(PlayerStartsPoints);
}

TSubclassOf<ASmashCharacter> AMatchGameMode::GetSmashCharacterClassFromInputType(EAutoReceiveInput::Type InputType) const
{
    switch (InputType)
    {
    case EAutoReceiveInput::Player0:
        return SmashCharacterClassP0;
    case EAutoReceiveInput::Player1:
        return SmashCharacterClassP1;
    case EAutoReceiveInput::Player2:
        return SmashCharacterClassP2;
    case EAutoReceiveInput::Player3:
        return SmashCharacterClassP3;
    default:
        return nullptr;
    }
}