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

#pragma once

#include <string>
#include <vector>

namespace f3goaty
{

struct OpenF3BootstrapConfig
{
	std::vector<std::string> installRoots;
	std::vector<std::string> dataRoots;
	std::vector<std::string> plugins;
	std::vector<std::string> archives;
	bool bootstrapOnly{false};
	
	static OpenF3BootstrapConfig FromCommandLineAndPrefs(int argc, char **argv, const char *asPrefsPath);
	
	void ExportToEnvironment() const;
};

}; // namespace f3goaty
