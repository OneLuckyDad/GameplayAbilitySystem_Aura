// Copyright OneLuckyDad

#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	
	auto* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	for (auto& TagToAttributePair : AuraAttributeSet->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TagToAttributePair.Value()).AddLambda(
		   [this, TagToAttributePair](const FOnAttributeChangeData& Data)
		   {
		   		BroadcastAttributeInfo(TagToAttributePair.Key, TagToAttributePair.Value());
		   }
	   );
	}
}

void UAttributeMenuWidgetController::BroadCastInitialValues()
{
	Super::BroadCastInitialValues();
	
	auto* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);	
	for (auto& TagToAttributePair : AuraAttributeSet->TagsToAttributes)
	{
		BroadcastAttributeInfo(TagToAttributePair.Key, TagToAttributePair.Value());
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& GameplayAttribute) const
{
	check (AttributeInfo);
	
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = GameplayAttribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
