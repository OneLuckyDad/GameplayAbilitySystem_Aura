// Copyright OneLuckyDad

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAbilityTypes.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

FAuraDamageStatics::FAuraDamageStatics()
{
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Source, false);
	DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
}

const FAuraDamageStatics& FAuraDamageStatics::Get()
{
	static FAuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(FAuraDamageStatics::Get().ArmorDef);
	RelevantAttributesToCapture.Add(FAuraDamageStatics::Get().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(FAuraDamageStatics::Get().BlockChanceDef);
	RelevantAttributesToCapture.Add(FAuraDamageStatics::Get().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(FAuraDamageStatics::Get().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(FAuraDamageStatics::Get().CriticalHitDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParameters, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParameters.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	// Get damage set by caller magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);
	
	// Capture BlockChance on Target and determine if there was a successful Block.
	float TargetBlockChance = 0.f;
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(FAuraDamageStatics::Get().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max(TargetBlockChance, 0.f);
	
	const bool bBlocked = FMath::RandRange(0.f, 100.f) < TargetBlockChance;
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	if (bBlocked)
	{
		// If blocking, half the Damage.
		constexpr float DamageMultiplier = .5f;
		Damage *= DamageMultiplier;
	}
	
	float TargetArmor = 0.f;
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(FAuraDamageStatics::Get().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max(TargetArmor, 0.f);
	
	float SourceArmorPenetration = 0.f;
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(FAuraDamageStatics::Get().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(SourceArmorPenetration, 0.f);
	
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParameters.GetSourceAbilitySystemComponent();
	const AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	const auto* SourceCombatInterface =  CastChecked<ICombatInterface>(SourceActor);
	
	const UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParameters.GetTargetAbilitySystemComponent();
	const AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;
	const auto* TargetCombatInterface =  CastChecked<ICombatInterface>(TargetActor);
	
	UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceActor);
	
	// Armor penetration ignores a percentage of the target Armor
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName(TEXT("ArmorPenetration")), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
	const float EffectiveArmor = TargetArmor * ((100.f - SourceArmorPenetration * ArmorPenetrationCoefficient ) / 100.f);
	
	// Armor ignores a  percentage of the Target damage
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName(TEXT("EffectiveArmor")), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	float SourceCriticalHitChance = 0.f;
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(FAuraDamageStatics::Get().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max(SourceCriticalHitChance, 0.f);
	
	float TargetCriticalHitResistance = 0.f;
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(FAuraDamageStatics::Get().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max(TargetCriticalHitResistance, 0.f);
	
	float SourceCriticalHitDamage = 0.f;
	ExecutionParameters.AttemptCalculateCapturedAttributeMagnitude(FAuraDamageStatics::Get().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max(SourceCriticalHitDamage, 0.f);
	
	// Critical hit resistance reduce critical hit chance by a certain percentage
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName(TEXT("CriticalHitResistance")), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	
	const bool bIsCriticalHit = FMath::RandRange(1.f, 100.f) < EffectiveCriticalHitChance;
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCriticalHit);
	if (bIsCriticalHit)
	{
		// Double damage plus a bonus damage on critical hit
		Damage = (2.f * Damage) + SourceCriticalHitDamage;
	}
	
	const FGameplayModifierEvaluatedData EvaluatedData{ UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage };
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
