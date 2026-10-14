// Copyright OneLuckyDad

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

struct FAuraDamageStatics
{
	static const FAuraDamageStatics& Get();
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	
private:
	FAuraDamageStatics();
};

UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UExecCalc_Damage();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParameters, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
