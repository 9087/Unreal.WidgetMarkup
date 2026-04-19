// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyElementNode.h"

#include "ObjectElementNode.h"
#include "PropertyBuffer.h"
#include "PropertyChainHandle.h"
#include "Misc/ScopeExit.h"
#include "Utilities/WidgetPropertyPath.h"
#include "UObject/UnrealType.h"

IMPLEMENT_ELEMENT_NODE(FPropertyElementNode, FElementNode)

TSharedRef<FElementNode> FPropertyElementNode::Create(const FStringView& InPropertyName, const FStringView& InPropertyValue)
{
	return MakeShared<FPropertyElementNode>(InPropertyName, InPropertyValue);
}

FPropertyElementNode::FPropertyElementNode(const FStringView& InPropertyName, const FStringView& InPropertyValue, bool bInUseBufferedWrite)
	: PropertyName(InPropertyName)
	, PropertyValue(InPropertyValue)
	, bUseBufferedWrite(bInUseBufferedWrite)
{
}

FPropertyElementNode::~FPropertyElementNode()
{
	if (BufferedPropertyContext.MatchesPath(PropertyPath))
	{
		const TSharedPtr<FPropertyBuffer> PropertyBuffer = BufferedPropertyContext.GetPropertyBuffer();
		if (!ensureMsgf(PropertyBuffer.IsValid(), TEXT("BufferedPropertyContext matches path but has no property buffer.")))
		{
			return;
		}
	}
}

void FPropertyElementNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	if (const TSharedPtr<FPropertyBuffer> PropertyBuffer = BufferedPropertyContext.GetPropertyBuffer(); PropertyBuffer.IsValid())
	{
		PropertyBuffer->AddStructReferencedObjects(Collector);
	}
}

FString FPropertyElementNode::GetReferencerName() const
{
	return TEXT("PropertyElementNode");
}

FElementNode::FResult FPropertyElementNode::OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	check(!Context.IsEmpty());

	TSharedPtr<FElementNode> ObjectNode = Context.GetLastObjectNode();
	if (!ObjectNode.IsValid())
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("No object node in context for property '{0}'.")),
			FText::FromString(PropertyName)));
	}

	UObject* Object = ObjectNode->GetObject();
	if (!Object)
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Object is null for property '{0}'.")),
			FText::FromString(PropertyName)));
	}

	PropertyPath.Reset();
	BufferedPropertyContext.Reset();
	TSharedPtr<FElementNode> Parent = Context.GetLastNode();
	TSharedPtr<FPropertyElementNode> PropertyParent = nullptr;
	if (!Parent.IsValid() || Parent == ObjectNode)
	{
		PropertyPath = PropertyPath.WithAppendedProperty(PropertyName);
	}
	else
	{
		PropertyParent = CastElementNode<FPropertyElementNode>(Parent);
		if (!PropertyParent)
		{
			return FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Expected property parent node for nested property '{0}'.")),
				FText::FromString(PropertyName)));
		}
		else
		{
			PropertyPath = PropertyParent->PropertyPath;
			bool bPropertyPathInitialized = false;
			if (PropertyParent->PropertyChain)
			{
				if (PropertyParent->PropertyChain->IsArrayProperty())
				{
					bUseBufferedWrite = true;
					PropertyPath = PropertyPath.WithAppendedArrayIndex(PropertyParent->ElementChildren.Num());
					bPropertyPathInitialized = true;
				}
			}
			if (!bPropertyPathInitialized)
			{
				PropertyPath = PropertyPath.WithAppendedProperty(PropertyName);
				bPropertyPathInitialized = true;
			}
		}
	}

	if (PropertyParent)
	{
		BufferedPropertyContext = PropertyParent->BufferedPropertyContext;
	}

	if (bUseBufferedWrite && BufferedPropertyContext.InValid())
	{
		TSharedPtr<FPropertyChainHandle> DirectPropertyChain = FPropertyChainHandle::Create(Object, PropertyPath);
		FProperty* TailProperty = DirectPropertyChain.IsValid() ? DirectPropertyChain->GetTailProperty() : nullptr;
		if (!TailProperty)
		{
			return FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Failed to resolve buffered root property for property path '{0}'.")),
				FText::FromString(PropertyPath.GetPathName().ToString())));
		}

		const TSharedPtr<FPropertyBuffer> PropertyBuffer = MakeShared<FPropertyBuffer>(TailProperty);
		if (!PropertyBuffer->HasValue())
		{
			return FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Failed to initialize buffered root value for property path '{0}'.")),
				FText::FromString(PropertyPath.GetPathName().ToString())));
		}

		BufferedPropertyContext = FBufferedPropertyContext(PropertyPath, PropertyBuffer);
		if (BufferedPropertyContext.InValid())
		{
			return FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Failed to initialize buffered root value for property path '{0}'.")),
				FText::FromString(PropertyPath.GetPathName().ToString())));
		}
	}

	if (!BufferedPropertyContext.InValid())
	{
		PropertyChain = FPropertyChainHandle::Create(Object, PropertyPath, BufferedPropertyContext);
	}
	else
	{
		PropertyChain = FPropertyChainHandle::Create(Object, PropertyPath);
	}
	if (!PropertyChain.IsValid())
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Failed to recognize the property path '{0}'.")),
			FText::FromString(PropertyPath.GetPathName().ToString())));
	}

	return FResult::Success();
}

