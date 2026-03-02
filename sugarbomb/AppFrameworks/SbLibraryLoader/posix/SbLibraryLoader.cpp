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

#include <dlfcn.h>
#include <string>

#include "AppFrameworks/SbLibraryLoader/SbLibraryLoader.hpp"

//*****************************************************************************

namespace sbe
{

namespace
{
std::string BuildPlatformLibName(const char *asModuleName)
{
#if defined(__APPLE__)
	constexpr auto suffix{".dylib"};
#else
	constexpr auto suffix{".so"};
#endif
	return std::string(asModuleName) + suffix;
};
};

SbLibraryLoader::LibHandle SbLibraryLoader::Load(const char *asPath)
{
	if((asPath == nullptr) || (asPath[0] == '\0'))
		return nullptr;
	
	if(void *pHandle{dlopen(asPath, RTLD_NOW)})
		return pHandle;
	
	std::string sPlatformName{BuildPlatformLibName(asPath)};
	
	if(void *pHandle{dlopen(sPlatformName.c_str(), RTLD_NOW)})
		return pHandle;
	
	if(sPlatformName.find('/') == std::string::npos && sPlatformName.find('\\') == std::string::npos)
	{
		sPlatformName.insert(0, "./");
		
		if(void *pHandle{dlopen(sPlatformName.c_str(), RTLD_NOW)})
			return pHandle;
	};
	
	return nullptr;
};

void SbLibraryLoader::Unload(SbLibraryLoader::LibHandle anHandle)
{
	if(!anHandle)
		return;
	
	dlclose(anHandle);
};

void *SbLibraryLoader::GetSymbol(SbLibraryLoader::LibHandle anHandle, const char *asSymbol)
{
	if(!anHandle)
		return nullptr;
	
	return dlsym(anHandle, asSymbol);
};

}; // namespace sbe