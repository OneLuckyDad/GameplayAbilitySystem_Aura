// Copyright OneLuckyDad

#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadCastInitialValues()
{
	Super::BroadCastInitialValues();

	const auto* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	const auto* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	FOnGameplayAttributeValueChange& HealthChangedDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute());
	HealthChangedDelegate.AddLambda([this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); });

	FOnGameplayAttributeValueChange& MaxHealthChangedDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute());
	MaxHealthChangedDelegate.AddLambda([this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); });

	FOnGameplayAttributeValueChange& ManaChangedDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute());
	ManaChangedDelegate.AddLambda([this](const FOnAttributeChangeData& Data) { OnManaChanged.Broadcast(Data.NewValue); });

	FOnGameplayAttributeValueChange& MaxManaChangedDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute());
	MaxManaChangedDelegate.AddLambda([this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); });
	
	FEffectAssetTagsDelegate& OnEffectAssetTags = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->OnEffectAssetTags;
	OnEffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					const auto* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}	
		});
}
