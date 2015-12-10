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

std::vector<std::vector<std::vector<Vector3*> > > FilesLoader::loadObjectsPos(const char* folder)
{
	std::vector<std::vector<char*> > fileNames;
	int x = 0, z = 0;

	bool arraySizeFound = false;

	intptr_t handle;

	std::string defaultDir, workingDir;

	//get current directory on Unix
	//getcwd()
	//set current directory on Unix
	//chdir()
	//i don't think there's a multiplatform command to find files tho

	//change the current working directory to allow precise folder search
	const int bufferSize = MAX_PATH;
	char oldDir[bufferSize]; // store the current directory

	// get the current directory, and store it
	if (!GetCurrentDirectoryA(bufferSize, oldDir)) 
	{
		std::cerr << "Error getting current directory: #" << GetLastError();
	}

	workingDir += "res/tmp/";
	workingDir += folder;

	SetCurrentDirectoryA(workingDir.c_str());

	__finddata64_t info;

	//check all the files in the current working directory that have the ".gpb" extension
	std::string fileName;
	fileName += "*.gpb";

	std::string lastFile;


	handle = _findfirst64(fileName.c_str(), &info);
	if (handle == -1)
	{
		printf("No files with .gpb extension were found inside %s", workingDir.c_str());
	}
	while(handle != -1 && info.name != lastFile)
	{
		//get the file extension
		std::string foundFileName = info.name;
		std::string fileEXT;
		fileEXT += foundFileName[foundFileName.size() - 4];
		fileEXT += foundFileName[foundFileName.size() - 3];
		fileEXT += foundFileName[foundFileName.size() - 2];
		fileEXT += foundFileName[foundFileName.size() - 1];

		//check against the file extension
		if (fileEXT[0] == '.' &&
			fileEXT[1] == 'g' &&
			fileEXT[2] == 'p' &&
			fileEXT[3] == 'b')
		{
			//check the tiles resolution
			std::string vector1, vector2;
			vector1 += foundFileName[0];
			vector2 += foundFileName[2];
			int iVector1 = atoi(vector1.c_str());
			int iVector2 = atoi(vector2.c_str());
			if (fileNames.size() < iVector1+1)
			{
				fileNames.resize(iVector1+1);
			}
			if (fileNames[iVector1].size() < iVector2+1)
			{
				fileNames[iVector1].resize(iVector2+1);
			}
		}
		lastFile.clear();
		lastFile += info.name;
		_findnext64(handle, &info);
		//make sure it's the last file in the folder
		if (lastFile == info.name)
		{
			lastFile.clear();
			lastFile += info.name;
		}
	}

	//get the object file name
	std::string foundFileName = info.name;
	fileName.clear();
	for (size_t i = 0; i < foundFileName.length(); i++)
	{
		//prevent from looking at the part that's defining the objects location
		if (i > 3)
		{
			//if it's a dot then it's the end of the object file name
			if (foundFileName[i] != '.')
			{
				fileName += foundFileName[i];
			}
			else
			{
				i = foundFileName.length();
				continue;
			}
		}
	}

	//close the searching tool
	_findclose(handle);
	//set back the old working directory
	SetCurrentDirectoryA(oldDir);

	tilesResolution = fileNames.size();

	int heightfieldBuffer;

	//read each files and output data in vectors
	std::vector<std::vector<std::vector<__int16>>> raw;
	raw.resize(fileNames.size());
	for (size_t i = 0; i < fileNames.size(); i++)
	{
		raw[i].resize(fileNames[i].size());
		for (size_t j = 0; j < fileNames[i].size(); j++)
		{

			std::string file;
			file += "res/tmp/";
			file += folder;
			file += "/";
			file += std::to_string(i);
			file += "-";
			file += std::to_string(j);
			file += "-";
			file += fileName;
			file += ".gpb";

			std::ifstream in(file, std::ifstream::ate | std::ifstream::binary);
			heightfieldBuffer = in.tellg();

			if (heightfieldBuffer != -1)
			{
				raw[i][j].resize(heightfieldBuffer / sizeof(__int16));

				int fd = open(file.c_str(), O_RDONLY | O_RAW);

				read(fd, raw[i][j].data(), heightfieldBuffer);
			}
		}
	}

	std::vector<std::vector<std::vector<Vector3*> > > objectsPosition;

	//parse every files and output object positions
	objectsPosition.resize(raw.size());
	for (size_t i = 0; i < raw.size(); i++)
	{
		objectsPosition[i].resize(raw[i].size());
		for (size_t j = 0; j < raw[i].size(); j++)
		{
			for (size_t g = 0; g < raw[i][j].size();)
			{
				Vector3 * objPos=new Vector3;
				objPos->x = raw[i][j][g];
				g++;
				objPos->y = raw[i][j][g];
				g++;
				objPos->z = raw[i][j][g];
				g++;

				objectsPosition[i][j].push_back(objPos);
			}
		}
	}
	
	return objectsPosition;
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