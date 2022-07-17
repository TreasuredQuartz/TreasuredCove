// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TownSystemComponent.generated.h"

class AGameplayTown;
class AGameplayJobSite;
class AGameplayBuilding;
class AGameplayResidence;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUpdateResource, FName, ResourceType, float, DeltaAmount, bool, bIsAdded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateCurrentBuilding, AGameplayBuilding*, CurrentBuilding);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateDesiredLocation, FVector, DesiredLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateTargetBuilding, AGameplayBuilding*, TargetBuilding);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEPLAYTOWNS_API UTownSystemComponent : 
	public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTownSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called after initialization of components
	// virtual void InitializeComponent() override;

public:	
	bool bNeedsTempHome;

	FVector DesiredLocation;

	TMap<FName, float> Resources;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Towns")
	void AddResource(FName InResourceType, float InResourceAmount)
	{
		if (Resources.Contains(InResourceType))
		{
			Resources[InResourceType] += InResourceAmount;
		}
		else
		{
			Resources.Add(InResourceType, InResourceAmount);
		}

		UpdateResources.Broadcast(InResourceType, InResourceAmount, true);
	}
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Towns")
	void RemoveResource(FName InResourceType, float InResourceAmount)
	{
		if (Resources.Contains(InResourceType))
		{
			Resources[InResourceType] -= InResourceAmount;
		}
		else
		{
			Resources.Add(InResourceType, -InResourceAmount);
		}
	}
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Towns")
	void AddResources(TMap<FName, float> InResources)
	{
		TArray<FName> Keys;
		InResources.GetKeys(Keys);
		for (FName ResourceType : Keys)
		{
			AddResource(ResourceType, InResources[ResourceType]);
		}
	}
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Towns")
	void RemoveResources(TMap<FName, float> InResources)
	{

		TArray<FName> Keys;
		InResources.GetKeys(Keys);
		for (FName ResourceType : Keys)
		{
			RemoveResource(ResourceType, InResources[ResourceType]);
		}
	}

	TMap<FName, float> GetResources()
	{
		return Resources;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns")
	AGameplayTown* HomeTown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns")
	AGameplayTown* CurrentTown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns")
	AGameplayBuilding* CurrentBuilding;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns")
	AGameplayBuilding* TargetBuilding;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns|Jobs")
	AGameplayJobSite* WorkPlace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns")
	AGameplayResidence* Home;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns")
	AGameplayResidence* TempHome;

	void ArriveAtBuilding(AGameplayBuilding* NewCurrentBuilding);
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Towns")
	void SetCurrentBuilding(AGameplayBuilding* NewCurrentBuilding);
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Towns")
	void SetDesiredLocation(FVector NewDesiredLocation);
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Towns")
	void SetTargetBuilding(AGameplayBuilding* NewTargetBuilding);
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Towns")
	bool AttemptJobSiteRequest(FName ResourceTypeToRequest, float ResourceAmountToRequest, float CurrencyForRequest);
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Towns")
	bool AttemptUseResidence(FName Reason);

	UPROPERTY(BlueprintAssignable)
	FUpdateResource UpdateResources;
	UPROPERTY(BlueprintAssignable)
	FUpdateCurrentBuilding UpdateCurrentBuilding;
	UPROPERTY(BlueprintAssignable)
	FUpdateDesiredLocation UpdateDesiredLocation;
	UPROPERTY(BlueprintAssignable)
	FUpdateTargetBuilding UpdateTargetBuilding;
		
	void CheckSpace(bool bHasSpace)
	{
	}
};
