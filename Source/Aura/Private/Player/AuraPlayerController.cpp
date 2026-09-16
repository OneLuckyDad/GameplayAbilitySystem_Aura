// Copyright OneLuckyDad

#include "Player/AuraPlayerController.h"

#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Inputs/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	TickAutoRun();
}

void AAuraPlayerController::BeginPlayingState()
{
	Super::BeginPlayingState();
	AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	check(AuraAbilitySystemComponent);
}

void AAuraPlayerController::EndPlayingState()
{
	Super::EndPlayingState();
	AuraAbilitySystemComponent = nullptr;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	auto* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::TickAutoRun()
{
	if (const TObjectPtr<APawn> ControlledPawn = GetPawn();
		bAutoRunning && ControlledPawn)
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		if (const float DistanceToDestinationSquared = (LocationOnSpline - CachedDestination).SquaredLength();
			DistanceToDestinationSquared <= AutoRunAcceptanceRadius * AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LeftMouseButton))
	{
		bTargeting = ThisActor != nullptr;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{	
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LeftMouseButton))
	{
		if (bTargeting && AuraAbilitySystemComponent)
		{
			AuraAbilitySystemComponent->AbilityInputTagReleased(InputTag);
		}
		else if (!bTargeting)
		{
			if (const TObjectPtr<APawn> ControlledPawn = GetPawn();
				ControlledPawn && FollowTime <= ShortPressThreshold)
			{
				if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
				{
					Spline->ClearSplinePoints();
					for (const FVector& PointLocation : NavPath->PathPoints)
					{
						Spline->AddSplineWorldPoint(PointLocation);
					}
					CachedDestination = NavPath->PathPoints.Last();
					bAutoRunning = true;
				}
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
	else if (AuraAbilitySystemComponent)
	{
		AuraAbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LeftMouseButton))
	{
		if (bTargeting && AuraAbilitySystemComponent)
		{
			AuraAbilitySystemComponent->AbilityInputTagHeld(InputTag);
		}
		else if (!bTargeting)
		{
			FollowTime += GetWorld()->GetDeltaSeconds();

			if (CursorHit.bBlockingHit)
			{
				CachedDestination = CursorHit.ImpactPoint;
			}
		
			if (const TObjectPtr<APawn> ControlledPawn = GetPawn())
			{
				const FVector WorldDirection =  (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
				ControlledPawn->AddMovementInput(WorldDirection);
			}
		}
	}
	else if (AuraAbilitySystemComponent)
	{
		AuraAbilitySystemComponent->AbilityInputTagHeld(InputTag);
	}
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (CursorHit.bBlockingHit)
	{
		LastActor = ThisActor;
		ThisActor = CursorHit.GetActor();

		if (LastActor != ThisActor)
		{
			if (LastActor)
			{
				LastActor->UnHighlightActor();
			}
			if (ThisActor)
			{
				ThisActor->HighlightActor();
			}
		}
	}
}
