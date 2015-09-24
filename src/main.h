/*
	GP_Terrain_Editor - GamePlay3D Unoffical Third Party Terrain Editor
	Copyright (C) 2014 Anthony Belisle <xt.hydra@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//xcopy ..\GamePlay\res\shaders res\shaders\* / s / y / d
//xcopy ..\GamePlay\bin\windows\OpenAL.dll $(Configuration) / s / y / d

//if you see error C2504 you need to put the files with error in it underneath the class undefined
#pragma once
#include <fstream>
#include <string>
#ifdef WIN32
#define _USE_MATH_DEFINES
#include <time.h>
#include <cstdlib>
#include <io.h>
#include <direct.h>
#endif
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <future>
#include <thread>
#include <chrono>
#include <iostream>	
#include <fcntl.h>

#include "Terrain.h"
#include "SelectionRing.h"
#include "LodePNG.h"
#include "FirstPersonCamera.h"
#include "INoiseAlgorithm.h"
#include "DiamondSquareNoise.h"
#include "SimplexNoise.h"
#include "TerrainToolAutoBindingResolver.h"
#include "TerrainGenerator.h"
#include "FilesLoader.h"
#include "FilesSaver.h"
#include "TerrainPager.h"
#include "TerrainEditor.h"
#include "TerrainToolMain.h"
