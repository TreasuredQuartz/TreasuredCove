// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


/**
 * 
 */
struct PROCEDURALPLANETPLUGIN_API FMinMax
{
private:
	float X;
	float Y;

public:
	FMinMax() { X = INFINITY; Y = -INFINITY;  };

	void Add(float inValue)
	{
		if (X > inValue) X = inValue;
		if (Y < inValue) Y = inValue;
	}

	float GetMin()
	{
		return X;
	}

	float GetMax()
	{
		return Y;
	}
};
