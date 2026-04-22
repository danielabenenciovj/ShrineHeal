// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealingShrine.generated.h"


class UBoxComponent;
class UStaticMeshComponent;
class AShrineHealCharacter;
class UNiagaraSystem;
class UMaterialInterface;

UCLASS()
class SHRINEHEAL_API AHealingShrine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealingShrine();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void TryActivateShrine(AShrineHealCharacter* TargetCharacter);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* InteractVolume;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ShrineMesh;

    UPROPERTY(ReplicatedUsing = OnRep_IsAvailable, BlueprintReadOnly, Category = "Shrine")
    bool bIsAvailable;

    UFUNCTION()
    void OnRep_IsAvailable();

    UPROPERTY(EditDefaultsOnly, Category = "Shrine Settings")
    float HealAmount;

    UPROPERTY(EditDefaultsOnly, Category = "Shrine Settings")
    float CooldownTime;

    FTimerHandle CooldownTimerHandle;
    void ResetShrine();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayEffects();

    UPROPERTY(EditDefaultsOnly, Category = "Shrine Settings")
    UMaterialInterface* AvailableMaterial; 

    UPROPERTY(EditDefaultsOnly, Category = "Shrine Settings")
    UMaterialInterface* CooldownMaterial;

    void UpdateShrineVisuals();

    UPROPERTY(EditDefaultsOnly, Category = "Shrine Settings")
    UNiagaraSystem* HealEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Shrine Settings")
    class USoundBase* HealSound;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
