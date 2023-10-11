// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FloatingItemInfoComponent.generated.h"

class AFloatingItemInfoActor;

/** This information will be included when
 * Looking at the item in-world. The intention
 * Being that the player will be informed on
 * whether or not to pick it up.
 */
UCLASS(BlueprintType)
class TREASUREDCOVE_API UItemPopupData : public UObject
{
	GENERATED_BODY()

public:
	/*UItemPopupData(UTexture2D* InIcon, FText InName, FText InWeaponType, FText InEffectType, FText InSummary, FText InCreatorName)
		: DisplayIcon(InIcon)
		, DisplayName(InName)
		, WeaponType(InWeaponType)
		, EffectType(InEffectType)
		, Summary(InSummary)
		, CreatorName(InCreatorName)
	{};*/

	UItemPopupData() {};

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	uint8 Rarity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UTexture2D* DisplayIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText DisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText WeaponType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText EffectType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText Summary;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	TMap<FString, FText> ItemAttributeData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Credits", meta = (AllowPrivateAccess = "true"))
	FText CreatorName;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UFloatingItemInfoComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FloatingItemInfo", Meta = (AllowPrivateAccess = "true"))
	float PopupDetectionDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FloatingItemInfo", Meta = (AllowPrivateAccess = "true"))
	float PopupVerticalOffset;

	// Blueprint override
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FloatingItemInfo", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AFloatingItemInfoActor> PopupClass;

	//
	AFloatingItemInfoActor* Popup;
	class AGAWeapon* WeaponPickup;

public:	
	// Sets default values for this component's properties
	UFloatingItemInfoComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "FloatingText")
	void AddPopup_Client(const UItemPopupData* ItemPopupData, const FVector& WorldLocation);

	// 
	UFUNCTION()
	void OnPopupDestroyed(AActor* DestroyedActor);

	UFUNCTION() // UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "FloatingText")
	void RemovePopup();
};
