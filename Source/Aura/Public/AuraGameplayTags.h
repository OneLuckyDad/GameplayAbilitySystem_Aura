// Copyright OneLuckyDad

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *  AuraGameplayTags
 *  Singleton containing native Gameplay Tags
 */
class AURA_API FAuraGameplayTags
{
public:
	static FAuraGameplayTags& Get() { return GameplayTags; };
	void InitializeNativeGameplayTags();
	
	FGameplayTag Attributes_Secondary_Armor;
	
private:
	static FAuraGameplayTags GameplayTags;
};
