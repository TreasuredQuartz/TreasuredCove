// Fill out your copyright notice in the Description page of Project Settings.

#include "PlanetQuadTree.h"
#include "PlanetLandscape.h"

constexpr int MAXLOD = 11;

void UPlanetQuadTree::ConstructQuadTree()
{
	if (detailLevel < 4 && detailLevel >= 0)
	{
		if (FVector::DistSquared( position.GetSafeNormal(), PlayerPosition.GetSafeNormal() ) <= Settings.LODSettings.LevelsOfDetail[detailLevel])
		{
			Children = new UPlanetQuadTree*[4];

			#define halfRadiusA (axisA) * localRadius / 2.0
			#define halfRadiusB (axisB) * localRadius / 2.0

			TArray<FVector> dict = {
				position + halfRadiusA + halfRadiusB,
				position + halfRadiusA - halfRadiusB,
				position + halfRadiusB - halfRadiusA,
				position - halfRadiusB - halfRadiusA };

			Children[0] = new UPlanetQuadTree(Settings, nullptr, PlayerPosition, dict[0], axisA, axisB, localRadius / 2, detailLevel + 1);
			Children[1] = new UPlanetQuadTree(Settings, nullptr, PlayerPosition, dict[1], axisA, axisB, localRadius / 2, detailLevel + 1);
			Children[2] = new UPlanetQuadTree(Settings, nullptr, PlayerPosition, dict[2], axisA, axisB, localRadius / 2, detailLevel + 1);
			Children[3] = new UPlanetQuadTree(Settings, nullptr, PlayerPosition, dict[3], axisA, axisB, localRadius / 2, detailLevel + 1);

			for (int i = 0; i < 4; ++i)
			{
				Children[i]->ConstructQuadTree();
			}
		}
	}
}

static TArray<FColor> detailColor = {
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

FColor UPlanetQuadTree::GetDebugColorForRegions()
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

void UPlanetQuadTree::CalculateMesh(TArray<FVector>& vertices, TArray<int>& triangles, TArray<FColor>& colors, int triangleOffset)
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

	if (Children != nullptr)
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

FString UPlanetQuadTree::ToString()
{
	return position.ToString();
}
