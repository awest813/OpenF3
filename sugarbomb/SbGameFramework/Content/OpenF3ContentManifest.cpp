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

using sbe::SbGameFramework::Content::PluginEntry;

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

PluginEntry ParsePluginEntry(const std::string &sRawEntry)
{
	PluginEntry Entry;
	
	const auto nSeparatorPos{sRawEntry.find('|')};
	if(nSeparatorPos == std::string::npos)
	{
		Entry.name = Trim(sRawEntry);
		return Entry;
	};
	
	Entry.name = Trim(sRawEntry.substr(0, nSeparatorPos));
	const auto sMastersPart{Trim(sRawEntry.substr(nSeparatorPos + 1))};
	
	std::string sCurrentMaster;
	for(const char cSymbol : sMastersPart)
	{
		if(cSymbol == '+' || cSymbol == ',')
		{
			auto sMaster{Trim(sCurrentMaster)};
			if(!sMaster.empty())
				Entry.requiredMasters.emplace_back(std::move(sMaster));
			
			sCurrentMaster.clear();
			continue;
		};
		
		sCurrentMaster.push_back(cSymbol);
	};
	
	auto sTailMaster{Trim(sCurrentMaster)};
	if(!sTailMaster.empty())
		Entry.requiredMasters.emplace_back(std::move(sTailMaster));
	
	Canonicalize(Entry.requiredMasters);
	return Entry;
};

std::vector<PluginEntry> ParsePluginList(const std::vector<std::string> &aRawEntries)
{
	std::vector<PluginEntry> Plugins;
	Plugins.reserve(aRawEntries.size());
	
	for(const auto &sRawEntry : aRawEntries)
	{
		auto Entry{ParsePluginEntry(sRawEntry)};
		if(!Entry.name.empty())
			Plugins.emplace_back(std::move(Entry));
	};
	
	return Plugins;
};

void CanonicalizePlugins(std::vector<PluginEntry> &aPlugins)
{
	std::sort(aPlugins.begin(), aPlugins.end(), [](const PluginEntry &Lhs, const PluginEntry &Rhs)
	{
		return ToLower(Lhs.name) < ToLower(Rhs.name);
	});
	
	std::vector<PluginEntry> Deduplicated;
	Deduplicated.reserve(aPlugins.size());
	
	for(auto &Plugin : aPlugins)
	{
		if(Deduplicated.empty() || ToLower(Deduplicated.back().name) != ToLower(Plugin.name))
		{
			Canonicalize(Plugin.requiredMasters);
			Deduplicated.emplace_back(std::move(Plugin));
			continue;
		};
		
		auto &MergedMasters{Deduplicated.back().requiredMasters};
		MergedMasters.insert(MergedMasters.end(), Plugin.requiredMasters.begin(), Plugin.requiredMasters.end());
		Canonicalize(MergedMasters);
	};
	
	aPlugins = std::move(Deduplicated);
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
	
	Manifest.installRoots = ReadEnvList("OPENF3_BOOTSTRAP_INSTALL_ROOTS");
	Manifest.dataRoots = ReadEnvList("OPENF3_BOOTSTRAP_DATA_ROOTS");
	Manifest.plugins = ParsePluginList(ReadEnvList("OPENF3_BOOTSTRAP_PLUGINS"));
	Manifest.archives = ReadEnvList("OPENF3_BOOTSTRAP_ARCHIVES");
	Manifest.bootstrapOnly = ReadEnvBool("OPENF3_BOOTSTRAP_ONLY");
	
	Canonicalize(Manifest.installRoots);
	Canonicalize(Manifest.dataRoots);
	CanonicalizePlugins(Manifest.plugins);
	Canonicalize(Manifest.archives);
	
	return Manifest;
};

}; // namespace sbe::SbGameFramework::Content
