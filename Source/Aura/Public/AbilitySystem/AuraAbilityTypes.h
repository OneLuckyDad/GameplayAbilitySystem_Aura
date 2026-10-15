#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }
	
	void SetIsBlockHit(bool Value) { bIsBlockedHit = Value; }
	void SetIsCriticalHit(bool Value) { bIsCriticalHit = Value; }
	
	virtual UScriptStruct* GetScriptStruct() const override; // This must be overridden by every subclass!
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override; // This must be overridden by every subclass!
	virtual FAuraGameplayEffectContext* Duplicate() const;
	
protected:
	UPROPERTY()
	bool bIsBlockedHit{ false };
	
	UPROPERTY()
	bool bIsCriticalHit{ false };
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true,
	};
};
