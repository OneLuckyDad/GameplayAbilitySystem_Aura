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

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();
		
		FAbilityTargetDataSetDelegate& DataSetDelegate = AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey);
		DataSetDelegate.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		
		const bool bCalledDelegate = AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey);
		if (!bCalledDelegate)
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
		ValidData.Broadcast(TargetDataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag ActivationTag) const
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(TargetDataHandle);	
	}
}
