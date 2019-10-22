#include "FItemPoolDataCustomization.h"
//#include "WheeledVehicleMovementComponent4W.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "ScopedTransaction.h"
#include "IDocumentation.h"
#include "IDetailGroup.h"

#define LOCTEXT_NAMESPACE "ItemPoolDataCustomization"

#define RowWidth_Customization 50
#define GearColumnsWidth (75.f * 3.f)

////////////////////////////////////////////////////////////////

void FItemPoolDataCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow & HeaderRow, IPropertyTypeCustomizationUtils & StructCustomizationUtils)
{
	HeaderRow.
	NameContent()
	[
		StructPropertyHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		StructPropertyHandle->CreatePropertyValueWidget()
	];
}

void FItemPoolDataCustomization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder & StructBuilder, IPropertyTypeCustomizationUtils & StructCustomizationUtils)
{
	// copy all transmision instances I'm accessing right now
	TArray<void*> StructPtrs;
	StructPropertyHandle->AccessRawData(StructPtrs);

	if (StructPtrs.Num() == 1)
	{
		SelectedItemPool = (FListItemPoolData *)StructPtrs[0];
	}
	else
	{
		SelectedItemPool = NULL;
	}

	uint32 NumChildren = 0;
	StructPropertyHandle->GetNumChildren(NumChildren);

	FName ItemPoolSetupGroupName = TEXT("ItemPool");
	FText ItemPoolSetupGroupLabel = LOCTEXT("ItemPoolSetupLabel", "ItemPool Setup");
	IDetailGroup* ItemPoolSetupGroup = NULL;

	for (uint32 ChildIdx = 0; ChildIdx < NumChildren; ++ChildIdx)
	{
		TSharedRef<IPropertyHandle> ChildProperty = StructPropertyHandle->GetChildHandle(ChildIdx).ToSharedRef();
		const FString PropertyName = ChildProperty->GetProperty() ? ChildProperty->GetProperty()->GetName() : TEXT("");

		if (PropertyName == TEXT("ItemPools"))
		{
			if (ItemPoolSetupGroup == NULL)
			{
				ItemPoolSetupGroup = &StructBuilder.AddGroup(ItemPoolSetupGroupName, ItemPoolSetupGroupLabel);
				BuildColumnsHeaderHelper(StructPropertyHandle, StructBuilder.AddCustomRow(ItemPoolSetupGroupLabel));
			}

		//	//determine which gear we're showing
		//	EGearType GearType = ForwardGear;
		//	if (PropertyName == TEXT("NeutralGearUpRatio"))
		//	{
		//		GearType = NeutralGear;
		//	}
		//	else if
		//		(PropertyName == TEXT("ReverseGearRatio"))
		//	{
		//		GearType = ReverseGear;
		//	}

		//	if (GearType == ForwardGear)
		//	{
				TSharedRef<FDetailArrayBuilder> ItemPoolArrayBuilder = MakeShareable(new FDetailArrayBuilder(ChildProperty, false));
				ItemPoolArrayBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FItemPoolDataCustomization::CreateItemPoolUIDelegate));
				StructBuilder.AddCustomBuilder(ItemPoolArrayBuilder);
		//	}
		//	else
		//	{
		//		const FText PropertyNameText = FText::FromString(PropertyName);
		//		CreateGearUIHelper(StructBuilder.AddCustomRow(PropertyNameText), PropertyNameText, ChildProperty, GearType);
		//	}
		}
		else
		{	//Add all other properties
		//	StructBuilder.AddProperty(ChildProperty);
		}
	}
}

