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

#include "OpenF3ContentValidator.hpp"

#include <algorithm>
#include <cctype>

namespace
{

std::string ToLower(std::string sValue)
{
	std::transform(sValue.begin(), sValue.end(), sValue.begin(), [](const unsigned char cChar){return static_cast<char>(std::tolower(cChar));});
	return sValue;
};

bool EndsWithInsensitive(const std::string &sValue, const std::string &sSuffix)
{
	const auto sLowerValue{ToLower(sValue)};
	const auto sLowerSuffix{ToLower(sSuffix)};
	
	if(sLowerSuffix.size() > sLowerValue.size())
		return false;
	
	return sLowerValue.compare(sLowerValue.size() - sLowerSuffix.size(), sLowerSuffix.size(), sLowerSuffix) == 0;
};

}; // namespace

namespace sbe::SbGameFramework::Content
{

bool OpenF3ContentValidationResult::HasErrors() const
{
	return std::any_of(issues.begin(), issues.end(), [](const ValidationIssue &Issue){return Issue.severity == ValidationSeverity::Error;});
};

void OpenF3ContentValidationResult::AddWarning(const std::string &sMessage)
{
	issues.push_back({ValidationSeverity::Warning, sMessage});
};

void OpenF3ContentValidationResult::AddError(const std::string &sMessage)
{
	issues.push_back({ValidationSeverity::Error, sMessage});
};

OpenF3ContentValidationResult OpenF3ContentValidator::Validate(const OpenF3ContentManifest &aManifest) const
{
	OpenF3ContentValidationResult Result;
	
	if(aManifest.dataRoots.empty())
		Result.AddWarning("No data roots specified for content bootstrap.");
	
	if(aManifest.plugins.empty())
		Result.AddWarning("No plugins specified for content bootstrap.");
	
	bool bHasMasterPlugin{false};
	bool bHasRegularPlugin{false};
	
	for(const auto &sPlugin : aManifest.plugins)
	{
		const bool bIsMaster{EndsWithInsensitive(sPlugin, ".esm")};
		const bool bIsPlugin{EndsWithInsensitive(sPlugin, ".esp")};
		const bool bIsLightPlugin{EndsWithInsensitive(sPlugin, ".esl")};
		
		bHasMasterPlugin = bHasMasterPlugin || bIsMaster;
		bHasRegularPlugin = bHasRegularPlugin || bIsPlugin;
		
		if(!(bIsMaster || bIsPlugin || bIsLightPlugin))
			Result.AddError("Unsupported plugin extension: \"" + sPlugin + "\".");
	};
	
	if(bHasRegularPlugin && !bHasMasterPlugin)
		Result.AddError("At least one .esp plugin was provided but no .esm master plugin was found.");
	
	for(const auto &sArchive : aManifest.archives)
	{
		if(!EndsWithInsensitive(sArchive, ".bsa"))
			Result.AddWarning("Archive entry does not end with .bsa: \"" + sArchive + "\".");
	};
	
	return Result;
};

}; // namespace sbe::SbGameFramework::Content
