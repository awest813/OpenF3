/*
*******************************************************************************

Copyright (C) 2026 SugarBombEngine Developers

This file is part of SugarBombEngine

SugarBombEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SugarBombEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SugarBombEngine. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************
*/

/// @file

#include "OpenF3BootstrapReport.hpp"

#include <sstream>

namespace sbe::SbGameFramework::Content
{

std::string OpenF3BootstrapReport::Render(const OpenF3ContentManifest &aManifest, const OpenF3ContentValidationResult &aValidation)
{
	std::ostringstream Stream;
	Stream << "=== OpenF3 Content Bootstrap Report ===\n";
	Stream << "Bootstrap-only mode: " << (aManifest.bootstrapOnly ? "enabled" : "disabled") << "\n";
	Stream << "Data roots (" << aManifest.dataRoots.size() << "):\n";
	
	for(const auto &sDataRoot : aManifest.dataRoots)
		Stream << "  - " << sDataRoot << "\n";
	
	Stream << "Plugins (" << aManifest.plugins.size() << "):\n";
	for(const auto &sPlugin : aManifest.plugins)
		Stream << "  - " << sPlugin << "\n";
	
	Stream << "Archives (" << aManifest.archives.size() << "):\n";
	for(const auto &sArchive : aManifest.archives)
		Stream << "  - " << sArchive << "\n";
	
	Stream << "Validation issues (" << aValidation.issues.size() << "):\n";
	for(const auto &Issue : aValidation.issues)
	{
		Stream << "  - [" << (Issue.severity == ValidationSeverity::Error ? "ERROR" : "WARNING") << "] " << Issue.message << "\n";
	};
	
	Stream << "Bootstrap status: " << (aValidation.HasErrors() ? "FAILED" : "OK") << "\n";
	Stream << "=======================================\n";
	
	return Stream.str();
};

}; // namespace sbe::SbGameFramework::Content
