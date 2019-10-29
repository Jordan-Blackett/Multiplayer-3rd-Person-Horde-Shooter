#include "HordeLootPoolCustomization.h"
#include "Loot/HordeItemPoolDataAsset.h"
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

#define LOCTEXT_NAMESPACE "HordeModeEditorModule"

TSharedRef<IDetailCustomization> FHordeLootPoolCustomization::MakeInstance()
{
	return MakeShareable(new FHordeLootPoolCustomization);
}

void FHordeLootPoolCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	Builder = &DetailBuilder;

	//TSharedRef<IPropertyHandle> PropertyArray = DetailBuilder.GetProperty("ItemPool");
	//check(PropertyArray->IsValidHandle());

	//TSharedRef<FDetailArrayBuilder> ArrayChildBuilder = MakeShareable(new FDetailArrayBuilder(PropertyArray)); //.ToSharedRef()
	//ArrayChildBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FHordeLootPoolCustomization::OnBuildChild));

	//IDetailCategoryBuilder& AttributesCategory = DetailBuilder.EditCategory("ItemPools", FText::GetEmpty(), ECategoryPriority::Important);
	//check(AttributesCategory.IsParentLayoutValid());
	//AttributesCategory.AddCustomBuilder(ArrayChildBuilder);

	//PropertyArray->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FHordeLootPoolCustomization::RefreshDelegate));
	//PropertyArray->SetOnChildPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FHordeLootPoolCustomization::RefreshDelegate));
}

