#include "RPGCharacterAppearance.h"

#include "DemoRPG/RPGAssetManager.h"

const FName URPGCharacterAppearanceTags::Appearance = TEXT("Appearance");
const FName URPGCharacterAppearanceTags::Body = TEXT("Body");

FPrimaryAssetId URPGCharacterAppearance::GetPrimaryAssetId() const
{
	// This is a DataAsset and not a blueprint so we can just use the raw FName
	// For blueprints you need to handle stripping the _C suffix
	return FPrimaryAssetId(URPGAssetManager::CharacterAppearanceType, GetFName());
}

void URPGCharacterModel::ApplyModel(IRPGCharacterModelVisitorInterface* Visitor)
{
	Visitor->ApplyModel(this);
}

void URPGModularCharacterModel::ApplyModel(IRPGCharacterModelVisitorInterface* Visitor)
{
	Visitor->ApplyModel(this);
}

URPGCharacterModelBase* URPGCharacterAppearance::GetModel(const FName InModelTag)
{
	for (URPGCharacterModelBase* Model : Models)
	{
		if (IsValid(Model) && Model->ModelTag == InModelTag)
		{
			return Model;
		}
	}
	return nullptr;
}
