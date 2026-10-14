// Copyright OneLuckyDad

#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector ProjectileTargetLocation) const
{
	if (GetAvatarActorFromActorInfo()->HasAuthority())
	{
		if (const auto* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
		{
			const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
			
			FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
			Rotation.Pitch = 0.f;
			
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(Rotation.Quaternion());
			
			auto* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
				ProjectileClass,
				SpawnTransform,
				GetOwningActorFromActorInfo(),
				Cast<APawn>(GetOwningActorFromActorInfo()),
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			const UAbilitySystemComponent* SourceAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
			FGameplayEffectContextHandle EffectContextHandle = SourceAbilitySystemComponent->MakeEffectContext();
			EffectContextHandle.SetAbility(this);
			EffectContextHandle.AddSourceObject(Projectile);
			
			// There are a bit silly set like this, but adding them to check how they get propagated
			TArray<TWeakObjectPtr<AActor>> ContextActors;
			ContextActors.Add(Projectile);
			EffectContextHandle.AddActors(ContextActors);
			FHitResult HitResult;
			HitResult.Location = Projectile->GetActorLocation();
			EffectContextHandle.AddHitResult(HitResult);
			
			const FGameplayEffectSpecHandle SpecHandle = SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

			const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
			const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaledDamage);
			Projectile->DamageEffectSpecHandle = SpecHandle;
			
			Projectile->FinishSpawning(SpawnTransform);
		}
	}
}
