/*
*******************************************************************************

Copyright (C) 2019 SugarBombEngine Developers

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

#include <cctype>
#include <cstdlib>
#include <fstream>
#include <string>

#include "GameApp.hpp"
#include "SbSystemExternal.hpp"
#include "SbRenderSystemExternal.hpp"
#include "SbInputSystemExternal.hpp"
#include "SbSoundSystemExternal.hpp"
#include "SbGameFrameworkExternal.hpp"
#include "OpenF3BootstrapConfig.hpp"

#ifdef _WIN32
#	include <windows.h>
#endif


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

void LoadDisplayPrefs(const char *asPath, int &anWidth, int &anHeight, bool &abFullScreen)
{
	std::ifstream PrefsFile{asPath};
	if(!PrefsFile)
		return;

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

		if(sCurrentSection != "Display")
			continue;

		auto EqualsPos{sTrimmedLine.find('=')};
		if(EqualsPos == std::string::npos)
			continue;

		auto sKey{Trim(sTrimmedLine.substr(0, EqualsPos))};
		auto sValue{Trim(sTrimmedLine.substr(EqualsPos + 1))};

		if(sKey == "iSize W")
			anWidth = std::strtol(sValue.c_str(), nullptr, 10);
		else if(sKey == "iSize H")
			anHeight = std::strtol(sValue.c_str(), nullptr, 10);
		else if(sKey == "bFull Screen")
			abFullScreen = (std::strtol(sValue.c_str(), nullptr, 10) != 0);
	};
};

}; // namespace

//sbe::ISoundSystem *CreateSoundSystem();
//sbe::IGameFramework *CreateGameFramework();

// TODO: delete mpSoundSystem; mpSoundSystem = nullptr;
// TODO: delete mpFramework; mpFramework = nullptr;

sbe::ISystem *CreateSystem()
{
#ifndef SBE_SINGLE_BINARY
	static sbe::SbSystemExternal SbSystemModule;
	return SbSystemModule.GetSystem();
#else
	return new sbe::SbSystem::SbSystem();
#endif
};

sbe::IRenderSystem *CreateRenderSystem(sbe::ISystem &aSystem)
{
#ifndef SBE_SINGLE_BINARY
	static sbe::SbRenderSystemExternal SbRenderModule(aSystem);
	return SbRenderModule.GetRenderSystem();
#else
	return new sbe::SbRenderer::SbRenderSystem();
#endif
};

sbe::IInputSystem *CreateInputSystem(sbe::ISystem &aSystem)
{
#ifndef SBE_SINGLE_BINARY
	static sbe::SbInputSystemExternal SbInputModule(aSystem);
	return SbInputModule.GetInputSystem();
#else
	return new sbe::SbInput::SbInputSystem();
#endif
};

sbe::ISoundSystem *CreateSoundSystem(sbe::ISystem &aSystem)
{
#ifndef SBE_SINGLE_BINARY
	static sbe::SbSoundSystemExternal SbSoundModule(aSystem);
	return SbSoundModule.GetSoundSystem();
#else
	return new sbe::SbSound::SbSound();
#endif
};

sbe::IGameFramework *CreateGameFramework(sbe::ISystem &aSystem, sbe::IRenderSystem *apRenderSystem, sbe::ISoundSystem *apSoundSystem)
{
#ifndef SBE_SINGLE_BINARY
	static sbe::SbGameFrameworkExternal SbGameFrameworkModule(aSystem, apRenderSystem, apSoundSystem);
	return SbGameFrameworkModule.GetGameFramework();
#else
	return new sbe::SbGameFramework::SbGameFramework();
#endif
};

int main(int argc, char **argv)
{
	sbe::ISystem *pSystem = CreateSystem();
	sbe::ISoundSystem *pSoundSystem = CreateSoundSystem(*pSystem);
	sbe::IRenderSystem *pRenderSystem = CreateRenderSystem(*pSystem);
	sbe::IInputSystem *pInputSystem = CreateInputSystem(*pSystem);
	sbe::IGameFramework *pGameFramework = CreateGameFramework(*pSystem, pRenderSystem, pSoundSystem);
	
	const char *sWindowTitle{"F3GOATY"};
	int nWindowWidth{1280};
	int nWindowHeight{600};
	bool bWindowFullScreen{false};

	LoadDisplayPrefs("FalloutPrefs.ini", nWindowWidth, nWindowHeight, bWindowFullScreen);

#ifdef OPENF3_ENABLE_PHASE1_BOOTSTRAP
	const auto BootstrapConfig{f3goaty::OpenF3BootstrapConfig::FromCommandLineAndPrefs(argc, argv, "FalloutPrefs.ini")};
	BootstrapConfig.ExportToEnvironment();
#endif
	
	f3goaty::CGameApp App(sWindowTitle, nWindowWidth, nWindowHeight, bWindowFullScreen, pGameFramework, pSoundSystem, pRenderSystem, pInputSystem, pSystem, argc, argv);
	App.Run();
	return EXIT_SUCCESS;
};

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main(__argc, __argv);
};
#endif