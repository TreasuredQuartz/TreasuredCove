// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ControlInputInterface.h"

#include "BulletStruct.h"
#include "GAItemInfoStruct.h"
#include "ItemKeyStruct.h"

#include "AbilityType.h"
#include "AttributeType.h"
#include "NavType.h"

#include "GAPlayerController.generated.h"

class AGAHUD;
class AGAActor;
class AGACharacter;
class UDlgDialogue;
class UDlgContext;

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API AGAPlayerController : 
	public APlayerController,
	public IControlInputInterface
{
	GENERATED_BODY()

private:
	//
	bool bIsPaused;
protected:
	//
	AGAHUD* HUD;

	//
	AGACharacter* Player;

	//
	UPROPERTY(BlueprintReadOnly)
	UDlgContext* ActiveContext = nullptr;
public:
	//
	//virtual void SetupInputComponent() override;

	//
	void ShouldDisableInput(bool ShouldDisable);

	//
	void AddAbilityToUI(FAbilityInfo AbilityInfo, EAbilityType AbilityType, struct FGameplayAbilitySpecHandle InHandle, bool bFromItem);

	//
	void RemoveAbilityFromUI(struct FGameplayAbilitySpecHandle InHandle);

	// We are not using this right now

	/*
	void OnControllerChanged(bool bIsConnected);
	*/

	//
	void OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);
	void OnHealed(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);
	void OnHealthModified(float Health, float MaxHealth);
	void OnStaminaModified(float Stamina, float MaxStamina);
	void OnManaModified(float Mana, float MaxMana);
	void OnExperienceModified(float Experience, float MaxExperience);
	void OnStatModified(float Charisma, float Constitution, float Dexterity, float Intelligence, float Strength, float Wisdom);
	void OnCurrentWeaponAmmoModified(float Ammo, float MaxAmmo);
	void OnAmmoStockModified(FBullet Bullet);

	//
	void OnDesiredLocationCleared();
	void OnDesiredLocationSet(FVector DesiredLocation);
	void OnUpdateCurrentBuilding(FName CurrentBuildingName);
	void OnUpdateTargetBuilding(FName TargetBuildingName);

	//------------------------------
	// User Interface Pass Functions
	//------------------------------

	UFUNCTION(BlueprintCallable)
	void ReturnMainMenu();
	void BeginPause();
	void EndPause();

	void ConfirmPressed();
	void ConfirmReleased();

	void ReturnPressed();
	void ReturnReleased();

	void RecieveNavRequest(ENavType NavType);
	void RecieveTabRequest(bool bNavRight);

	void CursorMoveUp(float Val);
	void CursorMoveRight(float Val);
	void OnMenuUpdated(FVector2D MenuSelection);
	void SetItemPriority(bool Priority);

	//------------------------------
	// Inventory Pass Functions
	//------------------------------

	void NotifyPickup(FName ItemName, bool CanPickup);

	// Pickup item into our inventory
	void OnPickupItem(FGAItemInfo& ItemInfo);

	// Pickup item into our repository
	void OnAddItemToRepository(const FItemKey& Item);
	void OnEquipItem(UObject* Item, FGAItemInfo& ItemInfo);
	void OnStowedItem(FGAItemInfo& ItemInfo);
	UFUNCTION(BlueprintCallable)
	void OnEquipItemFromInventory(uint8 slot);
	UFUNCTION(BlueprintCallable)
	void OnStowItemFromInventory(uint8 Slot);
	void OnAddEquippedItemToInventory();
	void OnAddStowedItemToInventory();
	UFUNCTION(BlueprintCallable)
	void OnMoveItem(uint8 From, uint8 To);
	UFUNCTION(BlueprintCallable)
	void OnDropItem(uint8 Slot);

	//------------------------------
	// Dialogue Functions
	//------------------------------

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(UDlgDialogue* Dialogue, UObject* OtherParticipant);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SelectDialogueOption(int32 Index);
};
