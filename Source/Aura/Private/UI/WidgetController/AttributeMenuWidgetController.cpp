// Copyright OneLuckyDad

#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
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
	
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	Info.AttributeValue = AuraAttributeSet->GetStrength();
	AttributeInfoDelegate.Broadcast(Info);
}
