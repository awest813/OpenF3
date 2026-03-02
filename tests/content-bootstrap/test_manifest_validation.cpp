#include "../../sugarbomb/SbGameFramework/Content/OpenF3ContentValidator.hpp"

#include <iostream>

int main()
{
	using namespace sbe::SbGameFramework::Content;
	
	OpenF3ContentValidator Validator;
	
	OpenF3ContentManifest ValidManifest;
	ValidManifest.dataRoots = {"Data"};
	ValidManifest.plugins = {"FalloutNV.esm", "DeadMoney.esm", "MyGameplayPatch.esp"};
	ValidManifest.archives = {"Fallout - Meshes.bsa"};
	const auto ValidResult{Validator.Validate(ValidManifest)};
	
	if(ValidResult.HasErrors())
	{
		std::cerr << "Expected valid manifest to pass validation." << std::endl;
		return 1;
	};
	
	OpenF3ContentManifest MissingMasterManifest;
	MissingMasterManifest.dataRoots = {"Data"};
	MissingMasterManifest.plugins = {"OnlyPlugin.esp"};
	const auto MissingMasterResult{Validator.Validate(MissingMasterManifest)};
	
	if(!MissingMasterResult.HasErrors())
	{
		std::cerr << "Expected missing-master manifest to fail validation." << std::endl;
		return 1;
	};
	
	return 0;
}
