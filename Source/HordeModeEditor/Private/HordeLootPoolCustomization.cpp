#include "HordeLootPoolCustomization.h"
#include "Loot/HordeItemPoolDataAsset.h" // The class we're customizing
#include "PropertyEditing.h"

#include "DetailCustomizations.h"
#include "IDetailChildrenBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"

#include "Widgets/SWidget.h"
#include "Widgets/SBoxPanel.h"
#include "ComboBox.h"
#include "PropertyCustomizationHelpers.h"

#define RowWidth_Customization 50
#define GearColumnsWidth (75.f * 3.f)

#define LOCTEXT_NAMESPACE "HordeModeEditorModule"

TSharedRef<IDetailCustomization> FHordeLootPoolCustomization::MakeInstance()
{
	return MakeShareable(new FHordeLootPoolCustomization);
}

void FHordeLootPoolCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	// This is where the core of the customization code will go.
	//TSharedRef<IPropertyHandle> Property = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UHordeItemPoolDataAsset, Probability));

	IDetailCategoryBuilder& Catagory = DetailBuilder.EditCategory(TEXT("Test"), FText::GetEmpty(), ECategoryPriority::Important);
	TSharedRef<IPropertyHandle> Property = DetailBuilder.GetProperty("Probability");

	//TSharedRef<IPropertyHandle> MaterialProperty = DetailCategory.GetProperty("Materials");

	
	UHordeItemPoolDataAsset* Details = nullptr;
	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);
	for (TWeakObjectPtr<UObject> Object : CustomizedObjects)
	{
		if (Object.IsValid())
		{
			//UHordeItemPoolDataAsset
		}
	}

	Catagory.AddCustomRow(LOCTEXT("RowSearchName", "test"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("DetailName", "Button --->"))
			//.Font(IDetailLayoutBuilder::GetDetailFont())
		]
	.ValueContent()
		[
			SNew(SButton)
			.Text(LOCTEXT("ButtonText", "Button"))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("ButtonToolTip", "TIP"))
		];
	
	


		Catagory.AddCustomRow(LOCTEXT("RowSearchName", "test2"))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				[
					// Make a new SProperty
					SNew(SProperty, Property)
				]
				+ SHorizontalBox::Slot()
				.Padding(4.0f, 0.0f)
				.MaxWidth(50)
				[
					SNew(SProperty, Property)
					//.DisplayName.IsEnabled(false)// Hide the name
				]
			];


		//FDetailArrayBuilder test(Property2);
		//test.OnGenerateArrayElementWidget(&FHordeLootPoolCustomization::OnGenerateElementForMaterials);
		//Catagory.AddCustomRow(LOCTEXT("RowSearchName", "test2"))
		//	.NameContent()
		//	[
		//		SNew(STextBlock)
		//		.Text(LOCTEXT("DetailName", "Button --->"))
		//	//.Font(IDetailLayoutBuilder::GetDetailFont())
		//	]
		//	.ValueContent()
		//	[
		//		SNew(test, MaterialProperty)

		// Populate combo boxes with material property list
		//FMaterialAttributeDefinitionMap::GetDisplayNameToIDList(AttributeNameToIDList);

		//AttributeDisplayNameList.Empty(AttributeNameToIDList.Num());
		//for (const TPair<FString, FGuid>& NameGUIDPair : AttributeNameToIDList)
		//{
		//	AttributeDisplayNameList.Add(MakeShareable(new FString(NameGUIDPair.Key)));
		//}


		//TSharedRef<IPropertyHandle> StructPropertyHandle = DetailBuilder.GetProperty("Pools");
		//
		//// copy all transmision instances I'm accessing right now
		//TArray<void*> StructPtrs;
		////StructPropertyHandle->AccessRawData(StructPtrs);




		//BuildColumnsHeaderHelper(StructPropertyHandle, StructBuilder.AddCustomRow(GearsSetupGroupLabel));

		//for (uint32 ChildIdx = 0; ChildIdx < NumChildren; ++ChildIdx)
		//{


		//	TSharedRef<IPropertyHandle> ChildProperty = StructPropertyHandle->GetChildHandle(ChildIdx).ToSharedRef();
		//	const FString PropertyName = ChildProperty->GetProperty() ? ChildProperty->GetProperty()->GetName() : TEXT("");





		//IDetailCategoryBuilder& Catagory2 = DetailBuilder.EditCategory(TEXT("Pools"), FText::GetEmpty(), ECategoryPriority::Important);
		//TSharedRef<IPropertyHandle> PropertyArray = DetailBuilder.GetProperty("Pools");

		//TSharedPtr<IPropertyHandle> PropertyGetArray = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UHordeItemPoolDataAsset, Pools));

		//check(PropertyArray->IsValidHandle());

		//// Add builder for children to handle array changes
		//TSharedRef<FDetailArrayBuilder> ArrayChildBuilder = MakeShareable(new FDetailArrayBuilder(PropertyArray)); //.ToSharedRef()
		//ArrayChildBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FHordeLootPoolCustomization::OnBuildChild));

		//IDetailCategoryBuilder& AttributesCategory = DetailBuilder.EditCategory("Pools", FText::GetEmpty(), ECategoryPriority::Important);
		//AttributesCategory.AddCustomBuilder(ArrayChildBuilder);

		////TSharedPtr<IPropertyHandle> Weight = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(FItemPoolData, Weight));
}

