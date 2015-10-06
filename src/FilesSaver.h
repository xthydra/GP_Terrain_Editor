/*
	GP_Terrain_Editor - GamePlay3D Unoffical Third Party Terrain Editor
	Copyright (C) 2015 Anthony Belisle <xt.hydra@gmail.com>

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

#include "main.h"

struct FilesSaver
{
	/*
	Save objects positions in a binary format(or someshit)
	@param objects positions in 3 vectors (z,x, positions)
	@param object name
	*/
	void saveObjectsPos(std::vector<std::vector<std::vector<Vector3*> > >,char*);

	/*
	Output normal maps as PNG images
	@param normalmap in 3 vectors (z,x, pixels as unsigned char)
	@param folder to output encoded PNG files
	@param normal map Resolution
	*/
	void saveNormalmaps(std::vector<std::vector<std::vector<unsigned char> > >, char *, int);

	/*
	Output heightmaps as PNG images(it's suposed to be encoding in binary tho lol)
	@param heightfields in 3 vectors (z,x, pixels as unsigned char*)
	@param heightmap Resolution
	*/
	void saveRAWHeightmaps(std::vector<std::vector<std::vector<unsigned char*> > >, int);

	/*
	Output heightmaps as PNG images
	@param heightfields in 3 vectors (z,x, pixels as unsigned char*)
	@param heightmap Resolution
	*/
	void saveHeightmaps(std::vector<std::vector<std::vector<unsigned char*> > >, int);

	/*
	Output blend maps as PNG images
	@param normalmap in 3 vectors (z,x, pixels as unsigned char)
	@param folder to output encoded PNG files
	@param normal map Resolution
	*/
	void saveBlendmaps(std::vector<std::vector<std::vector<std::vector<unsigned char> > > >,char*,int);

	/*
	Output one normal map as a PNG image
	@param normalmap in 1 vector (pixels as unsigned char)
	@param folder to output the encoded PNG file
	@param Z position in the vector of heightfields list
	@param X position in the vector of heightfields list
	@param normal map Resolution
	*/
	void saveNormalmap(std::vector<unsigned char>,char*,int,int,int);

	/*
	Output one blend map as a PNG image
	@param blend in 1 vector (pixels as unsigned char)
	@param folder to output the encoded PNG file
	@param Z position in the vector of heightfields list
	@param X position in the vector of heightfields list
	@param blend map Resolution
	*/
	void saveBlendmap(std::vector<unsigned char>,std::vector<unsigned char>,char*,int,int,int);

	/*
	Output one terrain objects position as a binary file(it's not active tho lol)
	@param objects positions in 1 vector (as gameplay::Vector3*)
	@param folder to output the encoded PNG file
	@param Z position in the vector of heightfields list
	@param X position in the vector of heightfields list
	@param normal map Resolution
	*/
	void saveObjectsPos2(std::vector<Vector3*>, char*,int,int);

	/*used to save the files name for the pager in case it would need to re-create a terrain*/
	std::vector<std::vector<std::vector<char*> > > blendFilesname;

	/*used to save the files name for the pager in case it would need to re-create a terrain*/
	std::vector<std::vector<char*> > heightFilesname;
};