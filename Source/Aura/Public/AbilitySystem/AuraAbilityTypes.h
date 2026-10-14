#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
	bool IsBlockHit() const { return bIsBlockHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }
	
	void SetIsBlockHit(bool Value) { bIsBlockHit = Value; }
	void SetIsCriticalHit(bool Value) { bIsCriticalHit = Value; }
	
	virtual UScriptStruct* GetScriptStruct() const override; // This must be overridden by every subclass!
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override; // This must be overridden by every subclass!
	
protected:
	bool bIsBlockHit{ false };
	bool bIsCriticalHit{ false };
};
