// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Core Class Dependencies
#include "CoreMinimal.h"
#include "Global/Actors/GAActor.h"
#include "Global/Interfaces/PickupInterface.h"

// Enums
#include "Global/Enumerations/WeaponType.h"

// Structs
#include "Global/Structs/SavedAttribute.h"
#include "Global/Structs/GAItemInfoStruct.h"

// .generated file is always last include
#include "GAWeapon.generated.h"

class UBoxComponent;
class UItemEditor;
class UItemPopupData;
class UCommandMenu;
class UGameplayAbilitySet;
class UGASkillTreeComponent;
class URarityComponent;
class UUIInfoComponent;
class UEquippableComponent;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	FText ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TSubclassOf<UItemEditor> EditMenuClass;

public:
	AGAWeapon();

public:
	// This would be where the projectile from an ability would spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	FVector AbilitySpawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TArray<FSavedAttribute> Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UGASkillTreeComponent* ProficiencyTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	URarityComponent* RarityComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UUIInfoComponent* UIInfoComponent;

public:
	// Used by owning character to throw held item
	void LaunchItem(const FVector& LaunchVelocity) const;

	//
	UFUNCTION()
	FGAItemInfo GetItemInfo() const;

	UFUNCTION(BlueprintCallable, Category="Item")
	UItemPopupData* GetItemData() const;

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
