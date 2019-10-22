#include "HordeModeEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "PropertyEditorModule.h"
#include "HordeLootPoolCustomization.h"
#include "FItemPoolDataCustomization.h"

IMPLEMENT_GAME_MODULE(FHordeModeEditorModule, HordeModeEditor);

DEFINE_LOG_CATEGORY(HordeModeEditor)

#define LOCTEXT_NAMESPACE "HordeModeEditor"

void FHordeModeEditorModule::StartupModule()
{
	UE_LOG(HordeModeEditor, Warning, TEXT("HordeModeEditor: Log Started"));

	// Register detail customizations
	auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("HordeItemPoolDataAsset", FOnGetDetailCustomizationInstance::CreateStatic(&FHordeLootPoolCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("ListItemPoolData", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FItemPoolDataCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FHordeModeEditorModule::ShutdownModule()
{
	UE_LOG(HordeModeEditor, Warning, TEXT("HordeModeEditor: Log Ended"));

	auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropertyModule.UnregisterCustomClassLayout("HordeItemPoolDataAsset");
	PropertyModule.UnregisterCustomClassLayout("ListItemPoolData");
}

#undef LOCTEXT_NAMESPACE