// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayJobSite.h"
#include "TownSystemComponent.h"

// Sets default values
AGameplayJobSite::AGameplayJobSite(const FObjectInitializer& ObjectInitializer)
{
}

// Called when the game starts or when spawned
void AGameplayJobSite::BeginPlay()
{
	Super::BeginPlay();
	
	for (UGameplayJob* Job : Jobs)
	{
		Job = NewObject<UGameplayJob>();
	}
}

void AGameplayJobSite::AddToResources(FName Resource, float Amount)
{
	if (Resources.Contains(Resource))
	{
		float NewAmount = Resources[Resource] + Amount;
		Resources.Add(Resource, NewAmount);
	}
}

void AGameplayJobSite::RemoveFromResources(FName Resource, float Amount)
{
	if (Resources.Contains(Resource))
	{
		float NewAmount = Resources[Resource] - Amount;
		Resources.Add(Resource, NewAmount);
	}
}

void AGameplayJobSite::AddRequest(FRequest Request)
{
	Requests.Add(Request);
}

void AGameplayJobSite::RemoveRequest(FRequest Request)
{
	Requests.Remove(Request);
}

void AGameplayJobSite::RequestResource(AActor* Requester, FName ResourceType, float RequestAmount)
{
	if (Requester->Implements<UTownSystemInterface>())
	{
		TMap<FName, float> RequestedResources;
		RequestedResources.Add(ResourceType, RequestAmount);

		FRequest NewRequest = FRequest(Requester, RequestedResources);
		Requests.Add(NewRequest);
		CheckRequests(true);
	}
}

void AGameplayJobSite::RequestResources(AActor* Requester, TMap<FName, float> RequestedResources)
{
	FRequest NewRequest = FRequest(Requester, RequestedResources);
	Requests.Add(NewRequest);
	CheckRequests(true);
}

bool AGameplayJobSite::CanMakeRequest(AActor* Requester, FName ResourceType, float RequestAmount, float CurrencyOffered)
{
	float CurrencyRequired = 0;

	if (Resources.Contains(ResourceType))
	{
		if (ResourcesCost.Contains(ResourceType))
			CurrencyRequired = ResourcesCost[ResourceType];

		if (CurrencyRequired <= CurrencyOffered)
		{
			Requester;
			RequestResource(Requester, ResourceType, RequestAmount);
		}
	}

	return Resources.Contains(ResourceType);
}

bool AGameplayJobSite::CanMakeRequests(AActor* Requester, TMap<FName, float> InResources, float CurrencyOffered)
{
	bool bSucceeded = false;
	float CurrencyRequired = 0;

	TArray<FName> Keys;
	InResources.GetKeys(Keys);

	for (FName ResourceType : Keys)
	{
		bSucceeded = CanMakeRequest(Requester, ResourceType, InResources[ResourceType], CurrencyOffered);

		if(ResourcesCost.Contains(ResourceType))
			CurrencyRequired += ResourcesCost[ResourceType];

		if (!bSucceeded)
		{
			break;
		}
		else if (CurrencyRequired > CurrencyOffered)
		{
			bSucceeded = false;
			break;
		}
	}

	if (bSucceeded)
	{
		UTownSystemComponent* OtherTownSystem = Cast<ITownSystemInterface>(Requester)->GetTownSystemComponent();
		if (OtherTownSystem)
		{
			RequestResources(Requester, InResources);
		}
	}

	return bSucceeded;
}

void AGameplayJobSite::CheckRequests(bool bNewRequest)
{
	if (Requests.Num() == 0)
	{
		return;
	}

	TMap<FName, float> NewResourcesNeeded;

	for (FRequest Request : Requests)
	{
		if (Request.RequestOwner)
		{
			bool bCanFulfillRequest = true;

			TArray<FName> Keys;
			Request.RequestedResources.GetKeys(Keys);

			if(bNewRequest)
			{
				for (FName RequestType : Keys)
				{
					float RequestAmount = Request.RequestedResources[RequestType];

					if (!(Resources[RequestType] >= RequestAmount))
					{
						bCanFulfillRequest = false;

						if (NewResourcesNeeded.Contains(RequestType))
						{
							NewResourcesNeeded[RequestType] += RequestAmount - Resources[RequestType];
						}
						else
						{
							NewResourcesNeeded.Add(RequestType, RequestAmount - Resources[RequestType]);
						}
					}
				}
			}
			else
			{
				for (FName RequestType : Keys)
				{
					float RequestAmount = Request.RequestedResources[RequestType];

					if (!(Resources[RequestType] >= RequestAmount))
					{
						bCanFulfillRequest = false;
					}
				}
			}

			if (bCanFulfillRequest)
			{
				FulfillRequest(Request);
				Requests.Remove(Request);
			}
		}
		else
		{
			Requests.Remove(Request);
		}
	}

	if (bNewRequest)
	{
		ResourcesNeeded.Empty(NewResourcesNeeded.Num());
		ResourcesNeeded.Append(NewResourcesNeeded);
	}
}

void AGameplayJobSite::FulfillRequest(FRequest Request)
{
	TArray<FName> Keys;
	Request.RequestedResources.GetKeys(Keys);
	for (FName RequestType : Keys)
	{
		float RequestAmount = Request.RequestedResources[RequestType];

		UTownSystemComponent* OtherTownSystem = Cast<ITownSystemInterface>(Request.RequestOwner)->GetTownSystemComponent();

		OtherTownSystem->AddResource(RequestType, RequestAmount);
		
		Resources[RequestType] -= RequestAmount;
	}
}
