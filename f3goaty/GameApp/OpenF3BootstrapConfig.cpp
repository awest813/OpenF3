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

#include "OpenF3BootstrapConfig.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>

#include "iniparser.h"

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
		if(cSymbol == ',' || cSymbol == ';')
		{
			auto sTrimmedToken{Trim(sCurrentToken)};
			if(!sTrimmedToken.empty())
				Result.emplace_back(std::move(sTrimmedToken));
			
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

void AddUniqueStrings(std::vector<std::string> &aTarget, const std::vector<std::string> &aValues)
{
	for(const auto &sValue : aValues)
	{
		if(std::find(aTarget.begin(), aTarget.end(), sValue) == aTarget.end())
			aTarget.emplace_back(sValue);
	};
};

bool ConsumePrefixedArg(const char *asArgument, const char *asPrefix, std::string &sOutValue)
{
	if(!asArgument || !asPrefix)
		return false;
	
	const std::string sArgument{asArgument};
	const std::string sPrefix{asPrefix};
	
	if(sArgument.rfind(sPrefix, 0) != 0)
		return false;
	
	sOutValue = Trim(sArgument.substr(sPrefix.size()));
	return !sOutValue.empty();
};

std::string JoinList(const std::vector<std::string> &aValues)
{
	std::string sResult;
	
	for(size_t i{0}; i < aValues.size(); ++i)
	{
		if(i > 0)
			sResult.push_back(';');
		
		sResult.append(aValues[i]);
	};
	
	return sResult;
};

void SetEnvironmentValue(const char *asName, const std::string &sValue)
{
#ifdef _WIN32
	_putenv_s(asName, sValue.c_str());
#else
	setenv(asName, sValue.c_str(), 1);
#endif
};

}; // namespace

namespace f3goaty
{

OpenF3BootstrapConfig OpenF3BootstrapConfig::FromCommandLineAndPrefs(int argc, char **argv, const char *asPrefsPath)
{
	OpenF3BootstrapConfig Config;
	
	if(asPrefsPath && asPrefsPath[0] != '\0')
	{
		if(dictionary *pDict{iniparser_load(asPrefsPath)})
		{
			if(const char *pDataRoots{iniparser_getstring(pDict, "OpenF3:sDataRoots", "")})
				AddUniqueStrings(Config.dataRoots, SplitList(pDataRoots));
			
			if(const char *pPlugins{iniparser_getstring(pDict, "OpenF3:sPlugins", "")})
				AddUniqueStrings(Config.plugins, SplitList(pPlugins));
			
			if(const char *pArchives{iniparser_getstring(pDict, "OpenF3:sArchives", "")})
				AddUniqueStrings(Config.archives, SplitList(pArchives));
			
			if(const char *pLegacyDataRoot{iniparser_getstring(pDict, "General:SLocalMasterPath", "")})
				AddUniqueStrings(Config.dataRoots, SplitList(pLegacyDataRoot));
			
			Config.bootstrapOnly = iniparser_getboolean(pDict, "OpenF3:bBootstrapOnly", false);
			iniparser_freedict(pDict);
		};
	};
	
	for(int i{1}; i < argc; ++i)
	{
		std::string sArgValue;
		
		if(ConsumePrefixedArg(argv[i], "--openf3-data-root=", sArgValue))
		{
			AddUniqueStrings(Config.dataRoots, SplitList(sArgValue));
			continue;
		};
		
		if(ConsumePrefixedArg(argv[i], "--openf3-plugin=", sArgValue))
		{
			AddUniqueStrings(Config.plugins, SplitList(sArgValue));
			continue;
		};
		
		if(ConsumePrefixedArg(argv[i], "--openf3-archive=", sArgValue))
		{
			AddUniqueStrings(Config.archives, SplitList(sArgValue));
			continue;
		};
		
		if(std::string{argv[i]} == "--openf3-bootstrap-only")
			Config.bootstrapOnly = true;
	};
	
	return Config;
};

void OpenF3BootstrapConfig::ExportToEnvironment() const
{
	SetEnvironmentValue("OPENF3_BOOTSTRAP_DATA_ROOTS", JoinList(dataRoots));
	SetEnvironmentValue("OPENF3_BOOTSTRAP_PLUGINS", JoinList(plugins));
	SetEnvironmentValue("OPENF3_BOOTSTRAP_ARCHIVES", JoinList(archives));
	SetEnvironmentValue("OPENF3_BOOTSTRAP_ONLY", bootstrapOnly ? "1" : "0");
};

}; // namespace f3goaty
