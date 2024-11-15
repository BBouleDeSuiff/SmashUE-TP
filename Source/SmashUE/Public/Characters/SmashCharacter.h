#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "SmashCharacter.generated.h"

class UInputMappingContext;
class USmashCharacterStateMachine;
class USmashCharacterInputData;

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

#pragma region Inputs Mapping

public:
    UPROPERTY()
    TObjectPtr<UInputMappingContext> InputMappingContext;
    UPROPERTY()
    TObjectPtr<USmashCharacterInputData> InputData;
protected:
    void SetupMappingContextIntoController() const;

 
#pragma endregion
    
#pragma region Inputs Move X

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputMoveXEvent, float, InputMoveX);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputJump);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputFallFastEvent, float, InputMoveY);
    
public:
    float GetInputMoveX() const;
    float GetInputMoveY() const;
    UPROPERTY()
    FInputMoveXEvent InputMoveXFastEvent;
    UPROPERTY()
    FInputJump InputJumpEvent;
    UPROPERTY()
    FInputFallFastEvent InputFallFastEvent;
protected:
    UPROPERTY()
    float InputMoveX = 0.f;
    UPROPERTY()
    float InputMoveY = 0.f;
private:
    void BindInputMoveXAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);
    void OnInputMoveX(const FInputActionValue& InputActionValue);
    void OnInputMoveXFast(const FInputActionValue& InputActionValue);
    void OnInputJump(const FInputActionValue& InputActionValue);
    void OnInputFallFast(const FInputActionValue& InputActionValue);


#pragma endregion

};