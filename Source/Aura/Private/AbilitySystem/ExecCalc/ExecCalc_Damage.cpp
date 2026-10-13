// Copyright OneLuckyDad

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

FAuraDamageStatics::FAuraDamageStatics()
{
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
}

const FAuraDamageStatics& FAuraDamageStatics::Get()
{
	static FAuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(FAuraDamageStatics::Get().ArmorDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParameters, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParameters.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParameters.GetTargetAbilitySystemComponent();
	
	const AActor* SourceAvatar = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	const AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;
	
	const FGameplayEffectSpec& Spec = ExecutionParameters.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float Armor{ 0.f };
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(FAuraDamageStatics::Get().ArmorDef, EvaluationParameters, Armor);
	
	Armor = FMath::Max(Armor, 0.f);
	Armor += 1.f;
	
	const FGameplayModifierEvaluatedData EvaluatedData{ FAuraDamageStatics::Get().ArmorProperty, EGameplayModOp::Additive, Armor };
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
