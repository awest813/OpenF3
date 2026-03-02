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

//*****************************************************************************

#include <windows.h>
#include <string>

#include "AppFrameworks/SbLibraryLoader/SbLibraryLoader.hpp"

//*****************************************************************************

namespace sbe
{

namespace
{
std::string BuildPlatformLibName(const char *asModuleName)
{
	constexpr auto suffix{".dll"};
	return std::string(asModuleName) + suffix;
};
};

SbLibraryLoader::LibHandle SbLibraryLoader::Load(const char *asPath)
{
	if((asPath == nullptr) || (asPath[0] == '\0'))
		return nullptr;
	
	if(HMODULE pHandle{LoadLibraryA(asPath)})
		return reinterpret_cast<SbLibraryLoader::LibHandle>(pHandle);
	
	std::string sPlatformName{BuildPlatformLibName(asPath)};
	
	if(HMODULE pHandle{LoadLibraryA(sPlatformName.c_str())})
		return reinterpret_cast<SbLibraryLoader::LibHandle>(pHandle);
	
	return nullptr;
};

void SbLibraryLoader::Unload(SbLibraryLoader::LibHandle anHandle)
{
	if(!anHandle)
		return;
	
	FreeLibrary(reinterpret_cast<HMODULE>(anHandle));
};

void *SbLibraryLoader::GetSymbol(SbLibraryLoader::LibHandle anHandle, const char *asSymbol)
{
	if(!anHandle)
		return nullptr;
	
	return reinterpret_cast<void *>(GetProcAddress(reinterpret_cast<HMODULE>(anHandle), asSymbol));
};

}; // namespace sbe