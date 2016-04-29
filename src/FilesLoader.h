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

struct FilesLoader
{
	/*unuseable*/
	std::vector<std::vector<std::vector<Vector3*> > > loadObjectsPos(const char*);

	/*
	Param folder to load the heightmaps
	@return heightfields in 2 vectors used to define the tile position
	*/
	std::vector<std::vector<HeightField*> > loadRAWHeightmaps(const char*);

	/*
	Param folder to load the normalmaps
	@return 2 vectors + an array of characters (z,x,char*)
	*/
	std::vector<std::vector<char*> > loadNormalmaps(const char*);

	/*
	Param folder to load the blendmaps
	@return 2 vectors + an array of characters (z,x,char*)
	*/
	std::vector<std::vector<std::vector<char*> > > loadBlendmaps(const char*);

	/*the resolution of the cumulative amount of terrains*/
	int tilesResolution;
};