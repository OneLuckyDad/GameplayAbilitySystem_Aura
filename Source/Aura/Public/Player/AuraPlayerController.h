// Copyright OneLuckyDad

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class IEnemyInterface;
class UInputAction;
class UInputMappingContext;
class USplineComponent;
struct FInputActionValue;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlayingState() override;
	virtual void EndPlayingState() override;;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:	
	void Move(const FInputActionValue& InputActionValue);
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	void CursorTrace();

	UPROPERTY(Transient)
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;
	
	// Point and click movement
	FVector CachedDestination{ FVector::ZeroVector };
	
	UPROPERTY(VisibleAnywhere, Category = "Point and click movement")
	TObjectPtr<USplineComponent> Spline;
	
	float FollowTime{ 0.0f };
	float ShortPressThreshold{ 0.5f };
	
	UPROPERTY(EditDefaultsOnly, Category = "Point and click movement")
	float AutoRunAcceptanceRadius{ 50.0f };
	
	bool bAutoRunning{ false };
	bool bTargeting{ false };
	
	// Highlighting enemy under cursor
	UPROPERTY(Transient)
	TScriptInterface<IEnemyInterface> LastActor{ nullptr };
	
	UPROPERTY(Transient)
	TScriptInterface<IEnemyInterface> ThisActor{ nullptr };
};
