// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/Actors/Items/GAWeapon.h"
#include "MCWorld_BasicInteractionTool.generated.h"

class UMaterialInterface;

class AMCWorld_Manager;
class AMCWorld_Voxel;
class AMCWorld_VoxelItem;

class AGAActor;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API AMCWorld_BasicInteractionTool : public AGAWeapon
{
	GENERATED_BODY()

public:
	AMCWorld_BasicInteractionTool();

	/* Basic Menu Functionality */

	TArray<AGAActor*> QuickVoxelInventory;
	TArray<AGAActor*> StowedVoxelInventory;

	int32 MaxColumns;
	int32 MaxRows;

	FVector2D CurrentSlot;

	void OpenVoxelMenu();
	void CloseVoxelMenu();
	bool bIsInMenu;

	void VoxelItemUp();
	void VoxelItemRight();
	void VoxelItemLeft();
	void VoxelItemDown();

	/* Basic Functionality */

	void SetCurrentItem(AGAActor* NewItem);

	bool AbilityTargeting();
	UMaterialInterface* VoxelTargetingMaterial;
	AMCWorld_VoxelItem* AbilityTargetingActor;

	AMCWorld_VoxelItem* VoxelItem;

	UMaterialInterface* VoxelMaterial;
	int32 InstanceValue;
	int32 BlockValue;
	int32 BlockType;
	FIntVector CurrentLocation;
	AMCWorld_Voxel* ActiveChunk;

	AGAActor* CurrentItem;

	virtual void UseUp_Implementation(float Val) override;
	virtual void UseRight_Implementation(float Val) override;

	virtual void UseThrow_Implementation() override;

public:
	void Tick(float DeltaTime) override;
};
