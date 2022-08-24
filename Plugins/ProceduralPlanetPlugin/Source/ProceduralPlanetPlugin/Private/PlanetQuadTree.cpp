// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanetQuadTree.h"
#include "PlanetLandscape.h"

#pragma region Constants
constexpr int MAXLOD = 11;

static const TArray<FColor> detailColor = {
	FColor::White,		// 0
	FColor::Silver,		// 1
	FColor::Yellow,		// 2
	FColor::Orange,		// 3
	FColor::Red,		// 4
	// FColor::Turquoise,	// 5
	FColor::Emerald,	// 6
	FColor::Green,		// 7
	FColor::Cyan,		// 8
	FColor::Blue,		// 9
	FColor::Purple,		// 10
	FColor::Magenta,	// 11
	FColor::Black		// 12
};
#pragma endregion

uint8 UPlanetQuadTree::GetDetailLevel()
{
	uint8 HighestDetail = detailLevel;

	if (!Children.IsEmpty())
	{
		for (int i = 0; i < 4; ++i)
		{
			HighestDetail = FMath::Max(Children[i]->GetDetailLevel(), HighestDetail);
		}
	}

	return HighestDetail;
}

void UPlanetQuadTree::ConstructQuadTree()
{
	if (detailLevel < 10 && detailLevel >= 0)
	{
		if (FVector::DistSquared(position * Settings.radius, PlayerPosition) <= FMath::Square(Settings.LODSettings.LevelsOfDetail[detailLevel]))
		{
			Children = TArray<UPlanetQuadTree*>();
			Children.Init(nullptr, 4);

			#define halfRadiusA (axisA) * localRadius / 2.0
			#define halfRadiusB (axisB) * localRadius / 2.0

			TArray<FVector> dict = {
				position + halfRadiusA + halfRadiusB,
				position + halfRadiusA - halfRadiusB,
				position + halfRadiusB - halfRadiusA,
				position - halfRadiusB - halfRadiusA };

			Children[0] = new UPlanetQuadTree(Settings, PlayerPosition, dict[0], axisA, axisB, localRadius / 2, detailLevel + 1);
			Children[1] = new UPlanetQuadTree(Settings, PlayerPosition, dict[1], axisA, axisB, localRadius / 2, detailLevel + 1);
			Children[2] = new UPlanetQuadTree(Settings, PlayerPosition, dict[2], axisA, axisB, localRadius / 2, detailLevel + 1);
			Children[3] = new UPlanetQuadTree(Settings, PlayerPosition, dict[3], axisA, axisB, localRadius / 2, detailLevel + 1);

			for (int i = 0; i < 4; ++i)
			{
				Children[i]->ConstructQuadTree();
			}
		}
		else
		{
			// UE_LOG(LogTemp, Warning, TEXT("UPlanetQuadTree: \n ---Our Position %s\n ---StoredPlayerPosition: %s\n ----Distance: %e"), *(position * Settings.radius).ToString(), *PlayerPosition.ToString(), FVector::Dist(position * Settings.radius, PlayerPosition));
		}
	}
}

