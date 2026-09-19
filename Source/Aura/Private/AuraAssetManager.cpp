// Copyright OneLuckyDad

#include "AuraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	auto* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	FAuraGameplayTags::Get().InitializeNativeGameplayTags();
	UAbilitySystemGlobals::Get().InitGlobalData(); // Not needed in UE 5.3+ but harmless to leave there
}
