// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletCreator.h"
#include "DrawingCanvas.h"

// Sets default values
ABulletCreator::ABulletCreator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABulletCreator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletCreator::InitializeBullet(float inLengthHead, float inLengthCylinder, float inLengthTail, float inRadiusCylinder, float inRadiusTail)
{
	Length_Head = inLengthHead;
	Length_Cylinder = inLengthCylinder;
	Length_Tail = inLengthTail;

	Radius_Cylinder = inRadiusCylinder;
	Radius_Tail = inRadiusTail;
}

UTexture2D* ABulletCreator::GetBulletTexture(bool& Succeeded)
{
	UTexture2D* ReturnTexture = Canvas->GetTexture();

	Succeeded = ReturnTexture != nullptr;

	return ReturnTexture;
}

void ABulletCreator::CreateBulletTexture()
{
	if (!Canvas)
	{
		Canvas = NewObject<UDrawingCanvas>();
		Canvas->InitializeCanvas(4096,4096);
	}

	Length = Length_Head + Length_Cylinder + Length_Tail;

	TArray<FColor> Pixels;
	Pixels.SetNum(16777216 /* = 256 * 65536 = 256 * 256 */);
	int x, y, z;

	for (z = 0; z < 256; z++)
	for (y = 0; y < 256; y++)
	for (x = 0; x < 256; x++)
	{
		float LocY = y;//FMath::Lerp(0.f, Length, (float)(y / 256) );
		float LocX = x;//FMath::Lerp(0.f, Length, (float)((x - 128) / 256) );
		float LocZ = z;
		int index = x + y * 256 + z * 65536;
		if (index >= Pixels.Num()) break;

		//if (LocY > Length_Cylinder + Length_Tail)
			//Pixels[x + y * 256] = Formula(LocX, LocY) * 255;
		Pixels[index] = z % 2 == 0 ? FColor(255, 255, 255, 255) : FColor(0, 255, 255, 255);
		//else if (LocY > Length_Tail)
		//	Pixels[x + y * 256] = (y <= (x * Radius_Cylinder)) * 255;
		//else
		//	Pixels[x + y * 256] = (y <= (x * Radius_Tail) / Length_Head) || (y >= (x * Radius_Tail) / Length_Head) * 255;
	}

	Canvas->SetCanvasFromColorArray(Pixels);
}

void ABulletCreator::CreateBulletModel()
{

}

int ABulletCreator::Formula(float x, float y)
{
	//
	return ConicFormula(x, y);
	//return SphereicallyBluntedCone(x, y, 0.001);
	//return (int)(x < (64) && x > -64 && y < (64) && y > -64);
}

int ABulletCreator::ConicFormula(float x, float y)
{
	// Is y inside radius?
	bool Value = y <= abs((x * Radius_Cylinder) / Length_Head);

	return (int)Value;
}

int ABulletCreator::SphereicallyBluntedCone(float x, float y, float sphereRadius)
{
	float L_Squared = Length_Head * Length_Head;
	float R_Squared = Radius_Cylinder * Radius_Cylinder;

	int Value = ConicFormula((L_Squared) / Radius_Cylinder * (sqrt(sphereRadius / (R_Squared + L_Squared))), y);

	return Value;
}
