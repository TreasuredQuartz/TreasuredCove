// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Libraries/RandomWalkLibrary.h"

TArray<int32> URandomWalkLibrary::GenerateRandomWalk2D(uint8 height, uint8 width, uint8 maxSteps, int32 RandomSeed)
{
	TArray<int32> ReturnValue;

	if (height < 1 || width < 1 || maxSteps < 1)
	{
		return ReturnValue;
	}

	ReturnValue.SetNumZeroed(height * width);
	FVector2D CurrentPosition = FVector2D(floor(width * 0.5), floor(height * 0.5));
	FRandomStream RandomStream = FRandomStream(RandomSeed);

	while (maxSteps > 0)
	{
		uint8 LoopCount = 4;

		while (LoopCount > 0)
		{
			uint8 randDirection = floor(RandomStream.FRand() * 4);
			FVector2D NewPosition = CurrentPosition;

			switch (randDirection)
			{
			case 0:
				NewPosition.X += 1;
				break;
			case 1:
				NewPosition.X -= 1;
				break;
			case 2:
				NewPosition.Y += 1;
				break;
			case 3:
				NewPosition.Y -= 1;
				break;
			}

			if (NewPosition.X > width - 1 ||
				NewPosition.X < 0 ||
				NewPosition.Y > height - 1 ||
				NewPosition.Y < 0)
			{
				LoopCount -= 1;
				if (LoopCount > 0)
				{
					continue;
				}
				else 
				{
					NewPosition = FVector2D(0, 0);
				}
			}

			CurrentPosition = NewPosition;
			break;
		}

		ReturnValue[ CurrentPosition.X + (CurrentPosition.Y * (width)) ] += 1;
		maxSteps -= 1;
	}

	return ReturnValue;
}