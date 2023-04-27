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
class UItemPopupData;
class UCommandMenu;
class UGameplayAbilitySet;

/** The base class for weapons.
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API AGAWeapon : 
	public AGAActor
{
	GENERATED_BODY()
	
protected:
	bool bBeingPickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	UTexture2D* Texture;

	UPROPERTY(Instanced, EditDefaultsOnly, Category = "Item")
	UItemPopupData* ItemData;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EWeaponType WeaponType;

	// This would be where the projectile from an ability would spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FVector AbilitySpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<FSavedAttribute> Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* Box;

public:
	// Used by owning character to throw held item
	void LaunchItem(const FVector& LaunchVelocity) const;

	//
	UFUNCTION()
	void GetItemInfo(FGAItemInfo& Info);

	UFUNCTION(BlueprintCallable, Category="Item")
	UObject* GetItemData();

	virtual void InteractedWith_Implementation(AActor* OtherActor) override;

public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnPhysicsSleep(UPrimitiveComponent* SleepingComponent, FName BoneName);

public:
	FItemKey GetPickup_Implementation()
	{
		return FItemKey(Name, 1);
	}
};
