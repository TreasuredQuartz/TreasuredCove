// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Canvas.h"
#include "GameFramework/HUD.h"
#include "GameplayAbilitySpec.h"

#include "BulletStruct.h"
#include "GAItemInfoStruct.h"
#include "ItemKeyStruct.h"

#include "NavType.h"
#include "AbilityType.h"

#include "GAHUD.generated.h"

class UGAGameInstance;
class UGAWidget;
class AGAActor;
class AGAPlayerController;

UENUM(BlueprintType)
enum class EInputExec : uint8
{
	Clicked,
	Pressed,
	Released
};

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API AGAHUD : public AHUD
{
	GENERATED_BODY()

	// Construct
	AGAHUD(const FObjectInitializer& ObjectInitailizer);
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options")
	bool bIsUsingController;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options")
	bool bDrawHUD;

	//
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void OnControllerChanged(bool bIsConnected);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void AddAbilityToUI(FAbilityInfo AbilityInfo, EAbilityType AbilityType, FGameplayAbilitySpecHandle InHandle, bool bFromItem);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void RemoveAbilityFromUI(FGameplayAbilitySpecHandle InHandle);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnDialogueStarted();

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnMaxHealthModified(float PreviousHealth, float CurrentHealth);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnHealthModified(float Health, float MaxHealth);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnMaxStaminaModified(float PreviousStamina, float CurrentStamina);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnStaminaModified(float Stamina, float MaxStamina);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnMaxManaModified(float PreviousMana, float CurrentMana);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnManaModified(float Mana, float MaxMana);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnExperienceModified(float Experience, float MaxExperience);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnStatModified(float Charisma, float Constitution, float Dexterity, float Intelligence, float Strength, float Wisdom);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnCurrentWeaponAmmoModified(float Ammo, float MaxAmmo);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnAmmoStockModified(FBullet Bullet);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnDesiredLocationCleared();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnDesiredLocationSet(FVector DesiredLocation);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnUpdateCurrentBuilding(FName CurrentBuildingName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnUpdateTargetBuilding(FName TargetBuildingName);

	//---------------------------------
	// User Interface Pass Functions
	//---------------------------------

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ConfirmPressed();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ConfirmReleased();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ReturnPressed();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ReturnReleased();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void SendNavigationRequest(ENavType Val);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void SendTabRequest(bool bNavRight);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void CursorMoveUp(float Val);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void CursorMoveRight(float Val);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OpenMainMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OpenPauseMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ClosePauseMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnMenuUpdated(FVector2D MenuSelection);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void SetItemPriority(bool Priority);

	//---------------------------------
	// Inventory Pass Functions
	//---------------------------------

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void NotifyPickup(FName ItemName, bool CanPickup);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnPickupItem(const FGAItemInfo& ItemInfo);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay", meta = (DepreciatedFunction, DepreciationMessage="On pickup item into repository has been depreciated. \nPlease delete legacy functions as it is intended to use access to a component instead."))
	void OnPickupItemIntoRepository(const FItemKey& Item);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnEquipItem(UObject* Item, const FGAItemInfo& ItemInfo);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnStowedItem(const FGAItemInfo& ItemInfo);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Gameplay")
	void OnSwitchEquippedItems();

	//
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnStowItemFromInventory(uint8 Slot);

	//
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnAddStowedItemToInventory();

	//
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnEquipItemFromInventory(uint8 Slot);

	//
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnMoveItem(uint8 From, uint8 To);

	//
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnDropItem(uint8 Slot);

public:
	// Cursor
	FVector2D MouseLocation;

	//
	UGAGameInstance* GA;

	//
	AGAPlayerController* PC;

	//
	bool bIsInMenu;

	//
	bool bIsQuickMenuUp;

	//////////////////////////////////////////////////////

	// Player User Interface Input Set Up
	void PlayerInputChecks();

	//////////////////////////////////////////////////////

	// Stop Camera From Moving With Mouse
	void SetCursorMoveOnly(bool CursorOnly);

protected:
	//
	bool bConfirmInput;

	//////////////////////////////////////////////////////

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void OpenMenu(TSubclassOf<UGAWidget> Menu);

	//
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void QuickMenuNavRequest(FVector2D Val);

	//////////////////////////////////////////////////////
	
	virtual void BeginPlay() override;
	void DrawHUD_Reset();
	virtual void DrawHUD() override;
	virtual void PostInitializeComponents() override;

};
