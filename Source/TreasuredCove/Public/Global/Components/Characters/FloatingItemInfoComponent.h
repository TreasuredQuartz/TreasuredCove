// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FloatingItemInfoComponent.generated.h"

class AFloatingItemInfoActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UFloatingItemInfoComponent : public UActorComponent
{
	GENERATED_BODY()

private:
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
	void AddPopup_Client(FName ItemName, const FVector& WorldLocation);

	// 
	UFUNCTION()
	void OnPopupDestroyed(AActor* DestroyedActor);

	UFUNCTION() // UFUNCTION(Client, Unreliable, BlueprintCallable, Category = "FloatingText")
	void RemovePopup();
};
