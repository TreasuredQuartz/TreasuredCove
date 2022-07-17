// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Core Class Dependencies
#include "CoreMinimal.h"
#include "Actors/GAActor.h"
#include "PickupInterface.h"

// Enums
#include "WeaponType.h"

// Structs
#include "SavedAttribute.h"
#include "GAItemInfoStruct.h"

// .generated file is always last include
#include "GAWeapon.generated.h"

class UBoxComponent;
class UItemEditor;
class UCommandMenu;
class UGameplayAbilitySet;

/** The base class for weapons.
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API AGAWeapon : 
	public AGAActor,
	public IPickupInterface
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	UTexture2D* Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TSubclassOf<UCommandMenu> ActiveMenuClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TSubclassOf<UItemEditor> EditMenuClass;

public:
	AGAWeapon();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TArray<int> MaxActiveMenuSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TArray<int> MaxSubMenuSlots;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	TSubclassOf<UGameplayAbilitySet> InitialActiveAbilitySetClass;
	UGameplayAbilitySet* InitialActiveAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<FSavedAttribute> Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* Box;

	template<class T>
	T* GetMesh()
	{
		return Cast<T>(Mesh);
	}

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Mesh"))
	class USkeletalMeshComponent* BP_GetMesh()
	{
		return Cast<USkeletalMeshComponent>(Mesh);
	}

	//
	UFUNCTION()
	void GetItemInfo(FGAItemInfo& Info);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EWeaponType WeaponType;

	// This would be where the projectile from an ability would spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FVector AbilitySpawnLocation;

	virtual void InteractedWith_Implementation(AActor* OtherActor) override;

public:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	FItemKey GetPickup_Implementation()
	{
		return FItemKey(Name, 1);
	}

	UStaticMeshComponent* GetPickupMesh_Implementation()
	{
		return Cast<UStaticMeshComponent>(Mesh);
	}
};
