// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/FloatingItemInfoComponent.h"
#include "Global/Actors/FloatingItemInfoActor.h"

#include "Global/Actors/Items/GAWeapon.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UFloatingItemInfoComponent::UFloatingItemInfoComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	PopupDetectionDistance = 500;
}

// Called when the game starts
void UFloatingItemInfoComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UFloatingItemInfoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Don't continue if our owner is nullptr or we are not locally controlled (Playing on this machine)
	const bool bIsValidOwnerClient = GetOwner<APawn>() && GetOwner<APawn>()->IsLocallyControlled();
	if (!bIsValidOwnerClient)
		return;

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

	// Reject bad viewport sizes
	if (ViewportSize.X <= 0.0f || ViewportSize.Y <= 0.0f)
		return;

	const int32 viewportX = FGenericPlatformMath::FloorToInt(ViewportSize.X);
	const int32 viewportY = FGenericPlatformMath::FloorToInt(ViewportSize.Y);

	const float ViewportScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportX, viewportY));

	// Reject bad viewport scales
	if (ViewportScale <= 0.0f)
		return;

	const auto* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!PlayerCameraManager)
		return;

	FVector PopupLocation;
	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();

	// Don't continue if we have no active Popup
	bool bLookingAtPickup = false;
	if (UWorld* World = GetWorld())
	{
		FHitResult Hit;
		const FVector Start = CameraLocation;
		const FVector End = Start + (CameraRotation.Vector() * PopupDetectionDistance);
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());
		if (World->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, Params))
		{
			if (AGAWeapon* HitWeapon = Cast<AGAWeapon>(Hit.GetActor()))
			{
				if (!HitWeapon->IsPickedUp())
				{
					bLookingAtPickup = true;
					PopupLocation = HitWeapon->GetActorLocation();
					// PopupLocation.Z = Hit.Location.Z >= PopupLocation.Z ? Hit.Location.Z : PopupLocation.Z;
					if (WeaponPickup != HitWeapon)
					{
						if (WeaponPickup)
						{
							RemovePopup();
						}

						WeaponPickup = HitWeapon;

						FGAItemInfo PickupInfo = WeaponPickup->GetItemInfo();
						UItemPopupData* PopupInfo = WeaponPickup->GetItemData();

						AddPopup_Client(PopupInfo, PopupLocation);
					}
				}
			}
		}
	}

	if (!bLookingAtPickup)
	{
		if (Popup)
			RemovePopup();

		WeaponPickup = nullptr;
	}
	else
	{
		if (Popup)
		{
			Popup->SetActorLocation(PopupLocation);

			const float Alpha = FMath::GetMappedRangeValueClamped<float>(TRange<float>(100.f, PopupDetectionDistance), TRange<float>(0.f, 1.f), (PopupLocation - GetOwner()->GetActorLocation()).Size());
			const FVector PopupScale3D = FMath::Lerp(FVector(10, 10, 10), FVector(0.1,0.1,0.1), Alpha);
			Popup->SetActorScale3D(PopupScale3D);
		}
	}
}

void UFloatingItemInfoComponent::AddPopup_Client_Implementation(const UItemPopupData* ItemPopupData, const FVector& WorldLocation)
{
	// We need the class specified before we can spawn text actors!
	if (!PopupClass)
		return;

	const FTransform spawnTransform = FTransform(FRotator::ZeroRotator, WorldLocation + FVector(0.0f, 0.0f, PopupVerticalOffset));
	AFloatingItemInfoActor* newPopupActor = GetWorld()->SpawnActorDeferred<AFloatingItemInfoActor>(PopupClass, spawnTransform, GetOwner());

	// Possible to get bad spawns and this checks that we did in fact spawn
	if (!newPopupActor)
		return;

	newPopupActor->OnDestroyed.AddDynamic(this, &UFloatingItemInfoComponent::OnPopupDestroyed);
	newPopupActor->AttachToActor(GetOwner(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), FName());
	UGameplayStatics::FinishSpawningActor(newPopupActor, spawnTransform);

	newPopupActor->Initialize(ItemPopupData);
	Popup = newPopupActor;
}

void UFloatingItemInfoComponent::OnPopupDestroyed(AActor* DestroyedPopup)
{
	Popup = nullptr;
}

void UFloatingItemInfoComponent::RemovePopup()
{
	if (Popup)
	{
		Popup->Destroy();
	}

	if (GEngine) GEngine->AddOnScreenDebugMessage(-2, 1, FColor::Black, FString("Remove Popup"));
	Popup = nullptr;
}

