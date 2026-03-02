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

namespace
{

std::string EscapeJson(const std::string &sValue)
{
	std::string sEscaped;
	sEscaped.reserve(sValue.size());
	
	for(const char cSymbol : sValue)
	{
		switch(cSymbol)
		{
			case '\\': sEscaped += "\\\\"; break;
			case '"': sEscaped += "\\\""; break;
			case '\n': sEscaped += "\\n"; break;
			case '\r': sEscaped += "\\r"; break;
			case '\t': sEscaped += "\\t"; break;
			default: sEscaped.push_back(cSymbol); break;
		};
	};
	
	return sEscaped;
};

}; // namespace

namespace sbe::SbGameFramework::Content
{

std::string OpenF3BootstrapReport::RenderHuman(const OpenF3ContentManifest &aManifest, const OpenF3ContentValidationResult &aValidation)
{
	std::ostringstream Stream;
	Stream << "=== OpenF3 Content Bootstrap Report ===\n";
	Stream << "Bootstrap-only mode: " << (aManifest.bootstrapOnly ? "enabled" : "disabled") << "\n";
	Stream << "Install roots (" << aManifest.installRoots.size() << "):\n";
	
	for(const auto &sInstallRoot : aManifest.installRoots)
		Stream << "  - " << sInstallRoot << "\n";
	
	Stream << "Data roots (" << aManifest.dataRoots.size() << "):\n";
	
	for(const auto &sDataRoot : aManifest.dataRoots)
		Stream << "  - " << sDataRoot << "\n";
	
	Stream << "Plugins (" << aManifest.plugins.size() << "):\n";
	for(const auto &Plugin : aManifest.plugins)
	{
		Stream << "  - " << Plugin.name;
		
		if(!Plugin.requiredMasters.empty())
		{
			Stream << " [masters:";
			
			for(size_t i{0}; i < Plugin.requiredMasters.size(); ++i)
			{
				if(i > 0)
					Stream << ",";
				Stream << Plugin.requiredMasters[i];
			};
			
			Stream << "]";
		};
		
		Stream << "\n";
	};
	
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

std::string OpenF3BootstrapReport::RenderJson(const OpenF3ContentManifest &aManifest, const OpenF3ContentValidationResult &aValidation)
{
	std::ostringstream Stream;
	Stream << "{";
	Stream << "\"bootstrapOnly\":" << (aManifest.bootstrapOnly ? "true" : "false") << ",";
	Stream << "\"status\":\"" << (aValidation.HasErrors() ? "FAILED" : "OK") << "\",";
	
	Stream << "\"installRoots\":[";
	for(size_t i{0}; i < aManifest.installRoots.size(); ++i)
	{
		if(i > 0)
			Stream << ",";
		Stream << "\"" << EscapeJson(aManifest.installRoots[i]) << "\"";
	};
	Stream << "],";
	
	Stream << "\"dataRoots\":[";
	for(size_t i{0}; i < aManifest.dataRoots.size(); ++i)
	{
		if(i > 0)
			Stream << ",";
		Stream << "\"" << EscapeJson(aManifest.dataRoots[i]) << "\"";
	};
	Stream << "],";
	
	Stream << "\"plugins\":[";
	for(size_t i{0}; i < aManifest.plugins.size(); ++i)
	{
		if(i > 0)
			Stream << ",";
		
		Stream << "{";
		Stream << "\"name\":\"" << EscapeJson(aManifest.plugins[i].name) << "\",";
		Stream << "\"requiredMasters\":[";
		
		for(size_t m{0}; m < aManifest.plugins[i].requiredMasters.size(); ++m)
		{
			if(m > 0)
				Stream << ",";
			
			Stream << "\"" << EscapeJson(aManifest.plugins[i].requiredMasters[m]) << "\"";
		};
		
		Stream << "]";
		Stream << "}";
	};
	Stream << "],";
	
	Stream << "\"archives\":[";
	for(size_t i{0}; i < aManifest.archives.size(); ++i)
	{
		if(i > 0)
			Stream << ",";
		Stream << "\"" << EscapeJson(aManifest.archives[i]) << "\"";
	};
	Stream << "],";
	
	Stream << "\"issues\":[";
	for(size_t i{0}; i < aValidation.issues.size(); ++i)
	{
		if(i > 0)
			Stream << ",";
		
		Stream << "{";
		Stream << "\"severity\":\"" << (aValidation.issues[i].severity == ValidationSeverity::Error ? "ERROR" : "WARNING") << "\",";
		Stream << "\"message\":\"" << EscapeJson(aValidation.issues[i].message) << "\"";
		Stream << "}";
	};
	Stream << "]";
	
	Stream << "}";
	return Stream.str();
};

}; // namespace sbe::SbGameFramework::Content
