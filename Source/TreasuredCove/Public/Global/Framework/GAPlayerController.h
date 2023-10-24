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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostProcessInput, float, DeltaTime, bool, bGamePaused);

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
	FOnPostProcessInput OnPostProcessInput;

	//
	virtual void SetupInputComponent() override;

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

public:
	//
	bool CanAccessHUD();

	//
	void ShouldDisableInput(bool ShouldDisable);

	//
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void AddAbilityToUI_Client(FAbilityInfo AbilityInfo, EAbilityType AbilityType, struct FGameplayAbilitySpecHandle InHandle, bool bFromItem);

	//
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void RemoveAbilityFromUI_Client(struct FGameplayAbilitySpecHandle InHandle);

	// We are not using this right now

	/*
	void OnControllerChanged(bool bIsConnected);
	*/

	//
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnDeath_Client();
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnDamaged_Client(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnHealed_Client(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnHealthModified_Client(float Health, float MaxHealth);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnStaminaModified_Client(float Stamina, float MaxStamina);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnManaModified_Client(float Mana, float MaxMana);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnExperienceModified_Client(float Experience, float MaxExperience);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnStatModified_Client(float Charisma, float Constitution, float Dexterity, float Intelligence, float Strength, float Wisdom);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnCurrentWeaponAmmoModified_Client(float Ammo, float MaxAmmo);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnAmmoStockModified_Client(FBullet Bullet);

	//
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnDesiredLocationCleared_Client();
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnDesiredLocationSet_Client(FVector DesiredLocation);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnUpdateCurrentBuilding_Client(FName CurrentBuildingName);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnUpdateTargetBuilding_Client(FName TargetBuildingName);

	//------------------------------
	// User Interface Pass Functions
	//------------------------------

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void ReturnMainMenu_Client();
	UFUNCTION(BlueprintCallable, Category = "GAPlayerController|HUD")
	void BeginPause();
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void EndPause_Client();

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void ConfirmPressed_Client();
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void ConfirmReleased_Client();

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void ReturnPressed_Client();
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void ReturnReleased_Client();

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void RecieveNavRequest_Client(ENavType NavType);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void RecieveTabRequest_Client(bool bNavRight);

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void CursorMoveUp_Client(float Val);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void CursorMoveRight_Client(float Val);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnMenuUpdated_Client(FVector2D MenuSelection);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void SetItemPriority_Client(bool Priority);

	//------------------------------
	// Inventory Pass Functions
	//------------------------------

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void NotifyCanInteract_Client(FName ItemName, bool CanPickup);

	// Pickup item into our inventory
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnPickupItem_Client(const FGAItemInfo& ItemInfo);

	// Pickup item into our repository
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnAddItemToRepository_Client(const FItemKey& Item);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnEquipItem_Client(UObject* Item, const FGAItemInfo& ItemInfo);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnStowedItem_Client(const FGAItemInfo& ItemInfo);
	UFUNCTION(BlueprintCallable)
	void OnEquipItemFromInventory(uint8 slot);
	UFUNCTION(BlueprintCallable)
	void OnStowItemFromInventory(uint8 Slot);
	void OnAddEquippedItemToInventory();
	void OnAddStowedItemToInventory();
	UFUNCTION(BlueprintCallable)
	void OnMoveItem(uint8 From, uint8 To);
	UFUNCTION(BlueprintCallable)
	void OnDropItemFromInventory(uint8 Slot);
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void OnDropItem_Client();

	//------------------------------
	// Dialogue Functions
	//------------------------------

	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "GAPlayerController|HUD")
	void StartDialogue_Client(UDlgDialogue* Dialogue, UObject* OtherParticipant);
	UFUNCTION(BlueprintCallable, Category = "GAPlayerController|HUD")
	void SelectDialogueOption(int32 Index);
};
