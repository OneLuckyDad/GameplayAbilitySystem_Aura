// Copyright OneLuckyDadS

#include "UI/HUD/AuraHUD.h"

#include "UI/Widgets/AuraUserWidget.h"

void AAuraHUD::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* Widget = CreateWidget<UAuraUserWidget>(GetWorld(), OverlayWidgetClass);
	Widget->AddToViewport();
}
