// Fill out your copywrite description in the Description Page in Project Settings

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FConfirmedDelegate);

enum class ENavType : uint8;

class AGAHUD;

/*
*
*/
UCLASS()
class TREASUREDCOVE_API UGAWidget :
	public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta = (CompactNodeTitle = "OnSetHUD"))
	void BP_OnSetHUD();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetHUD(AGAHUD* HUD);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	AGAHUD* OwningHUD;

	UPROPERTY(BlueprintAssignable)
	FConfirmedDelegate OnConfirmed;

	//
	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta = (CompactNodeTitle = "ConfirmPressed"))
	void BP_ConfirmPressed();

	//
	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta = (CompactNodeTitle = "ConfirmReleased"))
	void BP_ConfirmReleased();

	//
	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta = (CompactNodeTitle = "ReturnPressed"))
	void BP_ReturnPressed();

	//
	UFUNCTION(BlueprintImplementableEvent, Category = "UI", meta = (CompactNodeTitle = "ReturnReleased"))
	void BP_ReturnReleased();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FVector2D CursorPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	uint8 MainNavIndex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	uint8 SecondNavIndex;

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void OnModified(float CurrentValue, float MaxValue);

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void UpdateMainNavAppearence();

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void UpdateSecondNavAppearence();

	//
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void ConfirmPressed();

	//
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void ConfirmReleased();

	//
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void ReturnPressed();

	//
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void ReturnReleased();
	
	//
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RecieveNavRequest(ENavType Val);

	//
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RecieveTabRequest(bool bNavRight);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void RecieveNavUp();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void RecieveNavDown();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void RecieveNavRight();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void RecieveNavLeft();


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void RecieveTabRight();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI")
	void RecieveTabLeft();

	//
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void OnCursorMove(FVector2D Location);
};