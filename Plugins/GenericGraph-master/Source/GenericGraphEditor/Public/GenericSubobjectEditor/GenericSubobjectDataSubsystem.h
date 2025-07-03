// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "GenericSubobjectEditor/GenericSubobjectDataHandle.h"
#include "GenericSubobjectDataSubsystem.generated.h"

struct FGenericSubobjectData;
class USubobjectDataSubsystem; // For easy access peeking

/** Options when adding a new subobject */
USTRUCT(BlueprintType, Category = "SubobjectDataSubsystem")
struct FAddNewGenericSubobjectParams
{
	GENERATED_USTRUCT_BODY()

	FAddNewGenericSubobjectParams()
		: ParentHandle(FGenericSubobjectDataHandle::InvalidHandle)
		, NewClass(nullptr)
		, AssetOverride(nullptr)
	{
	}

	UPROPERTY(BlueprintReadWrite, Category = "SubobjectDataSubsystem")
	FGenericSubobjectDataHandle ParentHandle;

	/** The class of the new subobject that will be added */
	UPROPERTY(BlueprintReadWrite, Category = "SubobjectDataSubsystem")
	TObjectPtr<UClass> NewClass;

	/** Specific asset to use instead of the selected asset in the content browser */
	UObject* AssetOverride;
};

/**
 * 
 */
UCLASS()
class GENERICGRAPHEDITOR_API UGenericSubobjectDataSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	DECLARE_DELEGATE_OneParam(FOnSubobjectSelected, const TArray<FGenericSubobjectDataHandle>&);

