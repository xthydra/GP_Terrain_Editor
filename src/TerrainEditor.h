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

/**
* The structure is called and destroyed only for editing given terrains
**/
struct TerrainEditor{

	/**
	* Aligning each vertexes at given terrains extremity next to each others height to make it look smoother
	*
	* @param heightfields vectors
	* @param heightfield size
	* @param terrain XZ scale(must be squared)
	*
	* @return void
	**/
	void aligningTerrainsVertexes(std::vector<std::vector<HeightField*> >, 
								  size_t,
								  int);

	/**
	* Raise a part of the terrains vertices defined by a radius and a position
	*
	* @param (BoundingSphere)Selection ring BoundingSphere in world space
	* @param (int)terrain XZ scale(must be squared)
	* @param (int)terrain Y scale
	* @param vector containing loaded terrains
	* @param heightfields vector stack
	*
	* @return returning int pointing out to modified terrains from the vector stack
	**/
	std::vector<int> raise(BoundingSphere,
								int,
								int,
								std::vector<gameplay::Terrain*>,
								std::vector<HeightField*>);

	/**
	* Lower a part of the terrains vertices defined by a radius and a position
	*
	* @param (BoundingSphere)Selection ring BoundingSphere in world space
	* @param (int)terrain XZ scale(must be squared)
	* @param (int)terrain Y scale
	* @param vector containing loaded terrains
	* @param heightfields vector stack
	*
	* @return returning int pointing out to modified terrains from the vector stack
	**/
	std::vector<int> lower(BoundingSphere,
		int,
		int,
		std::vector<gameplay::Terrain*>,
		std::vector<HeightField*>);

	/**
	* it doesn't have a function right now
	*
	* @param (BoundingSphere)Selection ring BoundingSphere in world space
	* @param (int)terrain XZ scale(must be squared)
	* @param (int)terrain Y scale
	* @param vector containing loaded terrains
	* @param heightfields vector stack
	*
	* @return returning int pointing out to modified terrains from the vector stack
	**/
	std::vector<int> smooth(BoundingSphere,
		int,
		int,
		std::vector<gameplay::Terrain*>,
		std::vector<HeightField*>);

	/**
	* Flatten a circle of the terrain. The contents of the circle are all brought to the average - and some smoothing is done to the edges.
	*
	* @param (BoundingSphere)Selection ring BoundingSphere in world space
	* @param (int)terrain XZ scale(must be squared)
	* @param (int)terrain Y scale
	* @param vector containing loaded terrains
	* @param heightfields vector stack
	*
	* @return returning int pointing out to modified terrains from the vector stack
	**/
	std::vector<int> flatten(BoundingSphere,
		int,
		int,
		std::vector<gameplay::Terrain*>,
		std::vector<HeightField*>);

};