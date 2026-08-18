// Copyright OneLuckyDad

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply,
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	NeverRemove,
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:
	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);
	
	UFUNCTION(BlueprintCallable)
	void OnBeginOverlap(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects")
	bool bDestroyOnEffectRemoval{ false };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects", meta=(EditCondition = "InstantGameplayEffectClass != nullptr", EditConditionHides))
	EEffectApplicationPolicy InstantEffectApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects", meta=(EditCondition = "DurationGameplayEffectClass != nullptr", EditConditionHides))
	EEffectApplicationPolicy DurationEffectApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects", meta=(EditCondition = "InfiniteGameplayEffectClass != nullptr", EditConditionHides))
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects", meta=(EditCondition = "InfiniteGameplayEffectClass != nullptr", EditConditionHides))
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy{ EEffectRemovalPolicy::RemoveOnEndOverlap };

	UPROPERTY()
	TMap<uint32, FActiveGameplayEffectHandle> ActiveRemovableInfiniteEffects;
};
