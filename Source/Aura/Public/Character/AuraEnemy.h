// Copyright OneLuckyDad

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	virtual void BeginPlay() override;

	// Enemy Interface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	// end Enemy Interface
	
	// Combat Interface
	virtual int32 GetPlayerLevel() const override;
	
	virtual void Die() override;
	// end Combat Interface
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
protected:
	virtual void InitDefaultAttributes() const override;
	virtual void InitAbilityActorInfo() override;
	void InitHealthWidget();
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Default")
	ECharacterClass CharacterClass{ ECharacterClass::Warrior };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Default")
	int32 Level{ 1 };
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float BaseWalkSpeed{ 250.f };
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float LifespanOnDeath{ 5.f };
	
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bHitReacting{ false };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};
