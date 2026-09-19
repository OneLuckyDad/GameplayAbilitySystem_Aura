// Copyright OneLuckyDad

#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	auto* TargetData = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return TargetData;
}

void UTargetDataUnderMouse::Activate()
{
	Super::Activate();

	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();
		
		FAbilityTargetDataSetDelegate& DataSetDelegate = AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey);
		DataSetDelegate.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		if (const bool bCalledDelegate = AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey);
			!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData() const
{
	FScopedPredictionWindow ScopedPrediction{ AbilitySystemComponent.Get() };
	
	const APlayerController* PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	
	FHitResult CursorHit; 
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	const FGameplayAbilityTargetDataHandle TargetDataHandle{ new FGameplayAbilityTargetData_SingleTargetHit(CursorHit) };
	
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(), 
		GetActivationPredictionKey(),
		TargetDataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTargetDataValid.Broadcast(TargetDataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag ActivationTag) const
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnTargetDataValid.Broadcast(TargetDataHandle);	
	}
}
