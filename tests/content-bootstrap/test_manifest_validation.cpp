#include "../../sugarbomb/SbGameFramework/Content/OpenF3ContentValidator.hpp"

#include <iostream>

int main()
{
	using namespace sbe::SbGameFramework::Content;
	
	OpenF3ContentValidator Validator;
	
	OpenF3ContentManifest ValidManifest;
	ValidManifest.installRoots = {"FalloutNV"};
	ValidManifest.dataRoots = {"Data"};
	ValidManifest.plugins = {
		{"FalloutNV.esm", {}},
		{"DeadMoney.esm", {}},
		{"MyGameplayPatch.esp", {"FalloutNV.esm", "DeadMoney.esm"}}
	};
	ValidManifest.archives = {"Fallout - Meshes.bsa"};
	const auto ValidResult{Validator.Validate(ValidManifest)};
	
	if(ValidResult.HasErrors())
	{
		std::cerr << "Expected valid manifest to pass validation." << std::endl;
		return 1;
	};
	
	OpenF3ContentManifest MissingMasterManifest;
	MissingMasterManifest.installRoots = {"FalloutNV"};
	MissingMasterManifest.dataRoots = {"Data"};
	MissingMasterManifest.plugins = {
		{"OnlyPlugin.esp", {"FalloutNV.esm"}}
	};
	const auto MissingMasterResult{Validator.Validate(MissingMasterManifest)};
	
	if(!MissingMasterResult.HasErrors())
	{
		std::cerr << "Expected missing-master manifest to fail validation." << std::endl;
		return 1;
	};
	
	return 0;
}