void FHordeLootPoolCustomization::OnBuildChild(TSharedRef<IPropertyHandle> ChildHandle, int32 ElementIndex, IDetailChildrenBuilder& ChildrenBuilder)
{
	// Add an overridden combo box
	IDetailPropertyRow& PropertyArrayRow = ChildrenBuilder.AddProperty(ChildHandle);
	//ChildHandle.GetPropertyClass();
	//Cast<FItemPoolData>(ChildHandle.Get().GetPropertyClass())->Weight;
	//ChildrenBuilder.
	//TSharedPtr<IPropertyHandle> Weight = ChildrenBuilder.get
	//TSharedPtr<IPropertyHandle> PropertyGetArray = &ChildHandle.Get().GetProperty(GET_MEMBER_NAME_CHECKED(UHordeItemPoolDataAsset, Pools));


	//TSharedRef<SWidget> RatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("Ratio")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	//TSharedRef<SWidget> DownRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("DownRatio")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	//TSharedRef<SWidget> UpRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("UpRatio")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());


	//FItemPoolData
	PropertyArrayRow.CustomWidget()
	.NameContent()
	[
		ChildHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			//.OptionsSource(&AttributeDisplayNameList)
			//.OnGenerateWidget_Lambda([](TSharedPtr<FString> InItem)
			//{
			//	return SNew(STextBlock)
			//	//.Font(IDetailLayoutBuilder::GetDetailFont())
			//	.Text(FText::FromString(*InItem));
			//})
			//.OnSelectionChanged_Lambda([=](TSharedPtr<FString> Selection, ESelectInfo::Type)
			//{
			//	if (ChildHandle->IsValidHandle())
			//	{
			//		//// Convert display name to attribute ID
			//		//for (const auto& NameIDPair : AttributeNameToIDList)
			//		//{
			//		//	if (NameIDPair.Key == *Selection)
			//		//	{
			//		//		ChildHandle->SetValueFromFormattedString(NameIDPair.Value.ToString(EGuidFormats::Digits));
			//		//		break;
			//		//	}
			//		//}
			//	}
			//})
			//.ContentPadding(FMargin(2, 0))
			//[
			//	SNew(STextBlock)
			//	//.Font(IDetailLayoutBuilder::GetDetailFont())
			//	.Text_Lambda([=]() -> FText
			//	{
			//		if (ChildHandle->IsValidHandle())
			//		{
			//			// Convert attribute ID string to display name
			//			FString IDString; FGuid IDValue;
			//			ChildHandle->GetValueAsFormattedString(IDString);
			//			FGuid::ParseExact(IDString, EGuidFormats::Digits, IDValue);

			//			FString AttributeName = "Test"; //FMaterialAttributeDefinitionMap::GetDisplayName(IDValue);
			//			return FText::FromString(AttributeName);
			//		}

			//		return FText::GetEmpty();
			//	})
			//]
		]
		+ SHorizontalBox::Slot()
		.Padding(100.0f, 0.0f)
		.MaxWidth(50)
		[
			SNew(SProperty, ChildHandle)
			.DisplayName(FText::FromString("Weight:"))
		]
		+ SHorizontalBox::Slot()
		.Padding(200.0f, 0.0f)
		[
			SNew(SProperty, ChildHandle)
			.DisplayName(FText::FromString("Weight:"))
		]
	];
}

void FHordeLootPoolCustomization::BuildColumnsHeaderHelper(TSharedRef<class IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& LootPoolSetup)
{

	LootPoolSetup
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ListDefinition", "List Definition"))
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
					.Text(LOCTEXT("PoolTableLabel", "Pool Table"))
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
			.Text(LOCTEXT("HighRPMLabel", "Up Ratio"))
			//.Font(IDetailLayoutBuilder::GetDetailFontBold())
		]
		//+ SHorizontalBox::Slot()
		//.AutoWidth()
		//.VAlign(VAlign_Center)
		//[
		//	PropertyCustomizationHelpers::MakeAddButton(FSimpleDelegate::CreateSP(this, &FVehicleTransmissionDataCustomization::AddGear, StructPropertyHandle), LOCTEXT("AddGearToolTip", "Adds a new gear"))
		//]
		//+ SHorizontalBox::Slot()
		//.AutoWidth()
		//.VAlign(VAlign_Center)
		//[
		//	PropertyCustomizationHelpers::MakeEmptyButton(FSimpleDelegate::CreateSP(this, &FVehicleTransmissionDataCustomization::EmptyGears, StructPropertyHandle), LOCTEXT("EmptyGearToolTip", "Removes all gears"))
		//]
	];
}

#undef LOCTEXT_NAMESPACE
#undef GearColumnsWidth
#undef RowWidth_Customization