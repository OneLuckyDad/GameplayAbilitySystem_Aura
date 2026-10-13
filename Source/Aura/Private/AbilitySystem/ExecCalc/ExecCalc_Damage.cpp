// Copyright OneLuckyDad

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

FAuraDamageStatics::FAuraDamageStatics()
{
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
}

const FAuraDamageStatics& FAuraDamageStatics::Get()
{
	static FAuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(FAuraDamageStatics::Get().ArmorDef);
	RelevantAttributesToCapture.Add(FAuraDamageStatics::Get().BlockChanceDef);
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
	
	// Get damage set by caller magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);
	
	// Capture BlockChance on Target and determine if there was a successful Block. If blocking, half the Damage.
	float TargetBlockChance = 0.f;
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(FAuraDamageStatics::Get().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Clamp(TargetBlockChance, 0.f, 100.f);
	
	const bool bBlocked = FMath::RandRange(0.f, 100.f) < TargetBlockChance;
	if (bBlocked)
	{
		constexpr float DamageMultiplier = .5f;
		Damage *= DamageMultiplier;
	}
	
	const FGameplayModifierEvaluatedData EvaluatedData{ UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage };
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
