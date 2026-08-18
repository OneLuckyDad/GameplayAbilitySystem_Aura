// Copyright OneLuckyDad

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy{ EEffectApplicationPolicy::DoNotApply };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Apply EffectsW")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy{ EEffectRemovalPolicy::RemoveOnEndOverlap };
};
