#include "FItemPoolDataCustomization.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "DetailLayoutBuilder.h"
#include "IDetailChildrenBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "ScopedTransaction.h"
#include "IDocumentation.h"
#include "IDetailGroup.h"

#define LOCTEXT_NAMESPACE "ItemPoolDataCustomization"

#define RowWidth_Customization 150
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
	//IPropertyTypeCustomization::CustomizeChildren(StructPropertyHandle, StructBuilder, StructCustomizationUtils);

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

	FName ItemPoolSetupGroupName = TEXT(""); //ItemPool
	FText ItemPoolSetupGroupLabel = LOCTEXT("ItemPoolSetupLabel", "ItemPool Setup");
	IDetailGroup* ItemPoolSetupGroup = NULL;

	for (uint32 ChildIdx = 0; ChildIdx < NumChildren; ++ChildIdx)
	{
		TSharedRef<IPropertyHandle> ChildProperty = StructPropertyHandle->GetChildHandle(ChildIdx).ToSharedRef();
		const FString PropertyName = ChildProperty->GetProperty() ? ChildProperty->GetProperty()->GetName() : TEXT("");

		if (PropertyName == TEXT("ItemPools"))
		{
	/*		if (ItemPoolSetupGroup == NULL)
			{
				ItemPoolSetupGroup = &StructBuilder.AddGroup(ItemPoolSetupGroupName, ItemPoolSetupGroupLabel);
				BuildColumnsHeaderHelper(StructPropertyHandle, StructBuilder.AddCustomRow(ItemPoolSetupGroupLabel));
			}*/

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
				//BuildColumnsHeaderHelper(StructPropertyHandle, StructBuilder.AddCustomRow(ItemPoolSetupGroupLabel));

			//TSharedRef<FDetailArrayBuilder> ItemPoolArrayBuilder = MakeShareable(new FDetailArrayBuilder(ChildProperty, false));
			//ItemPoolArrayBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FItemPoolDataCustomization::CreateItemPoolUIDelegate));
			//StructBuilder.AddCustomBuilder(ItemPoolArrayBuilder);

			//StructBuilder.AddProperty(ChildProperty);
			//IStructCustomization
		//	}
		//	else
		//	{
		//		const FText PropertyNameText = FText::FromString(PropertyName);
		//		CreateGearUIHelper(StructBuilder.AddCustomRow(PropertyNameText), PropertyNameText, ChildProperty, GearType);
		//	}
		}
		else
		{	//Add all other properties
			StructBuilder.AddProperty(ChildProperty);
		}
	}
}

void FItemPoolDataCustomization::CreateItemPoolUIHelper(FDetailWidgetRow & GearsSetup, FText Label, TSharedRef<IPropertyHandle> GearHandle)
{
	uint32 NumChildren = 0;
	GearHandle->GetNumChildren(NumChildren);	//we use num of children to determine if we are dealing with a full gear that has ratio, down, up - or just a single value

	TSharedRef<SWidget> RatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("SelectedItemPool")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	TSharedRef<SWidget> DownRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("Weight")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	TSharedRef<SWidget> UpRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("Probability")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	//PropertyCustomizationHelpers::MakeUseSelectedButton
	TSharedRef<SWidget> RemoveWidget = PropertyCustomizationHelpers::MakeDeleteButton(FSimpleDelegate::CreateSP(this, &FItemPoolDataCustomization::RemoveGear, GearHandle), LOCTEXT("RemoveGearToolTip", "Removes gear"));
	RemoveWidget->SetEnabled(NumChildren > 1);

	//StructBuilder.AddProperty(GearHandle);

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

void FItemPoolDataCustomization::CreateItemPoolUIHelper2(FDetailWidgetRow & GearsSetup, FText Label, TSharedRef<IPropertyHandle> GearHandle)
{
	uint32 NumChildren = 0;
	GearHandle->GetNumChildren(NumChildren);	//we use num of children to determine if we are dealing with a full gear that has ratio, down, up - or just a single value

	TSharedRef<SWidget> RatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("ItemPool")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	TSharedRef<SWidget> DownRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("Weight")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	TSharedRef<SWidget> UpRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("Probability")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	//PropertyCustomizationHelpers::MakeUseSelectedButton
	
	//TSharedRef<SWidget> RemoveWidget = PropertyCustomizationHelpers::MAKEex(FSimpleDelegate::CreateSP(this, &FItemPoolDataCustomization::RemoveGear, GearHandle), LOCTEXT("RemoveGearToolTip", "Removes gear"));
	//RemoveWidget->SetEnabled(NumChildren > 1);
	//PropertyCustomizationHelpers::
	TSharedRef<SWidget> Rati22Widget = GearHandle->CreatePropertyValueWidget();
	TSharedRef<SWidget> Rati2332Widget = GearHandle->CreatePropertyNameWidget();

	//StructBuilder.AddProperty(GearHandle);

	//FText t2 = GearHandle->GetChildHandle("Weight")->GetPropertyDisplayName();
	//FText t3 = GearHandle->GetChildHandle("Probability")->GetPropertyDisplayName();

	GearsSetup
	.NameContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			Rati2332Widget
		]
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
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			[
				Rati22Widget
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(RowWidth_Customization)
				.HAlign(HAlign_Left)
				.Content()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ItemPoolLabel", "ItemPo3olLabel"))
					//.Font(IDetailLayoutBuilder::GetDetailFontBold())
				]
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
				//IStructCustomization
				SNew(STextBlock)
				.Text(LOCTEXT("WeightLabel", "ItemPoolLabel"))
				//.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.3333f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ProbabilityLabel", "ItemPoolLabel"))
			//.Font(IDetailLayoutBuilder::GetDetailFontBold())
		]
	];
}

void FItemPoolDataCustomization::CreateItemPoolUIDelegate(TSharedRef<IPropertyHandle> PropertyHandle, int32 ItemPoolIndex, IDetailChildrenBuilder & ChildrenBuilder)
{
	FText Label = FText::Format(LOCTEXT("ItemPoolGear", "Gear {0}"), FText::AsNumber(ItemPoolIndex + 1));
	CreateItemPoolUIHelper(ChildrenBuilder.AddCustomRow(Label), Label, PropertyHandle);
	CreateItemPoolUIHelper2(ChildrenBuilder.AddCustomRow(Label), Label, PropertyHandle);
}

void FItemPoolDataCustomization::BuildColumnsHeaderHelper(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow & GearsSetup)
{
	GearsSetup
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ItemPoolSetup", "")) // ItemPool Setup
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
				//.ColorAndOpacity(COLOR_BLUE)
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
				//.ColorAndOpacity(COLOR_RED)
				//.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.3333f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ProbabilityLabel", "Probability"))
			//.ColorAndOpacity(COLOR_GREEN)
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