// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayBuilding.h"
#include "TownSystemInterface.h"
#include "GameplayJobSite.generated.h"

class UResourceComponent;
class UTownSystemComponent;
class UGameplayJob;
class AGameplayTown;
class AGameplayResident;

USTRUCT()
struct FRequest
{
	GENERATED_BODY()

	FRequest()
		: RequestOwner(nullptr)
	{};

	FRequest(AActor* Owner, TMap<FName, float> InRequestedResources)
		: RequestOwner(Owner)
	{
		RequestedResources = InRequestedResources;
	};

	FORCEINLINE bool operator == (const FRequest& Other) const
	{
		bool bSucceeded = false;

		if (RequestOwner == Other.RequestOwner)
		{
			bSucceeded = true;

			TArray<FName> Keys;
			RequestedResources.GetKeys(Keys);
			for (FName Key : Keys)
			{
				if (RequestedResources[Key] != Other.RequestedResources[Key])
				{
					bSucceeded = false;
					break;
				}
			}
		}

		return bSucceeded;
	}

	AActor* RequestOwner;
	TMap<FName, float> RequestedResources;
};

UCLASS()
class GAMEPLAYTOWNS_API AGameplayJobSite :
	public AGameplayBuilding,
	public ITownSystemInterface
{
	GENERATED_BODY()
	
	// Override function from Town system interface
	UTownSystemComponent* GetTownSystemComponent() const override
	{
		return TownSystem;
	}

public:	
	// Sets default values for this actor's properties
	AGameplayJobSite(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Town Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns", meta = (AllowPrivateAccess = "true"))
	UTownSystemComponent* TownSystem;

	// Resources
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns", meta = (AllowPrivateAccess = "true"))
	UResourceComponent* ResourceComponent;


	// Resources Component
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns")
	// UResourceSet* ResourceSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns|Jobs")
	TArray<UGameplayJob*> Jobs;
	TArray<TSubclassOf<UGameplayJob>> JobClasses;

private:
	TMap<FName, float> ResourceCosts;
	TArray<FRequest> Requests;

	TMap<FName, float> GetResources() const;
	void AddToResource(FName Resource, float Amount);
	void RemoveFromResource(FName Resource, float Amount);

	virtual void RequestResource(AActor* Requester, FName ResourceType, float RequestAmount);
	virtual void RequestResources(AActor* Requester, TMap<FName, float> InResources); 

	void CheckRequests(bool bNewRequest);
	void FulfillRequest(FRequest Request);

public:
	int CurrentNewRequestAmount;
	int NewRequestLimit;

	FVector RequestLocation;
	// FVector GetNewRequestLocation();

	TMap<FName, float> ResourcesNeeded;

	TArray<FRequest> GetRequests() const;
	virtual void AddRequest(FRequest Request);
	virtual void RemoveRequest(FRequest Request);

	virtual bool CanMakeRequest(AActor* Requester, FName ResourceType, float RequestAmount, float CurrencyPaid);
	virtual bool CanMakeRequests(AActor* Requester, TMap<FName, float> InResources, float CurrencyPaid);

public:
	void InitializeJobs();
};
