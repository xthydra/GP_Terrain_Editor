/*
	GP_Terrain_Editor - GamePlay3D Unoffical Third Party Terrain Editor
	Copyright (C) 2016 Anthony Belisle <xt.hydra@gmail.com>

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

struct FilesSaver
{
	/*
	Save the zone objects file path and terrain textures file path
	the first and second vectors define the world position of the terrains but i always forget which one is defining where

	the third vector for objects is defining how many different objects file paths there's
	the third vector for terrains is defining how many textures path the terrain have been assigned
	it output everything in a text file for loading an instance
	@param objects file paths in a vector of unsigned characters
	@param file paths of terrains textures in a vector of unsigned characters
	@param folder to save into
	*/
	void saveZoneInformation(std::vector<std::vector<std::vector<unsigned char> > >, std::vector<std::vector<std::vector<unsigned char> > >,char *);

	/*
	Save objects positions in a binary format(or someshit)
	@param objects positions in 3 vectors (z,x, positions)
	@param object index position inside the information file
	@param folder to save into
	*/
	void saveObjectsPos(std::vector<std::vector<std::vector<Vector3*> > >,int,char*);

	/*
	Output normal maps as PNG images
	@param normalmap in 3 vectors (z,x, pixels as unsigned char)
	@param folder to output encoded PNG files
	@param normal map Resolution
	*/
	void saveNormalmaps(std::vector<std::vector<std::vector<unsigned char> > >, char *, int);

	/*
	Output heightfields in binary files
	@param heightfields in 2 vectors (z,x)
	@param folder to save into
	*/
	void saveRAWHeightmaps(std::vector<std::vector<gameplay::HeightField *> >, char *);

	/*
	Output blend maps as PNG images
	@param normalmap in 3 vectors (z,x, pixels as unsigned char)
	@param folder to output encoded PNG files
	@param blend map Resolution
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
	@param blendmap1 in 1 vector (pixels as unsigned char)
	@param blendmap2 in 1 vector (pixels as unsigned char)
	@param folder to output the encoded PNG file
	@param Z position in the vector of heightfields list
	@param X position in the vector of heightfields list
	@param blend map Resolution
	*/
	void saveBlendmap(std::vector<unsigned char>,std::vector<unsigned char>,char*,int,int,int);

	/*
	Output one blend map as a PNG image
	@param blendmap in 1 vector (pixels as unsigned char)
	@param blendmap number(1 or 2)
	@param folder to output the encoded PNG file
	@param Z position in the vector of heightfields list
	@param X position in the vector of heightfields list
	@param blend map Resolution
	*/
	void saveBlendmap(std::vector<unsigned char>, int, char*, int, int, int);

	/*
	Output one terrain objects position as a binary file(it's not active tho lol)
	@param objects positions in 1 vector (as gameplay::Vector3*)
	@param folder to output the encoded PNG file
	@param Z position in the vector of heightfields list
	@param X position in the vector of heightfields list
	*/
	//void saveObjectsPos(std::vector<Vector3*>, char*,int,int);

	/*used to save the files name for the pager in case it would need to re-create a terrain*/
	std::vector<std::vector<std::vector<char*> > > blendFilesname;

	/*used to save the files name for the pager in case it would need to re-create a terrain*/
	std::vector<std::vector<char*> > heightFilesname;
};