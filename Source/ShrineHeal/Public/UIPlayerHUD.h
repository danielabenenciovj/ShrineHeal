// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIPlayerHUD.generated.h"


class UProgressBar;
class UTextBlock;
class UWidgetAnimation;


UCLASS()
class SHRINEHEAL_API UUIPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
    
	UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateHealthUI(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowHealMessage(float HealAmount);

protected:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_Health;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_HealthNumbers;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_HealMessage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* Anim_FadeMessage;
};
