#pragma once

#pragma once

#include "IDetailCustomization.h"

class IPropertyHandle;
class IDetailChildrenBuilder;
class FDetailWidgetRow;

class FHordeLootPoolCustomization : public IDetailCustomization
{
public:
	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	// End of IDetailCustomization interface

	static TSharedRef<IDetailCustomization> MakeInstance();

private:
	void OnBuildChild(TSharedRef<IPropertyHandle> ChildHandle, int32 ElementIndex, IDetailChildrenBuilder& ChildrenBuilder);
	void RemoveItemPool(TSharedRef<IPropertyHandle> PoolHandle);

	void RefreshDelegate();

	IDetailLayoutBuilder* Builder;

};