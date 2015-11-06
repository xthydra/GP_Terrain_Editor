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

void FilesSaver::saveObjectsPos(std::vector<std::vector<std::vector<Vector3*> > > objsPos, char * objectName)
{
#ifdef WIN32
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mktemp(tmpdir);
	_mkdir("res/tmp");
	_mkdir(tmpdir);
#else
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mkdtemp(tmpdir);
#endif
	for (size_t i = 0; i < objsPos.size();)
	{
		for (size_t j = 0; j < objsPos[i].size();)
		{
			std::vector<__int16> raw;

			size_t x, z, k;

			for (size_t g = 0; g < objsPos[i][j].size();)
			{
				raw.push_back(objsPos[i][j][g]->x);
				raw.push_back(objsPos[i][j][g]->y);
				raw.push_back(objsPos[i][j][g]->z);
				g++;
			}

			std::string fileName;
			fileName += tmpdir;
			fileName += "/";
			fileName += std::to_string(i);
			fileName += "-";
			fileName += std::to_string(j);
			fileName += "-";
			fileName += objectName;

			int fd = open(fileName.c_str(), O_CREAT | O_WRONLY | O_RAW);
			if (fd != -1)
			{
				write(fd, raw.data(), raw.size()*sizeof(__int16));
				close(fd);
			}
			j++;
		}
		i++;
	}
}

void FilesSaver::saveNormalmaps(std::vector<std::vector<std::vector<unsigned char> > > normalmaps, char * folder, int normalMapResolution)
{
	for (size_t i = 0; i < normalmaps.size(); i++)
	{
		for (size_t j = 0; j < normalmaps[i].size(); j++)
		{
			//E.G //field-0-0.png
			std::string fieldName;

			fieldName += folder;
			fieldName += "/";
			fieldName += "normalMap-";
			fieldName += std::to_string(i);
			fieldName += "-";
			fieldName += std::to_string(j);
			fieldName += ".png";

			//std::vector<unsigned char> mapData(reinterpret_cast<unsigned char>(normalmaps[i][j].data()));

			// Generate the png.
			lodepng::encode(fieldName.c_str(), normalmaps[i][j], normalMapResolution, normalMapResolution);
		}
	}
}

void FilesSaver::saveRAWHeightmaps(std::vector<std::vector<gameplay::HeightField *> > heightFields)
{
	// Generate a new tmp folder for the blend images.
#ifdef WIN32
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mktemp(tmpdir);
	_mkdir("res/tmp");
	_mkdir(tmpdir);
#else
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mkdtemp(tmpdir);
#endif

	//TODO: if i want to be able to support 32 bit integer instead of 16 bit then
	//i would need a conditional iteration like : while(i<heightfields.size() && bit16==true)
	//check against every height if it doesnt go beyond or below 32000 else bit16==false
	//and change the file extension to someshit like ".r32"
	for (size_t i = 0; i < heightFields.size(); i++)
	{
		for (size_t j = 0; j < heightFields[i].size(); j++)
		{
			std::vector<__int16> raw;

			size_t x, z, k;

			int heightfieldSize = heightFields[0][0]->getColumnCount();

			raw.resize((heightfieldSize * heightfieldSize));

			//get the heightfield height in a vector
			for (x = 0; x < heightfieldSize; x++)
			{
				for (z = 0; z < heightfieldSize; z++)
				{
					k = x + (z * heightfieldSize);

					// Get the unscaled height value from the HeightField
					float height = heightFields[i][j]->getHeight(x, z);

					raw[k] = height;
				}
			}

			//create the filename
			std::string fieldName;

			fieldName += tmpdir;
			fieldName += "/";
			fieldName += "field-";
			fieldName += std::to_string(i);
			fieldName += "-";
			fieldName += std::to_string(j);
			fieldName += ".r16";

			//save the file
			int fd = open(fieldName.c_str(), O_CREAT | O_WRONLY | O_RAW);
			if (fd != -1)
			{
				write(fd, raw.data(), raw.size()*sizeof(__int16));
				close(fd);
			}
		}
	}
}

