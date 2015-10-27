/*
GP_Terrain_Editor - GamePlay3D Unoffical Third Party Terrain Editor
Copyright (C) 2015 Anthony Belisle <xt.hydra@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "main.h"

std::vector<std::vector<char*> > FilesLoader::loadObjectsPos(const char* folder)
{
	return std::vector<std::vector<char*> >(0);
}

std::vector<std::vector<HeightField*> > FilesLoader::loadRAWHeightmaps(const char* folder)
{
	std::vector<std::vector<char*> > fileNames;
	int arraySize, x = 0, z=0;

	bool arraySizeFound = false;

	//find the tiles resolution
	while (!arraySizeFound)
	{
		std::string filename;
		filename += "res/tmp/";
		filename += folder;
		filename += "/field-";
		filename += std::to_string(x);
		filename += "-";
		filename += std::to_string(z);
		filename += ".raw";

		std::ifstream file;
		file.open(filename, std::ifstream::binary);

		file.seekg(0, std::ios::end);
		int size=file.tellg();

		if (size > 0)
		{
			arraySize = x;
			x++;
		}
		else if (size == -1)
		{
			arraySize = x;
			arraySizeFound = true;
		}
	}

	//stack the files names in vectors
	fileNames.resize(arraySize);
	for (size_t i = 0; i<arraySize;)
	{
		fileNames[i].resize(arraySize);
		for (size_t j = 0; j<arraySize;)
		{
			std::string filename;
			filename += "field-";
			filename += std::to_string(i);
			filename += "-";
			filename += std::to_string(j);
			filename += ".raw";


			char* cstr = new char[filename.length() + 1];
			strcpy(cstr, filename.c_str());

			fileNames[i][j] = cstr;
			j++;
		}
		i++;
	}
	tilesResolution = arraySize;

	int heightfieldBuffer;

	//read each files and output data in vectors
	std::vector<std::vector<std::vector<__int16>>> raw;
	raw.resize(fileNames.size());
	for (size_t i = 0; i < fileNames.size(); i++)
	{
		raw[i].resize(fileNames[i].size());
		for (size_t j = 0; j < fileNames[i].size(); j++)
		{
			std::string filename;
			filename += "res/tmp/";
			filename += folder;
			filename += "/";
			filename += fileNames[i][j];

			std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
			heightfieldBuffer=in.tellg();

			if (heightfieldBuffer != -1)
			{
				raw[i][j].resize(heightfieldBuffer/sizeof(__int16));

				int fd = open(filename.c_str(), O_RDONLY | O_RAW);

				read(fd, raw[i][j].data(), heightfieldBuffer);
			}
		}
	}

	std::vector<std::vector<gameplay::HeightField*> > heightfields;

	int heighfieldResolution = sqrt(heightfieldBuffer / 2);

	//create empty heightmaps
	heightfields.resize(tilesResolution);
	for (size_t i = 0; i < tilesResolution; i++)
	{
		heightfields[i].resize(tilesResolution);
		for (size_t j = 0; j < tilesResolution; j++)
		{
			heightfields[i][j] = HeightField::create(heighfieldResolution, heighfieldResolution);
		}
	}

	//parse every files and output heightfields
	for (size_t i = 0; i < raw.size(); i++)
	{
		for (size_t j = 0; j < raw[i].size(); j++)
		{
			float * verts=heightfields[i][j]->getArray();

			for (size_t g = 0; g < raw[i][j].size(); g++)
			{
				verts[g] = raw[i][j][g];
			}
		}
	}
	return heightfields;
}
std::vector<std::vector<char*> > FilesLoader::loadNormalmaps(const char* folder)
{
	std::vector<std::vector<char*> > fileNames;
	int arraySize, x = 0, z = 0;

	bool arraySizeFound = false;

	while (!arraySizeFound)
	{
		std::string filename;
		filename += "res/tmp/";
		filename += folder;
		filename += "/normalMap-";
		filename += std::to_string(x);
		filename += "-";
		filename += std::to_string(z);
		filename += ".raw";

		std::ifstream file;
		file.open(filename, std::ifstream::binary);

		file.seekg(0, std::ios::end);
		int size = file.tellg();

		if (size > 0)
		{
			arraySize = x;
			x++;
		}
		else if (size == -1)
		{
			arraySize = x;
			arraySizeFound = true;
		}
	}

	fileNames.resize(arraySize);
	for (size_t i = 0; i<arraySize;)
	{
		fileNames[i].resize(arraySize);
		for (size_t j = 0; j<arraySize;)
		{
			std::string filename;
			filename += "normalMap-";
			filename += std::to_string(i);
			filename += "-";
			filename += std::to_string(j);
			filename += ".raw";


			char* cstr = new char[filename.length() + 1];
			strcpy(cstr, filename.c_str());

			fileNames[i][j] = cstr;
			j++;
		}
		i++;
	}
	tilesResolution = arraySize;
	return fileNames;
}
std::vector<std::vector<std::vector<char*> > > FilesLoader::loadBlendmaps(const char* folder)
{
	std::vector<std::vector<std::vector<char*> > > fileNames;
	int arraySize, x = 0, z = 0;

	bool arraySizeFound = false;

	while (!arraySizeFound)
	{
		std::string filename;
		filename += "res/tmp/";
		filename += folder;
		filename += "/blend-";
		filename += std::to_string(x);
		filename += "-";
		filename += std::to_string(z);
		filename += "_";
		filename += std::to_string(1);
		filename += ".png";

		std::ifstream file;
		file.open(filename, std::ifstream::binary);

		file.seekg(0, std::ios::end);
		int size = file.tellg();

		if (size > 0)
		{
			arraySize = x;
			x++;
		}
		else if (size == -1)
		{
			arraySize = x;
			arraySizeFound = true;
		}
	}

	fileNames.resize(arraySize);
	for (size_t i = 0; i<arraySize;)
	{
		fileNames[i].resize(arraySize);
		for (size_t j = 0; j<arraySize;)
		{
			std::string filename;
			filename += "blend-";
			filename += std::to_string(i);
			filename += "-";
			filename += std::to_string(j);
			filename += "_";
			filename += std::to_string(1);
			filename += ".png";

			std::string filename2;
			filename2 += "blend-";
			filename2 += std::to_string(i);
			filename2 += "-";
			filename2 += std::to_string(j);
			filename2 += "_";
			filename2 += std::to_string(2);
			filename2 += ".png";


			char* cstr = new char[filename.length() + 1];
			strcpy(cstr, filename.c_str());

			char* cstr2 = new char[filename2.length() + 1];
			strcpy(cstr2, filename2.c_str());

			fileNames[i][j].push_back(cstr);
			fileNames[i][j].push_back(cstr2);
			j++;
		}
		i++;
	}
	tilesResolution = arraySize;
	return fileNames;
}