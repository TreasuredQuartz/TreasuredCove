// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemKeyStruct.h"
#include "VendorComponent.generated.h"

/** A Component that holds information related to Vendors or Shops.
* Intended to allow entities to make offers to other entiies.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UVendorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVendorComponent();

	// Offers are item(s) that are available to trade, and thier price.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vendor")
	TMap<FItemKey, FItemKey> Offers;
};