void FilesSaver::saveBlendmaps(std::vector<std::vector<std::vector<std::vector<unsigned char> > > > blendmaps, char* folder, int blendmapRes)
{
	blendFilesname.resize(blendmaps.size());
	for (size_t i = 0; i < blendmaps.size(); i++)
	{
		blendFilesname[i].resize(blendmaps[i].size());
		for (size_t j = 0; j < blendmaps[i].size(); j++)
		{	
			std::string fieldName;

			fieldName += folder;
			fieldName += "blend-";
			fieldName += std::to_string(i);
			fieldName += "-";
			fieldName += std::to_string(j);
			fieldName += "_";
			fieldName += std::to_string(1);
			fieldName += ".png";

			char* cstr = new char[fieldName.length() + 1];
			strcpy(cstr, fieldName.c_str());
			blendFilesname[i][j].push_back(cstr);

			// Generate the png.
			lodepng::encode(fieldName.c_str(), blendmaps[i][j][0], blendmapRes, blendmapRes);

			fieldName.clear();

			fieldName += folder;
			fieldName += "blend-";
			fieldName += std::to_string(i);
			fieldName += "-";
			fieldName += std::to_string(j);
			fieldName += "_";
			fieldName += std::to_string(2);
			fieldName += ".png";

			cstr = new char[fieldName.length() + 1];
			strcpy(cstr, fieldName.c_str());
			blendFilesname[i][j].push_back(cstr);

			// Generate the png.
			lodepng::encode(fieldName.c_str(), blendmaps[i][j][1], blendmapRes, blendmapRes);
		}
	}
	int shit = 1 + 1;
}

//===========================================================================

void FilesSaver::saveNormalmap(std::vector<unsigned char> normalmap, char* folder, int z, int x, int normalmapRes)
{
	std::string fieldName;

	fieldName += folder;
	fieldName += "/";
	fieldName += "normalMap-";
	fieldName += std::to_string(z);
	fieldName += "-";
	fieldName += std::to_string(x);
	fieldName += ".png";

	// Generate the png.
	lodepng::encode(fieldName.c_str(), normalmap, normalmapRes, normalmapRes);
}

void FilesSaver::saveBlendmap(std::vector<unsigned char> blendmap, std::vector<unsigned char> blendmap2, char* folder, int z, int x, int blendmapRes)
{
	std::string fieldName;

	fieldName += folder;
	fieldName += "blend-";
	fieldName += std::to_string(z);
	fieldName += "-";
	fieldName += std::to_string(x);
	fieldName += "_";
	fieldName += std::to_string(1);
	fieldName += ".png";

	// Generate the png.
	lodepng::encode(fieldName.c_str(), blendmap.data(), blendmapRes, blendmapRes);

	fieldName.clear();

	fieldName += folder;
	fieldName += "blend-";
	fieldName += std::to_string(z);
	fieldName += "-";
	fieldName += std::to_string(x);
	fieldName += "_";
	fieldName += std::to_string(2);
	fieldName += ".png";

	// Generate the png.
	lodepng::encode(fieldName.c_str(), blendmap2.data(), blendmapRes, blendmapRes);
}


/*
void FilesSaver::saveObjectsPos2(std::vector<Vector3> objsPos, char * objectName, int z, int x)
{
#ifdef WIN32
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mktemp(tmpdir);
	_mkdir("res/tmp");
	_mkdir(tmpdir);
#else
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mkdtemp(tmpdir);
#endif
	std::vector<char> raw;
	for (size_t i = 0; i < objsPos.size();)
	{
		raw.push_back(objsPos[i].x);
		raw.push_back(' ');
		raw.push_back(objsPos[i].y);
		raw.push_back(' ');
		raw.push_back(objsPos[i].z);
		raw.push_back(' ');
		raw.push_back(' ');
		i++;
	}
	std::string fileName;
	fileName += tmpdir;
	fileName += "/";
	fileName += std::to_string(z);
	fileName += "-";
	fileName += std::to_string(x);
	fileName += objectName;

	std::ofstream rawField;
	rawField.open(fileName.c_str(), std::ios::binary);

	rawField.write(raw.data(), raw.size());
	rawField.close();

	//char* cstr = new char[fileName.length() + 1];
	//strcpy(cstr, fileName.c_str());

	//fileNames[i][j] = cstr;
}*/