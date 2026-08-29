// Copyright OneLuckyDad

#pragma once

#include "CoreMinimal.h"

/**
 *  AuraGameplayTags
 *  Singleton containing native Gameplay Tags
 */
class AURA_API FAuraGameplayTags
{
public:
	static FAuraGameplayTags& Get() { return GameplayTags; };
	static void InitializeNativeGameplayTags();
	
private:
	static FAuraGameplayTags GameplayTags;
};
