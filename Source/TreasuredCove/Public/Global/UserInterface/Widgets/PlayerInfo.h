// Fill out your copywrite description in the Description Page in Project Settings

#pragma once

#include "CoreMinimal.h"
#include "GAWidget.h"
#include "PlayerInfo.generated.h"

class UImage;
class UTextBlock;
class UVerticalBox;
class UMaterialInterface;
class UMaterialInstanceDynamic;

/**
*
*/
UCLASS()
class TREASUREDCOVE_API UPlayerInfo :
	public UGAWidget
{
	GENERATED_BODY()

	// Construct
	virtual void NativeConstruct() override;
private:
	//
	UVerticalBox* Container;
	//
	UImage* HealthBar;
	//
	UTextBlock* HealthText;
protected:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearence)
	UMaterialInterface* HealthBarMaterial;
	//
	UMaterialInstanceDynamic* HealthMID;
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearence)
	float Density;

	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearence)
	float Radius;

	//
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearence)
	float StartingPoint;

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetDensity(float Val);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetRadius(float Val);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	void SetStartingPoint(float Val);

	//
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnHealthModified(float Health, float MaxHealth);

	//
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnManaModified(float Mana, float MaxMana);

	//
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnStaminaModified(float Stamina, float MaxStamina);

	//
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void OnExperienceModified(float Experience, float MaxExperience);
};

