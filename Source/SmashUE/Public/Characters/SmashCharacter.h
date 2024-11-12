#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SmashCharacter.generated.h"

class USmashCharacterStateMachine;

UCLASS()
class SMASHUE_API ASmashCharacter : public ACharacter
{
    GENERATED_BODY()

#pragma region Unreal Default
public:
    // Sets default values for this character's properties
    ASmashCharacter();
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

#pragma endregion
#pragma region Orient

public:
    float GetOrientX() const;
    void SetOrientX(float NewOrientX);

protected:
    UPROPERTY(BlueprintReadOnly)
    float OrientX = 1.f;
    
    void RotateMeshUsingOrientX() const;
 
#pragma endregion

#pragma region Movement

public:
    void Move(float InputX, float Speed);

 
#pragma endregion

#pragma region State Machine
    
public:
    void CreateStateMachine();
    void InitStateMachine();
    void TickStateMachine(float DeltaTime) const;
protected:
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<USmashCharacterStateMachine> StateMachine;
    
#pragma endregion

};