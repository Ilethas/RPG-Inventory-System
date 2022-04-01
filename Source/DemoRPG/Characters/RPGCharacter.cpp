#include "RPGCharacter.h"

#include "DemoRPG/RPGAssetManager.h"

FPrimaryAssetId URPGCharacter::GetPrimaryAssetId() const
{
	// This is a DataAsset and not a blueprint so we can just use the raw FName
	// For blueprints you need to handle stripping the _C suffix
	return FPrimaryAssetId(URPGAssetManager::CharacterType, GetFName());
}
