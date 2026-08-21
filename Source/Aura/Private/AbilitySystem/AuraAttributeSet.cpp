// Copyright OneLuckyDad

#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitMaxHealth(100.f);
	InitMaxMana(50.f);

	InitHealth(GetMaxHealth() * .5f);
	InitMana(GetMaxMana() * .5f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	ClampAttribute(Attribute, NewValue);
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FEffectProperties Properties = GetEffectProperties(Data);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

FEffectProperties UAuraAttributeSet::GetEffectProperties(const FGameplayEffectModCallbackData& Data)
{
	FEffectProperties OutProperties;
	
	OutProperties.EffectContextHandle = Data.EffectSpec.GetContext();
	
	// Source
	OutProperties.SourceAbilitySystemComponent = OutProperties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(OutProperties.SourceAbilitySystemComponent) &&
		OutProperties.SourceAbilitySystemComponent->AbilityActorInfo.IsValid() &&
		OutProperties.SourceAbilitySystemComponent->AbilityActorInfo->AvatarActor.IsValid())
	{
		OutProperties.SourceAvatarActor = OutProperties.SourceAbilitySystemComponent->AbilityActorInfo->AvatarActor.Get();
		OutProperties.SourceController = OutProperties.SourceAbilitySystemComponent->AbilityActorInfo->PlayerController.Get();
		if (!OutProperties.SourceController && OutProperties.SourceAvatarActor)
		{
			if (APawn* Pawn = Cast<APawn>(OutProperties.SourceAvatarActor))
			{
				OutProperties.SourceController = Pawn->GetController();
			}
		}
		if (OutProperties.SourceController)
		{
			ACharacter* SourceCharacter = Cast<ACharacter>(OutProperties.SourceController->GetPawn()); 
		}
	}
	
	// Target
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		OutProperties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();	
		OutProperties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		OutProperties.TargetCharacter = Cast<ACharacter>(OutProperties.TargetAvatarActor);
		OutProperties.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OutProperties.TargetAvatarActor);
	}
	
	return OutProperties;
}

void UAuraAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	
	if (Attribute == GetMaxHealthAttribute() || Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}