FElementNode::FResult FPropertyElementNode::OnEnd()
{
	if (const bool bShouldSetValue = PropertyChain.IsValid() && !PropertyValue.IsEmpty() && ElementChildren.Num() == 0)
	{
		if (!PropertyChain->SetValue(FStringView(PropertyValue)))
		{
			return FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Failed to set value for property path '{0}'.")),
				FText::FromString(PropertyPath.GetPathName().ToString())));
		}
	}

	if (const bool bMatchesPropertyBufferPath = BufferedPropertyContext.MatchesPath(PropertyPath))
	{
		const TSharedPtr<FPropertyBuffer> PropertyBuffer = BufferedPropertyContext.GetPropertyBuffer();
		if (!ensureMsgf(PropertyBuffer.IsValid(), TEXT("BufferedPropertyContext matches path but has no property buffer.")) || !PropertyBuffer->HasValue())
		{
			return FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Buffered root has no value for property path '{0}'.")),
				FText::FromString(PropertyPath.GetPathName().ToString())));
		}

		TSharedPtr<FPropertyChainHandle> DirectHandle = PropertyChain.IsValid() ? PropertyChain->GetDirectHandle() : nullptr;
		if (!DirectHandle.IsValid() || !DirectHandle->SetValue(*PropertyBuffer))
		{
			return FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Failed to commit buffered root value for property path '{0}'.")),
				FText::FromString(PropertyPath.GetPathName().ToString())));
		}
	}

	PropertyPath.Reset();
	BufferedPropertyContext.Reset();
	PropertyChain = nullptr;
	return FResult::Success();
}

