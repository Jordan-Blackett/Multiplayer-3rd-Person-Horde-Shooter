#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;
struct FListItemPoolData;

class FItemPoolDataCustomization : public IPropertyTypeCustomization
{
public:

	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FItemPoolDataCustomization);
	}

	/** IPropertyTypeCustomization interface */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	FListItemPoolData* SelectedItemPool;

	void CreateItemPoolUIHelper(FDetailWidgetRow& GearsSetup, FText Label, TSharedRef<IPropertyHandle> GearHandle);
	void CreateItemPoolUIHelper2(FDetailWidgetRow& GearsSetup, FText Label, TSharedRef<IPropertyHandle> GearHandle);
	void CreateItemPoolUIDelegate(TSharedRef<IPropertyHandle> PropertyHandle, int32 ItemPoolIndex, IDetailChildrenBuilder& ChildrenBuilder);
	
	void BuildColumnsHeaderHelper(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow & GearsSetup);
	void AddItemPool(TSharedRef<IPropertyHandle> StructPropertyHandle);
	void RemoveGear(TSharedRef<IPropertyHandle> GearHandle);
	//void EmptyGears(TSharedRef<IPropertyHandle> StructPropertyHandle);
	//bool IsAutomaticEnabled() const;
};