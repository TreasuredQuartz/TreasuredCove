// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DialogueInterface.generated.h"

/**
*
*/
UINTERFACE(BlueprintType)
class TREASUREDCOVE_API UDialogueInterface : public UInterface
{
	GENERATED_BODY()

};

class TREASUREDCOVE_API IDialogueInterface
{
	GENERATED_BODY()

public:
	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue")
	void StartDialogue();
	virtual void StartDialogue_Implementation();

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue")
	void ContinueDialogue();
	virtual void ContinueDialogue_Implementation();

	//
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dialogue")
	void EndDialogue();
	virtual void EndDialogue_Implementation();
};