FElementNode::FResult FPropertyElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	ElementChildren.Add(Child);

	if (!PropertyChain.IsValid())
	{
		return FResult::Failure();
	}

	FProperty* TailProperty = PropertyChain->GetTailProperty();
	void* TailValueAddress = PropertyChain->GetTailValueAddress();
	FArrayProperty* ArrayProperty = CastField<FArrayProperty>(TailProperty);
	if (ArrayProperty)
	{
		check(TailValueAddress);
		FScriptArrayHelper ArrayHelper(ArrayProperty, TailValueAddress);
		const int32 NewArrayIndex = ArrayHelper.AddValue();
		void* NewElementPointer = ArrayHelper.GetRawPtr(NewArrayIndex);

		if (auto ChildObjectElementNode = CastElementNode<FObjectElementNode>(Child))
		{
			UObject* ChildObject = ChildObjectElementNode->GetObject();
			if (!ChildObject)
			{
				return FResult::Failure().Error(FText::Format(
					FText::FromString(TEXT("Failed to add object child to array property path '{0}': object element node returned null object.")),
					FText::FromString(PropertyPath.GetPathName().ToString())));
			}
			FObjectPropertyBase* InnerObjectProperty = CastField<FObjectPropertyBase>(ArrayProperty->Inner);
			if (!InnerObjectProperty)
			{
				return FResult::Failure().Error(FText::Format(
					FText::FromString(TEXT("Failed to add child object to array property path '{0}': array inner type '{1}' is not an object property.")),
					FText::FromString(PropertyPath.GetPathName().ToString()),
					FText::FromString(ArrayProperty->Inner ? ArrayProperty->Inner->GetClass()->GetName() : TEXT("null"))));
			}
			if (InnerObjectProperty->PropertyClass && !ChildObject->IsA(InnerObjectProperty->PropertyClass))
			{
				return FResult::Failure().Error(FText::Format(
					FText::FromString(TEXT("Failed to add child object to array property path '{0}': child class '{1}' is not compatible with '{2}'.")),
					FText::FromString(PropertyPath.GetPathName().ToString()),
					FText::FromString(ChildObject->GetClass()->GetName()),
					FText::FromString(InnerObjectProperty->PropertyClass->GetName())));
			}
			InnerObjectProperty->SetObjectPropertyValue(NewElementPointer, ChildObject);
		} 
		else if (auto ChildPropertyElementNode = CastElementNode<FPropertyElementNode>(Child))
		{
			if (!ChildPropertyElementNode->bUseBufferedWrite)
			{
				return FResult::Failure().Error(FText::Format(
					FText::FromString(TEXT("Failed to add child property to array property path '{0}': child property node is not using buffered write mode.")),
					FText::FromString(PropertyPath.GetPathName().ToString())));
			}

			const TSharedPtr<const FPropertyBuffer> ChildPropertyBuffer = ChildPropertyElementNode->GetPropertyBuffer();
			if (!ChildPropertyBuffer.IsValid() || !ChildPropertyBuffer->GetValueData())
			{
				return FResult::Failure().Error(FText::Format(
					FText::FromString(TEXT("Failed to add child property to array property path '{0}': child property buffer is invalid or uninitialized.")),
					FText::FromString(PropertyPath.GetPathName().ToString())));
			}

			FProperty* ChildProperty = ChildPropertyBuffer->GetProperty();
			if (!ChildProperty)
			{
				return FResult::Failure().Error(FText::Format(
					FText::FromString(TEXT("Failed to add child property to array property path '{0}': child buffer root property is null.")),
					FText::FromString(PropertyPath.GetPathName().ToString())));
			}

			if (!ArrayProperty->Inner->SameType(ChildProperty))
			{
				return FResult::Failure().Error(FText::Format(
					FText::FromString(TEXT("Failed to add child property to array property path '{0}': array inner type '{1}' is not compatible with child buffered root type '{2}'.")),
					FText::FromString(PropertyPath.GetPathName().ToString()),
					FText::FromString(ArrayProperty->Inner->GetClass()->GetName()),
					FText::FromString(ChildProperty->GetClass()->GetName())));
			}

			ArrayProperty->Inner->CopyCompleteValue(NewElementPointer, ChildPropertyBuffer->GetValueData());
		}
	}


	return FResult::Success();
}

UStruct* FPropertyElementNode::GetPropertyOwnerStruct() const
{
	if (!PropertyChain.IsValid())
	{
		return nullptr;
	}
	FProperty* TailProperty = PropertyChain->GetTailProperty();
	if (!TailProperty)
	{
		return nullptr;
	}
	if (FStructProperty* StructProperty = CastField<FStructProperty>(TailProperty))
	{
		return StructProperty->Struct;
	}
	if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(TailProperty))
	{
		return ObjectProperty->PropertyClass;
	}
	return nullptr;
}

bool FPropertyElementNode::HasProperty(const FStringView& AttributeName)
{
	return PropertyChain.IsValid() && PropertyChain->GetChildHandle(AttributeName).IsValid();
}

void FPropertyElementNode::SetPropertyRun(TSharedPtr<IPropertyRun> InPropertyRun)
{
	PropertyRun = InPropertyRun;
}