void FItemPoolDataCustomization::CreateItemPoolUIHelper(FDetailWidgetRow & GearsSetup, FText Label, TSharedRef<IPropertyHandle> GearHandle)
{
	uint32 NumChildren = 0;
	GearHandle->GetNumChildren(NumChildren);	//we use num of children to determine if we are dealing with a full gear that has ratio, down, up - or just a single value

	TSharedRef<SWidget> RatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("ItemPool")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	TSharedRef<SWidget> DownRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("Weight")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	TSharedRef<SWidget> UpRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("Probability")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());

	TSharedRef<SWidget> RemoveWidget = PropertyCustomizationHelpers::MakeDeleteButton(FSimpleDelegate::CreateSP(this, &FItemPoolDataCustomization::RemoveGear, GearHandle), LOCTEXT("RemoveGearToolTip", "Removes gear"));
	RemoveWidget->SetEnabled(NumChildren > 1);

	GearsSetup
	.NameContent()
	[
		SNew(STextBlock)
		.Text(Label)
		//.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	.MaxDesiredWidth(GearColumnsWidth)
	.MinDesiredWidth(GearColumnsWidth)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		//.FillWidth(0.3333f)
		[
			RatioWidget
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		//.FillWidth(0.3333f)
		//.Padding(4.f)
		[
			DownRatioWidget
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		//.FillWidth(0.3333f)
		//.Padding(4.f)
		[
			UpRatioWidget
		]
		+ SHorizontalBox::Slot()
		.Padding(4.f)
		.AutoWidth()
		[
			RemoveWidget
		]
	];
}

void FItemPoolDataCustomization::CreateItemPoolUIDelegate(TSharedRef<IPropertyHandle> PropertyHandle, int32 ItemPoolIndex, IDetailChildrenBuilder & ChildrenBuilder)
{
	FText Label = FText::Format(LOCTEXT("ItemPoolGear", "Gear {0}"), FText::AsNumber(ItemPoolIndex + 1));
	CreateItemPoolUIHelper(ChildrenBuilder.AddCustomRow(Label), Label, PropertyHandle);
}

void FItemPoolDataCustomization::BuildColumnsHeaderHelper(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow & GearsSetup)
{
	GearsSetup
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ItemPoolSetup", "ItemPool Setup"))
		//.Font(IDetailLayoutBuilder::GetDetailFontBold())
	]
	.ValueContent()
	.MaxDesiredWidth(GearColumnsWidth)
	.MinDesiredWidth(GearColumnsWidth)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.3333f)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(RowWidth_Customization)
			.HAlign(HAlign_Left)
			.Content()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ItemPoolLabel", "ItemPool"))
				//.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.3333f)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.HAlign(HAlign_Left)
			.WidthOverride(RowWidth_Customization)
			.Content()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("WeightLabel", "Weight"))
				//.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.3333f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ProbabilityLabel", "Probability"))
			//.Font(IDetailLayoutBuilder::GetDetailFontBold())
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			PropertyCustomizationHelpers::MakeAddButton(FSimpleDelegate::CreateSP(this, &FItemPoolDataCustomization::AddItemPool, StructPropertyHandle), LOCTEXT("AddGearToolTip", "Adds a new gear"))
		]
		//+ SHorizontalBox::Slot()
		//.AutoWidth()
		//.VAlign(VAlign_Center)
		//[
		//	PropertyCustomizationHelpers::MakeEmptyButton(FSimpleDelegate::CreateSP(this, &FItemPoolDataCustomization::EmptyPool, StructPropertyHandle), LOCTEXT("EmptyGearToolTip", "Removes all gears"))
		//]
	];
}

void FItemPoolDataCustomization::AddItemPool(TSharedRef<IPropertyHandle> StructPropertyHandle)
{
	TSharedPtr<IPropertyHandle> GearsHandle = StructPropertyHandle->GetChildHandle("ItemPools");
	if (GearsHandle->IsValidHandle())
	{
		TSharedPtr<IPropertyHandleArray> GearsArray = GearsHandle->AsArray();
		GearsArray->AddItem();
	}
}

void FItemPoolDataCustomization::RemoveGear(TSharedRef<IPropertyHandle> GearHandle)
{
	if (GearHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = GearHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		ParentArrayHandle->DeleteItem(GearHandle->GetIndexInArray());
	}
}

#undef LOCTEXT_NAMESPACE
#undef GearColumnsWidth
#undef RowWidth_Customization