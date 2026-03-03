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
#include <fstream>
#include <unordered_map>

namespace
{

using PrefsMap = std::unordered_map<std::string, std::string>;

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

PrefsMap LoadPrefs(const char *asPrefsPath)
{
	PrefsMap Result;

	if(!asPrefsPath || asPrefsPath[0] == '\0')
		return Result;

	std::ifstream PrefsFile{asPrefsPath};
	if(!PrefsFile)
		return Result;

	std::string sCurrentSection;
	std::string sLine;

	while(std::getline(PrefsFile, sLine))
	{
		auto sTrimmedLine{Trim(sLine)};
		if(sTrimmedLine.empty() || sTrimmedLine[0] == ';' || sTrimmedLine[0] == '#')
			continue;

		if(sTrimmedLine.front() == '[' && sTrimmedLine.back() == ']')
		{
			sCurrentSection = Trim(sTrimmedLine.substr(1, sTrimmedLine.size() - 2));
			continue;
		};

		auto EqualsPos{sTrimmedLine.find('=')};
		if(EqualsPos == std::string::npos)
			continue;

		auto sKey{Trim(sTrimmedLine.substr(0, EqualsPos))};
		auto sValue{Trim(sTrimmedLine.substr(EqualsPos + 1))};

		if(sKey.empty())
			continue;

		if(!sCurrentSection.empty())
			sKey = sCurrentSection + ":" + sKey;

		Result[sKey] = sValue;
	};

	return Result;
};

const std::string &GetPrefsValue(const PrefsMap &aPrefs, const std::string &sKey, const std::string &sDefault)
{
	if(auto It{aPrefs.find(sKey)}; It != aPrefs.end())
		return It->second;

	return sDefault;
};

bool ParsePrefsBool(const PrefsMap &aPrefs, const std::string &sKey, bool bDefault)
{
	static const std::string EmptyString;
	const auto &sValue{GetPrefsValue(aPrefs, sKey, EmptyString)};
	if(sValue.empty())
		return bDefault;

	std::string sLower;
	sLower.reserve(sValue.size());
	for(const auto cSymbol : sValue)
		sLower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(cSymbol))));

	if(sLower == "1" || sLower == "true" || sLower == "yes" || sLower == "on")
		return true;

	if(sLower == "0" || sLower == "false" || sLower == "no" || sLower == "off")
		return false;

	return bDefault;
};

}; // namespace

namespace f3goaty
{

OpenF3BootstrapConfig OpenF3BootstrapConfig::FromCommandLineAndPrefs(int argc, char **argv, const char *asPrefsPath)
{
	OpenF3BootstrapConfig Config;
	
	if(asPrefsPath && asPrefsPath[0] != '\0')
	{
		static const std::string EmptyString;
		const auto Prefs{LoadPrefs(asPrefsPath)};

		AddUniqueStrings(Config.installRoots, SplitList(GetPrefsValue(Prefs, "OpenF3:sInstallRoots", EmptyString)));
		AddUniqueStrings(Config.dataRoots, SplitList(GetPrefsValue(Prefs, "OpenF3:sDataRoots", EmptyString)));
		AddUniqueStrings(Config.plugins, SplitList(GetPrefsValue(Prefs, "OpenF3:sPlugins", EmptyString)));
		AddUniqueStrings(Config.archives, SplitList(GetPrefsValue(Prefs, "OpenF3:sArchives", EmptyString)));
		AddUniqueStrings(Config.dataRoots, SplitList(GetPrefsValue(Prefs, "General:SLocalMasterPath", EmptyString)));

		Config.bootstrapOnly = ParsePrefsBool(Prefs, "OpenF3:bBootstrapOnly", false);
	};
	
	for(int i{1}; i < argc; ++i)
	{
		std::string sArgValue;
		
		if(ConsumePrefixedArg(argv[i], "--openf3-data-root=", sArgValue))
		{
			AddUniqueStrings(Config.dataRoots, SplitList(sArgValue));
			continue;
		};
		
		if(ConsumePrefixedArg(argv[i], "--openf3-install-root=", sArgValue))
		{
			AddUniqueStrings(Config.installRoots, SplitList(sArgValue));
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
	SetEnvironmentValue("OPENF3_BOOTSTRAP_INSTALL_ROOTS", JoinList(installRoots));
	SetEnvironmentValue("OPENF3_BOOTSTRAP_DATA_ROOTS", JoinList(dataRoots));
	SetEnvironmentValue("OPENF3_BOOTSTRAP_PLUGINS", JoinList(plugins));
	SetEnvironmentValue("OPENF3_BOOTSTRAP_ARCHIVES", JoinList(archives));
	SetEnvironmentValue("OPENF3_BOOTSTRAP_ONLY", bootstrapOnly ? "1" : "0");
};

}; // namespace f3goaty