void UPlanetQuadTree::CalculateMesh(TArray<FVector>& vertices, TArray<int>& triangles, TArray<FColor>& colors, int triangleOffset) const
{
	if (Settings.resolutionPerChunk < 4 || detailLevel == MAXLOD) return;

	FColor VertColor = detailColor[detailLevel];
	int resolution = 4;
	int triIndex = 0, total = resolution - 1,
		i = 0, y = 0, x = 0, resSqrd = resolution * resolution;

	vertices.SetNumZeroed(resSqrd);
	triangles.SetNumZeroed(total * total * 6);
	colors.SetNumZeroed(resSqrd);

	// VertColor = GetDebugColorForRegions();

	for (y = 0; y < resolution; ++y)
	{
		for (x = 0; x < resolution; ++x)
		{
			FVector2D percent = FVector2D(x, y) / total;
			FVector pointOnUnitCube = (position) +
				((percent.X - 0.5f) * 2 * (axisA) +
				(percent.Y - 0.5f) * 2 * (axisB)) * localRadius;
			//UE_LOG(LogTemp, Warning, TEXT("  Vert Position: %s"), *pointOnUnitCube.ToString());
			FVector pointOnUnitSphere = pointOnUnitCube.GetSafeNormal();

			// Set Vertex
			vertices[i] = pointOnUnitSphere; // Convert to kilometers
			colors[i] = VertColor;

			// Set Triangle
			if (x != total && y != total)
			{
				triangles[triIndex] = i + triangleOffset;
				triangles[triIndex + 2] = i + resolution + 1 + triangleOffset;
				triangles[triIndex + 1] = i + resolution + triangleOffset;

				triangles[triIndex + 3] = i + triangleOffset;
				triangles[triIndex + 5] = i + 1 + triangleOffset;
				triangles[triIndex + 4] = i + resolution + 1 + triangleOffset;
				triIndex += 6;
			}

			// i = x + y * resolution;
			++i;
		}
	}
}

TArray<UPlanetQuadTree*> UPlanetQuadTree::GetVisibleChildren()
{
	TArray<UPlanetQuadTree*> value = TArray<UPlanetQuadTree*>();

	if (!Children.IsEmpty())
	{
		for (int i = 0; i < 4; ++i)
			value.Append(Children[i]->GetVisibleChildren());
	}
	else
	{
		value.Add(this);
	}

	return value;
}

bool UPlanetQuadTree::CheckLOD(const FVector& NewLocation)
{
	// Confirmed passed vector equivellent to our stored reference
	// UE_LOG(LogTemp, Warning, TEXT("UPlanetQuadTree: \n -NewLocation %s\n -StoredPlayerPosition: %s"), *NewLocation.ToString(), *PlayerPosition.ToString())
	bool bChanged = false;
	PlayerPosition = NewLocation;

	if (!Children.IsEmpty())
	{
		for (int i = 0; i < 4; ++i)
			if (Children[i]->CheckLOD(NewLocation)) bChanged = true;
	}
	else
	{
		if (FVector::DistSquared(position * Settings.radius, PlayerPosition) <= FMath::Square(Settings.LODSettings.LevelsOfDetail[detailLevel]))
		{
			ConstructQuadTree();
			bChanged = true;
		}
	}
	
	return bChanged;
}

#pragma region Debugging
FColor UPlanetQuadTree::GetDebugColorForRegions() const
{
	FColor VertColor = FColor::Emerald; // Failed for some reason? Shouln't be possible.

	if (position.X == 1 || position.X == -1)
	{
		if (position.Y > 0)
		{
			if (position.Z > 0)
			{
				VertColor = detailColor[0];
			}
			else
			{
				VertColor = detailColor[2];
			}
		}
		else
		{
			if (position.Z > 0)
			{
				VertColor = detailColor[4];
			}
			else
			{
				VertColor = detailColor[7];
			}

		}
	}
	else if (position.Y == 1 || position.Y == -1)
	{
		if (position.X > 0)
		{
			if (position.Z > 0)
			{
				VertColor = detailColor[0];
			}
			else
			{
				VertColor = detailColor[2];
			}
		}
		else
		{
			if (position.Z > 0)
			{
				VertColor = detailColor[4];
			}
			else
			{
				VertColor = detailColor[7];
			}

		}
	}
	else
	{
		if (position.X > 0)
		{
			if (position.Y > 0)
			{
				VertColor = detailColor[0];
			}
			else
			{
				VertColor = detailColor[2];
			}
		}
		else
		{
			if (position.Y > 0)
			{
				VertColor = detailColor[4];
			}
			else
			{
				VertColor = detailColor[7];
			}

		}
	}

	return VertColor;
}

FString UPlanetQuadTree::ToString() const
{
	return position.ToString();
}
#pragma endregion