public:
	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	/**
	* Static wrapper for getting this engine subsystem. Will return nullptr
	* if the SubobjectDataInterface module has not been loaded yet.
	*/
	static UGenericSubobjectDataSubsystem* Get();

	/**
	* Gather all subobjects that the given UObject context has. Populates an array of
	* handles that will have the given context and all it's subobjects.
	*
	* @param Context		Object to gather subobjects for
	* @param OutArray		Array to populate (will be emptied first)
	*/
	void GatherGenericSubobjectData(UObject* Context, TArray<FGenericSubobjectDataHandle>& OutArray);

	/**
	* Attempt to find an existing handle for the given object.
	*
	* @param Context		The context that the object to find is within
	* @param ObjectToFind	The object that you want to find the handle for within the context
	*
	* @return FGenericSubobjectDataHandle	The subobject handle for the object, Invalid handle if not found.
	*/
	FGenericSubobjectDataHandle FindHandleForObject(const FGenericSubobjectDataHandle& Context, const UObject* ObjectToFind) const;

	/**
	 * Find all child subobject data related to the given root context.
	 * Populates the OutVisited set with all data.
	 */
	void FindAllSubobjectData(FGenericSubobjectData* InData, TSet<FGenericSubobjectData*>& OutVisited) const;

	/**
	* Add a new subobject as a child to the given parent object
	*
	* @param Params			Options to consider when adding this subobject
	*
	* @return FSubobjectDataHandle		Handle to the newly created subobject, Invalid handle if creation failed
	*/
	UFUNCTION(BlueprintCallable, Category = "SubobjectDataSubsystem")
	FGenericSubobjectDataHandle AddNewSubobject(const FAddNewGenericSubobjectParams& Params, FText& FailReason);

	/**
	* Attempts to delete the given array of subobjects from their context
	*
	* @param ContextHandle			The owning context of the subobjects that should be removed
	* @param SubobjectsToDelete		Array of subobject handles that should be deleted
	* @param OutComponentToSelect	Populates this handle with a valid selection in the component hierarchy if desired
	* @param bForce					If true then this will attempt to delete a subobject even if the CanDelete function flags otherwise.
	* @return 	The number of subobjects successfully deleted
	*/
	int32 DeleteSubobjects(const FGenericSubobjectDataHandle& ContextHandle, const TArray<FGenericSubobjectDataHandle>& SubobjectsToDelete, FGenericSubobjectDataHandle& OutComponentToSelect, bool bForce = false);

	/**
	* Attempts to delete the given array of subobjects from their context
	*
	* @param ContextHandle			The owning context of the subobjects that should be removed
	* @param SubobjectsToDelete		Array of subobject handles that should be deleted
	* @param BPContext				The blueprint context for the given
	*
	* @return 	The number of subobjects successfully deleted
	*/
	UFUNCTION(BlueprintCallable, Category = "SubobjectDataSubsystem", meta = (DisplayName = "Delete Subobjects from Blueprint"))
	int32 DeleteSubobjects(const FGenericSubobjectDataHandle& ContextHandle, const TArray<FGenericSubobjectDataHandle>& SubobjectsToDelete);

	/**
	 * Returns true if the given new text is a valid option to rename the
	 * subobject with the given handle. Populates the OutErrorMessage if
	 * it is not valid.
	 *
	 * @param Handle			Handle to the subobject that is being checked
	 * @param InNewText			The new name that is desired
	 * @param OutErrorMessage	The reasoning for an invalid name
	 *
	 * @return True if the rename is valid
	 */
	UFUNCTION(BlueprintCallable, Category = "SubobjectDataSubsystem")
	bool IsValidRename(const FGenericSubobjectDataHandle& Handle, const FText& InNewName, FText& OutErrorMessage) const;

	/**
	* Attempts to rename the given subobject to the new name.
	*
	* @param Handle			Handle to the subobject to rename
	* @param InNewName		The new name that is desired for the given subobject
	*
	* @return True if the rename was successful, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "SubobjectDataSubsystem")
	bool RenameSubobject(const FGenericSubobjectDataHandle& Handle, const FText& InNewName);

	/**
	 * Returns true if the given array of handles represents subobjects that
	 * can be copied.
	 */
	UFUNCTION(BlueprintCallable, Category = "SubobjectDataSubsystem")
	bool CanCopySubobjects(const TArray<FGenericSubobjectDataHandle>& Handles) const;

	UFUNCTION(BlueprintCallable, Category = "SubobjectDataSubsystem")
	void CopySubobjects(const TArray<FGenericSubobjectDataHandle>& Handles);

	UFUNCTION(BlueprintCallable, Category = "SubobjectDataSubsystem")
	bool CanPasteSubobjects(const FGenericSubobjectDataHandle& RootHandle) const;

	void PasteSubobjects(const FGenericSubobjectDataHandle& PasteToContext, const TArray<FGenericSubobjectDataHandle>& NewParentHandles, TArray<FGenericSubobjectDataHandle>& OutPastedHandles);

	/**
	 * Duplicate the given array of subobjects on the context.
	 *
	 * @param Context			The owning context that the subobjects to dup come from
	 * @param SubobjectsToDup	Array of handles of existing subobjects you would like to have duplicated
	 * @param BpContext			Pointer to the current blueprint context if necessary. Use nullptr if dealing with instances
	 * @param OutNewSubobjects	Array that will be populated with any newly created subobjects
	 */
	void DuplicateSubobjects(const FGenericSubobjectDataHandle& Context, const TArray<FGenericSubobjectDataHandle>& SubobjectsToDup, TArray<FGenericSubobjectDataHandle>& OutNewSubobjects);

	/**
	 * Gets a ref to subobject data that has been initialized with the given context.
	 * It will have a unique ID, but may be reused from the FreeData array.
	 */
	FGenericSubobjectDataHandle CreateSubobjectData(UObject* Context, const FGenericSubobjectDataHandle& ParentHandle = FGenericSubobjectDataHandle::InvalidHandle);

	/**
	 * Get a handle to the subobject with the given context with a specific parent.
	 * If the context is already a known child of the parent, then it will return an existing subobject handle.
	 */
	FGenericSubobjectDataHandle FactoryCreateSubobjectDataWithParent(UObject* Context, const FGenericSubobjectDataHandle& ParentHandle);

	/**
	 * Broadcasts a delegate with the given parameter information.
	 */
	void SelectSubobjects(const TArray<FGenericSubobjectDataHandle>& SelectedSubobjects) const;

public:
	FOnSubobjectSelected OnSubobjectSelectedDelegate;
};
