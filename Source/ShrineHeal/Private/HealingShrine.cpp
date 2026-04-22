// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingShrine.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "ShrineHealCharacter.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"

// Sets default values
AHealingShrine::AHealingShrine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    InteractVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractVolume"));
    RootComponent = InteractVolume;
    InteractVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    ShrineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShrineMesh"));
    ShrineMesh->SetupAttachment(RootComponent);

    bIsAvailable = true;
    HealAmount = 30.0f; 
    CooldownTime = 5.0f;

}

// Called when the game starts or when spawned
void AHealingShrine::BeginPlay()
{
	Super::BeginPlay();

    UpdateShrineVisuals();
}

// Called every frame
void AHealingShrine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AHealingShrine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AHealingShrine, bIsAvailable);
}

void AHealingShrine::TryActivateShrine(AShrineHealCharacter* TargetCharacter)
{
    if (HasAuthority() && bIsAvailable && TargetCharacter)
    {
        if (TargetCharacter->CurrentHealth < TargetCharacter->MaxHealth)
        {
            bIsAvailable = false;

            TargetCharacter->CurrentHealth = FMath::Clamp(TargetCharacter->CurrentHealth + HealAmount, 0.0f, TargetCharacter->MaxHealth);

            if (TargetCharacter->IsLocallyControlled())
            {
                TargetCharacter->OnRep_CurrentHealth();
            }

            TargetCharacter->Client_ReceiveHealMessage(HealAmount);
            Multicast_PlayEffects();

            OnRep_IsAvailable(); 

            GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &AHealingShrine::ResetShrine, CooldownTime, false);
        }
    }
}

void AHealingShrine::ResetShrine()
{
    if (HasAuthority())
    {
        bIsAvailable = true;
        OnRep_IsAvailable();
    }
}

void AHealingShrine::OnRep_IsAvailable()
{
    UpdateShrineVisuals();
}

void AHealingShrine::UpdateShrineVisuals()
{
    if (ShrineMesh)
    {
        
        UMaterialInterface* CurrentMaterial = bIsAvailable ? AvailableMaterial : CooldownMaterial;

        if (CurrentMaterial)
        {
            ShrineMesh->SetMaterial(0, CurrentMaterial);
        }
    }
}

void AHealingShrine::Multicast_PlayEffects_Implementation()
{
    if (HealEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            HealEffect,
            GetActorLocation(),
            GetActorRotation()
        );
    }

   
    if (HealSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HealSound, GetActorLocation());
    }
}
