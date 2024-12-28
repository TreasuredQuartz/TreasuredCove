// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/FloatingBarComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "Kismet/GameplayStatics.h"

#include "Global/Actors/FloatingBarActor.h"

// Sets default values for this component's properties
UFloatingBarComponent::UFloatingBarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFloatingBarComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (!GetOwner<APawn>()->IsLocallyControlled())
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

	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();

	// Adjust active text actors' locations based on viewport size, scale, and current DPI scaling.
	for (int32 i = 1; i < ActiveBarActors.Num(); ++i)
	{
		// Calculate Offset between text actors
		AFloatingBarActor* bar = ActiveBarActors[i];
		const float distance = FVector::Dist(CameraLocation, bar->GetAnchorLocation());
		const float VerticalOffset = distance / (ViewportSize.X / ViewportScale);

		bar->SetActorLocation(bar->GetAnchorLocation() + FVector(0.0f, 0.0f, i * VerticalOffset * BarVerticalSpacing));
	}
}


// Called every frame
void UFloatingBarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


//
void UFloatingBarComponent::AddFloatingBar_Client_Implementation(int Layers, int Bars, const FVector& WorldLocation)
{
	// We need the class specified before we can spawn text actors!
	if (!FloatingBarActorClass)
		return;

	const FTransform spawnTransform = FTransform(FRotator::ZeroRotator, WorldLocation + FVector(0.0f, 0.0f, BarVerticalOffset));
	AFloatingBarActor* newBarActor = GetWorld()->SpawnActorDeferred<AFloatingBarActor>(FloatingBarActorClass, spawnTransform, GetOwner());

	// Possible to get bad spawns and this checks that we did in fact spawn
	if (!newBarActor)
		return;

	newBarActor->AttachToActor(GetOwner(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
	newBarActor->OnDestroyed.AddDynamic(this, &UFloatingBarComponent::OnBarDestroyed);
	UGameplayStatics::FinishSpawningActor(newBarActor, spawnTransform);

	ActiveBarActors.Insert(newBarActor, 0);
}

//
void UFloatingBarComponent::OnBarDestroyed(AActor* DestroyedActor)
{
	if (AFloatingBarActor* DestroyedBar = Cast<AFloatingBarActor>(DestroyedActor))
	{
		int32 Index = ActiveBarActors.Find(DestroyedBar);

		if (Index == INDEX_NONE) return;

		ActiveBarActors.RemoveAtSwap(Index);
	}
}

void UFloatingBarComponent::UpdateProgress(int32 Index, float Current, float Max)
{
	if (ActiveBarActors.IsValidIndex(Index)) ActiveBarActors[Index]->UpdateProgress(Current, Max);
}

bool UFloatingBarComponent::HasIndex(int32 Index)
{
	return ActiveBarActors.IsValidIndex(Index);
}

void UFloatingBarComponent::DestroyAllBars()
{
	if (ActiveBarActors.IsEmpty()) return;

	/*for (AFloatingBarActor* Bar : ActiveBarActors)
	{
		if (Bar) Bar->Destroy();
	}*/

	ActiveBarActors.Empty();
}
