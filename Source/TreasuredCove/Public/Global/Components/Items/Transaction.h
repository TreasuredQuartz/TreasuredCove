// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Transaction.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UTransaction : public UObject
{
	GENERATED_BODY()
	
public:
	UActorComponent* Seller;
	UActorComponent* Buyer;

	TSubclassOf<UObject> SellerClass;
	TSubclassOf<UObject> BuyerClass;

	UActorComponent* GetSeller(TSubclassOf<UActorComponent> SellerClass) const;
	UActorComponent* GetBuyer(TSubclassOf<UActorComponent> BuyerClass) const;

	void SetSeller(UActorComponent NewSeller);
	void SetBuyer(UActorComponent NewBuyer);
};
