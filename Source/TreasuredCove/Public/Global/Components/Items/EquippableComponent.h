// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquippableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquipped);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UEquippableComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FString EquipSlot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bEquipped;

public:	
	// Sets default values for this component's properties
	UEquippableComponent();

public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FString GetEquipSlot() const { return EquipSlot; };

public:
	void Equipped();
	void UnEquipped();

public:
	UPROPERTY(BlueprintAssignable)
	FOnEquipped OnEquipped;
	UPROPERTY(BlueprintAssignable)
	FOnUnEquipped OnUnEquipped;
};
