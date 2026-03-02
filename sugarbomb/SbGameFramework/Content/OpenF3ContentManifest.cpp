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

#include "OpenF3ContentManifest.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace
{

std::string Trim(std::string sValue)
{
	while(!sValue.empty() && std::isspace(static_cast<unsigned char>(sValue.front())))
		sValue.erase(sValue.begin());
	
	while(!sValue.empty() && std::isspace(static_cast<unsigned char>(sValue.back())))
		sValue.pop_back();
	
	return sValue;
};

std::vector<std::string> SplitList(const std::string &sRawValue)
{
	std::vector<std::string> Result;
	std::string sCurrentToken;
	
	for(const char cSymbol : sRawValue)
	{
		if(cSymbol == ';' || cSymbol == ',')
		{
			auto sToken{Trim(sCurrentToken)};
			if(!sToken.empty())
				Result.emplace_back(std::move(sToken));
			sCurrentToken.clear();
			continue;
		};
		
		sCurrentToken.push_back(cSymbol);
	};
	
	auto sTailToken{Trim(sCurrentToken)};
	if(!sTailToken.empty())
		Result.emplace_back(std::move(sTailToken));
	
	return Result;
};

std::string ToLower(std::string sValue)
{
	std::transform(sValue.begin(), sValue.end(), sValue.begin(), [](const unsigned char cChar){return static_cast<char>(std::tolower(cChar));});
	return sValue;
};

void Canonicalize(std::vector<std::string> &aValues)
{
	std::sort(aValues.begin(), aValues.end(), [](const std::string &sLhs, const std::string &sRhs)
	{
		return ToLower(sLhs) < ToLower(sRhs);
	});
	
	aValues.erase(std::unique(aValues.begin(), aValues.end(), [](const std::string &sLhs, const std::string &sRhs)
	{
		return ToLower(sLhs) == ToLower(sRhs);
	}), aValues.end());
};

std::vector<std::string> ReadEnvList(const char *asEnvName)
{
	if(const char *pRawValue{std::getenv(asEnvName)})
		return SplitList(pRawValue);
	return {};
};

bool ReadEnvBool(const char *asEnvName)
{
	if(const char *pRawValue{std::getenv(asEnvName)})
	{
		const std::string sValue{ToLower(Trim(pRawValue))};
		return sValue == "1" || sValue == "true" || sValue == "yes" || sValue == "on";
	};
	
	return false;
};

}; // namespace

namespace sbe::SbGameFramework::Content
{

OpenF3ContentManifest OpenF3ContentManifest::FromEnvironment()
{
	OpenF3ContentManifest Manifest;
	
	Manifest.dataRoots = ReadEnvList("OPENF3_BOOTSTRAP_DATA_ROOTS");
	Manifest.plugins = ReadEnvList("OPENF3_BOOTSTRAP_PLUGINS");
	Manifest.archives = ReadEnvList("OPENF3_BOOTSTRAP_ARCHIVES");
	Manifest.bootstrapOnly = ReadEnvBool("OPENF3_BOOTSTRAP_ONLY");
	
	Canonicalize(Manifest.dataRoots);
	Canonicalize(Manifest.plugins);
	Canonicalize(Manifest.archives);
	
	return Manifest;
};

}; // namespace sbe::SbGameFramework::Content