void FHordeLootPoolCustomization::OnBuildChild(TSharedRef<IPropertyHandle> ChildHandle, int32 ElementIndex, IDetailChildrenBuilder& ChildrenBuilder)
{
	// Add an overridden combo box
	IDetailPropertyRow& PropertyArrayRow = ChildrenBuilder.AddProperty(ChildHandle);
	//ChildHandle.GetPropertyClass();
	//Cast<FItemPoolData>(ChildHandle.Get().GetPropertyClass())->Weight;
	//ChildrenBuilder.
	//TSharedPtr<IPropertyHandle> Weight = ChildrenBuilder.get
	TSharedPtr<IPropertyHandle> PropertyGetArray = ChildHandle->GetChildHandle("Ratio");

	//PropertyGetArray.Get()->GetValueAsFormattedText();
	//TSharedRef<SWidget> RatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("Ratio")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	//TSharedRef<SWidget> DownRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("DownRatio")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());
	//TSharedRef<SWidget> UpRatioWidget = (NumChildren > 1 ? GearHandle->GetChildHandle("UpRatio")->CreatePropertyValueWidget() : GearHandle->CreatePropertyValueWidget());

	//PropertyArrayRow.GetDefaultWidgets();
	//PropertyArrayRow.
	//FItemPoolData
	//ChildHandle->GetChildHandle("UpRatio")->GetPropertyDisplayName()
	//TEXT(ChildHandle->GetChildHandle("UpRatio")->GetPropertyDisplayName()

	FFormatNamedArguments Arguments;

	FText ItemPoolValueText;
	//FString NameValue;
	//NameValue = ChildHandle->GetChildHandle("ItemPool")->GetValueAsDisplayText()//->GetPropertyClass()//GetProperty()->GetClass()->ClassGeneratedBy->GetName();  //ChildHandle->GetChildHandle("ItemPool")->GetProperty()->GetOuter()->GetName();
	ChildHandle->GetChildHandle("ItemPool")->GetValueAsFormattedText(ItemPoolValueText);

	//FString unwantedBit = TEXT("ItemPool=BlueprintGeneratedClass'" / Game / Blueprints / LootTables / DA_LootPool_GunsAndGear_All.DA_LootPool_GunsAndGear_All_C"') ");
//	resolutionString->RemoveFromStart(unwantedBit);
	FString t;
	//ChildHandle->GetChildHandle("ItemPool")->GetValueAsDisplayString(t);
	//int32 err = t.Find("DA_");
	//int32 err2 = t.Find(".DA_");
	//int32 c = t.Len() - err2;
	//t.RemoveAt(0, err);
	//t.RemoveAt(err2, c);
	//t = ChildHandle->GetChildHandle("ItemPool");
	//TSharedPtr<IPropertyHandle> PropertyGetArray2 = ChildHandle->GetChildHandle("ItemPool"); FText::FromString(NameValue)

	//UProperty* const Property = ChildHandle->GetChildHandle("ItemPool")->GetPropertyClass();
	//Property->GetPathName();
	ChildHandle->GetChildHandle("ItemPool")->GetValueAsFormattedString(t);
	FText ItemPoolLabel = FText::Format(LOCTEXT("ItemPoolLabel", "Pool: {0}"), FText::FromString(t));
	//FText ItemPoolLabel = FText::Format(LOCTEXT("ItemPoolLabel", "Pool: {0}"), FText::FromString(ClassName));

	////auto FillClassFilters = [&](TArray<const UClass*> &ClassFilters, FName TagName, FText out)
	////{
	//	//TArray<const UClass*> &ClassFilters;
	//	FName TagName;
	//	const FString* ClassesFilterString = &Property->GetMetaData(TagName);
	//	
	//	//ClassFilters.Empty();

	//	if (!ClassesFilterString->IsEmpty())
	//	{
	//		TArray<FString> ClassFilterNames;
	//		ClassesFilterString->ParseIntoArray(ClassFilterNames, TEXT(","), true);

	//		for (FString& ClassName : ClassFilterNames)
	//		{
	//			ItemPoolLabel = FText::Format(LOCTEXT("ItemPoolLabel", "Pool: {0}"), FText::FromString(ClassName));
	//		}
	//	}
	////};

	//const FString* ClassesFilterString = &ChildHandle->GetChildHandle("ItemPool")->GetProperty()->GetMetaData(TagName);

	//TArray<FString> ClassFilterNames;
	//ClassesFilterString->ParseIntoArray(ClassFilterNames, TEXT(","), true);

	//ClassName.TrimStartAndEndInline();
	//UClass* Class = FindObject<UClass>(ANY_PACKAGE, *ClassName);

	//void* ItemPoolValueAsVoidPtr;
	//ChildHandle->GetChildHandle("ItemPool")->GetValueData(ItemPoolValueAsVoidPtr);
	//UClass *ItemPoolValue = (UClass*)&ItemPoolValueAsVoidPtr;
	//ItemPoolValue->GetName();
	//FText Label44 = FText::Format(LOCTEXT("ItemPoolLabel", "Pool: {0}"), FText::FromString(ItemPoolValue->GetName()));
	//UClass*
	//BlueprintGeneratedClass'/Game/Blueprints/LootTables/DA_LootPool_GunsAndGear_All.DA_LootPool_GunsAndGear_All_C'

	//FSimpleDelegate te;
	//te.CreateSP(this, &FHordeLootPoolCustomization::OnBuildChild);
	//te.BindSP(this, &FHordeLootPoolCustomization::CreateItemPoolUIDelegate);

	//TSharedRef<FDetailArrayBuilder> ArrayChildBuilder = MakeShareable(new FDetailArrayBuilder(PropertyArray)); //.ToSharedRef()
	//ArrayChildBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateSP(this, &FHordeLootPoolCustomization::OnBuildChild));

	//ChildHandle->GetChildHandle("ItemPool")->SetOnPropertyValueChanged(FSimpleDelegate::CreateSP(this, &FHordeLootPoolCustomization::CreateItemPoolUIDelegate, DetailBuilder));

	// Weight Label
	FText WeightValueText;
	FText WeightModifierValueText;
	ChildHandle->GetChildHandle("Weight")->GetValueAsDisplayText(WeightValueText);
	ChildHandle->GetChildHandle("WeightModifier")->GetValueAsDisplayText(WeightModifierValueText);
	FText WeightLabel;
	void* WeightValueAsVoidPtr;
	ChildHandle->GetChildHandle("WeightModifier")->GetValueData(WeightValueAsVoidPtr);
	float WeightValue = *(float *)&WeightValueAsVoidPtr;
	if (WeightValue > 0.0f) {
		WeightLabel = FText::Format(LOCTEXT("WeightLabel", "{0} * {1}"), WeightValueText, WeightModifierValueText);
	} else {
		WeightLabel = FText::Format(LOCTEXT("WeightLabel", "{0}"), WeightValueText);
	}

	// Probability Label
	FText ProbabilityValueText;
	ChildHandle->GetChildHandle("Probability")->GetValueAsDisplayText(ProbabilityValueText);
	void* ProbabilityValueAsVoidPtr;
	ChildHandle->GetChildHandle("Probability")->GetValueData(ProbabilityValueAsVoidPtr);
	float ProbabilityValue = *(float *)&ProbabilityValueAsVoidPtr;
	float **Jim = (float**)&ProbabilityValueAsVoidPtr;

	FNumberFormattingOptions format;
	format.SetMinimumFractionalDigits(5);
	format.SetMaximumFractionalDigits(5);
	FText ProbabilityLabel = FText::Format(LOCTEXT("ProbabilityLabel", "{value}"), FText::AsPercent(**Jim, &format));

	FText Label = FText::Format(LOCTEXT("ItemPoolLabel", "ItemPool {0}"), FText::AsNumber(ElementIndex));

	TSharedRef<SWidget> RemoveWidget = PropertyCustomizationHelpers::MakeDeleteButton(FSimpleDelegate::CreateSP(this, &FHordeLootPoolCustomization::RemoveItemPool, ChildHandle), LOCTEXT("RemoveGearToolTip", "Removes gear"));

	FResetToDefaultOverride ResetToDefault;
	PropertyArrayRow = PropertyArrayRow.OverrideResetToDefault(ResetToDefault.Hide(true));
	PropertyArrayRow.ShowPropertyButtons(false);
	PropertyArrayRow.CustomWidget(true)
	.NameContent()
	[
		//ChildHandle->CreatePropertyNameWidget()
		SNew(STextBlock)
		.Text(Label)
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 10.0f, 0.f)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.HAlign(HAlign_Left)
			.Content()
			[
				SNew(STextBlock)
				.Text(ItemPoolLabel)
				.ColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 0.9f))
				//.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 10.0f, 0.f)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			//.Padding()
			.HAlign(HAlign_Left)
			.Content()
			[
				SNew(STextBlock)
				.Text(WeightLabel)
				.ColorAndOpacity(FLinearColor(0.f, 0.f, 1.f, 0.9f))
				//.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 10.0f, 0.f)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.HAlign(HAlign_Left)
			.Content()
			[
				SNew(STextBlock)
				.Text(ProbabilityLabel)
				.ColorAndOpacity(FLinearColor(0.f, 1.f, 0.f, 0.9f))
				//.Font(IDetailLayoutBuilder::GetDetailFontBold())
			]
		]
		+ SHorizontalBox::Slot()
		.Padding(4.f)
		.AutoWidth()
		[
			RemoveWidget
		]
	];
}

void FHordeLootPoolCustomization::RemoveItemPool(TSharedRef<IPropertyHandle> PoolHandle)
{
	if (PoolHandle->IsValidHandle())
	{
		const TSharedPtr<IPropertyHandle> ParentHandle = PoolHandle->GetParentHandle();
		const TSharedPtr<IPropertyHandleArray> ParentArrayHandle = ParentHandle->AsArray();

		ParentArrayHandle->DeleteItem(PoolHandle->GetIndexInArray());
	}
}

void FHordeLootPoolCustomization::RefreshDelegate()
{
	if (Builder)
	{
		Builder->ForceRefreshDetails();
	}
}

#undef LOCTEXT_NAMESPACE