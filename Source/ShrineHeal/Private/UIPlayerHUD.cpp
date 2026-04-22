// Fill out your copyright notice in the Description page of Project Settings.


#include "UIPlayerHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UUIPlayerHUD::UpdateHealthUI(float CurrentHealth, float MaxHealth)
{
	
	if (PB_Health && MaxHealth > 0.0f)
	{
		float HealthPercentage = CurrentHealth / MaxHealth;
		PB_Health->SetPercent(HealthPercentage);
	}

	
	if (Txt_HealthNumbers)
	{
		
		FString HealthString = FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, MaxHealth);
		Txt_HealthNumbers->SetText(FText::FromString(HealthString));
	}
}

void UUIPlayerHUD::ShowHealMessage(float HealAmount)
{
	
	if (Txt_HealMessage)
	{
		FString HealString = FString::Printf(TEXT("+ %.0f HP"), HealAmount);
		Txt_HealMessage->SetText(FText::FromString(HealString));
	}

	
	if (Anim_FadeMessage)
	{
		PlayAnimation(Anim_FadeMessage);
	}
}