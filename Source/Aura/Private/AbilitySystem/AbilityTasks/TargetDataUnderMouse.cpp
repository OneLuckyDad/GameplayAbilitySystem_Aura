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
		// TODO: We are on the server, so listen for target data
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
