#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "Templates/SharedPointer.h"
#include "GenericGraphNodeComponent.h"
#include "GenericGraphNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemsUpdated);

class UGenericGraph;
class UGenericGraphEdge;
// class UGenericGraphNodeComponent;

UENUM(BlueprintType)
enum class ENodeLimit : uint8
{
	Unlimited,
    Limited
};


UCLASS(Blueprintable)
class GENERICGRAPHRUNTIME_API UGenericGraphNode : public UObject
{
	GENERATED_BODY()

public:
	UGenericGraphNode();
	virtual ~UGenericGraphNode();

	UPROPERTY(VisibleDefaultsOnly, Category = "GenericGraphNode")
	UGenericGraph* Graph;

	UPROPERTY(VisibleDefaultsOnly, Category = "GenericGraphNode")
	TArray<UGenericGraphNodeComponent*> OwnedComponents;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
	TArray<UGenericGraphNode*> ParentNodes;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
	TArray<UGenericGraphNode*> ChildrenNodes;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
	TMap<UGenericGraphNode*, UGenericGraphEdge*> Edges;

	UFUNCTION(BlueprintCallable, Category = "GenericGraphNode")
	virtual UGenericGraphEdge* GetEdge(UGenericGraphNode* ChildNode);

	UFUNCTION(BlueprintCallable, Category = "GenericGraphNode")
	bool IsLeafNode() const;

	UFUNCTION(BlueprintCallable, Category = "GenericGraphNode")
	UGenericGraph* GetGraph() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GenericGraphNode")
	FText GetDescription() const;
	virtual FText GetDescription_Implementation() const;

	void AddInstanceComponent(UGenericGraphNodeComponent* Component);
	void RemoveOwnedComponent(UGenericGraphNodeComponent* Component);

	/**
	 * UGenericGraphNodeComponent specialization of GetComponents() to avoid unnecessary casts.
	 * It's recommended to use TArrays with a TInlineAllocator to potentially avoid memory allocation costs.
	 * TInlineComponentArray is defined to make this easier, for example:
	 * {
	 * 	   TInlineComponentArray<UGenericGraphNodeComponent*> PrimComponents;
	 *     GenericGraphNode->GetComponents(PrimComponents);
	 * }
	 *
	 * @param bIncludeFromChildGenericGraphNodes	If true then recurse in to ChildGenericGraphNode components and find components of the appropriate type in those GenericGraphNodes as well
	 */
	template<class T, class AllocatorType>
	void GetComponents(TArray<T*, AllocatorType>& OutComponents) const
	{
		OutComponents.Reset();
		ForEachComponent_Internal<UGenericGraphNodeComponent>(UGenericGraphNodeComponent::StaticClass(), [&](UGenericGraphNodeComponent* InComp)
		{
			OutComponents.Add(InComp);
		});
	}

	TArray<UGenericGraphNodeComponent*> GetComponents() const { return OwnedComponents; };

	UFUNCTION(BlueprintCallable, Category = "GenericGraphNode", meta = (ComponentClass = "/Script/Engine.GenericGraphNodeComponent"), meta = (DeterminesOutputType = "ComponentClass"))
	UGenericGraphNodeComponent* GetComponentByClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass) const;

	UGenericGraphNodeComponent* FindComponentByClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass) const;

	/** Templated version of GetComponentByClass */
	template<class T>
	T* GetComponentByClass() const
	{
		return FindComponentByClass<T>();
	}

	/** Templatized version of FindComponentByClass that handles casting for you */
	template<class T>
	T* FindComponentByClass() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UGenericGraphNodeComponent>::Value, "'T' template parameter to FindComponentByClass must be derived from UGenericGraphNodeComponent");

		return (T*)FindComponentByClass(T::StaticClass());
	}

private:
	/**
	 * Internal helper function to call a compile-time lambda on all components of a given type
	 * Redirects the call to the proper template function specialization for bClassIsGenericGraphNodeComponent and bIncludeFromChildGenericGraphNodes :
	 */
	template<class ComponentType, typename Func>
	void ForEachComponent_Internal(TSubclassOf<UGenericGraphNodeComponent> ComponentClass, Func InFunc) const
	{
		static_assert(TPointerIsConvertibleFromTo<ComponentType, const UGenericGraphNodeComponent>::Value, "'ComponentType' template parameter to ForEachComponent must be derived from UGenericGraphNodeComponent");

		check(ComponentClass == UGenericGraphNodeComponent::StaticClass());
		check(ComponentClass->IsChildOf(ComponentType::StaticClass()));

		for (UGenericGraphNodeComponent* OwnedComponent : OwnedComponents)
		{
			if (OwnedComponent)
			{
				if (OwnedComponent->IsA(ComponentClass))
				{
					InFunc(static_cast<ComponentType*>(OwnedComponent));
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	FName IconName;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	FText NodeTitle;

	UPROPERTY(VisibleDefaultsOnly, Category = "GenericGraphNode_Editor")
	TSubclassOf<UGenericGraph> CompatibleGraphType;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	FLinearColor BackgroundColor;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	FText ContextMenuName;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	ENodeLimit ParentLimitType;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor" ,meta = (ClampMin = "0",EditCondition = "ParentLimitType == ENodeLimit::Limited", EditConditionHides))
	int32 ParentLimit;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor")
	ENodeLimit ChildrenLimitType;

	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode_Editor" ,meta = (ClampMin = "0",EditCondition = "ChildrenLimitType == ENodeLimit::Limited", EditConditionHides))
	int32 ChildrenLimit;
#endif

#if WITH_EDITOR
	FName GetCustomIconName() const;

	virtual bool IsNameEditable() const;

	virtual FLinearColor GetBackgroundColor() const;

	virtual FText GetNodeTitle() const;

	virtual void SetNodeTitle(const FText& NewTitle);

	virtual bool CanCreateConnection(UGenericGraphNode* Other, FText& ErrorMessage);

	virtual bool CanCreateConnectionTo(UGenericGraphNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage);
	virtual bool CanCreateConnectionFrom(UGenericGraphNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage);
#endif
};
