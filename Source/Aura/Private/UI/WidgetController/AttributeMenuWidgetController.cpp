// Copyright OneLuckyDad

#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
}

void UAttributeMenuWidgetController::BroadCastInitialValues()
{
	Super::BroadCastInitialValues();
	
	auto* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	
	check (AttributeInfo);
	
	for (auto& Pair : AuraAttributeSet->TagsToAttributes)
	{
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
		Info.AttributeValue = Pair.Value().GetNumericValue(AuraAttributeSet);
		AttributeInfoDelegate.Broadcast(Info);
	}
}
