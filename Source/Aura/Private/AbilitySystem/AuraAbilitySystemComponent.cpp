// Copyright OneLuckyDad

#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraAbilitySystemComponent::OnAbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbility(const TSubclassOf<UGameplayAbility> Ability)
{
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
	//GiveAbility(AbilitySpec);
	GiveAbilityAndActivateOnce(AbilitySpec);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	for (const auto AbilityClass : Abilities)
	{
		AddCharacterAbility(AbilityClass);
	}
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	OnEffectAssetTags.Broadcast(TagContainer);
}
