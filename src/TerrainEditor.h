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
	*
	* @return void
	**/
	void aligningTerrainsVertexes(std::vector<std::vector<HeightField*> >, 
								  size_t);

	/**
	* Raise a part of the terrain vertices defined by a radius and a position
	*
	* @param x x coordinate for the center of the circle.
	* @param z z coordinate for the center of the circle.
	* @param scale, the radius that's defining the vertexes height to be modified
	* @param vector containing loaded terrains
	* @param an int pointing out to the terrain that got it by the ray
	* @param heightfield size
	* @param heightfields vector stack
	*
	* @return returning int pointing out to modified terrains from the vector stack
	**/
	std::vector<int> raiseTiles(float,
								float,
								float,
								std::vector<gameplay::Terrain*>,
								int,
								int,
								std::vector<HeightField*>);

	/**
	* Lower a part of the terrain vertices defined by a radius and a position
	*
	* @param x coordinate for the center of the circle.
	* @param z coordinate for the center of the circle.
	* @param scale, the radius that's defining the vertexes height to be modified
	* @param vector containing loaded terrains
	* @param an int pointing out to the terrain that got it by the ray
	* @param heightfield size
	* @param heightfields vector stack
	*
	* @return returning int pointing out to modified terrains from the vector stack
	**/
	std::vector<int> lowerTiles(float,
								float,
								float,
								std::vector<gameplay::Terrain*>,
								int,
								int,
								std::vector<HeightField*>);

	/**
	* Smooth a part of the terrain vertices defined by a radius and a position
	*
	* @param x coordinate for the center of the circle.
	* @param z coordinate for the center of the circle.
	* @param scale, the radius that's defining the vertexes height to be modified
	* @param vector containing loaded terrains
	* @param an int pointing out to the terrain that got it by the ray
	* @param heightfield size
	* @param heightfields vector stack
	*
	* @return returning int pointing out to modified terrains from the vector stack
	**/
	std::vector<int> smoothTiles(float,
								 float,
								 float, 
								 std::vector<gameplay::Terrain*>, 
								 int, 
								 int, 
								 std::vector<HeightField*>);

	/**
	* Flatten a circle of the terrain. The contents of the circle are all brought to the average - and some smoothing is done to the edges.
	*
	* @param x x coordinate for the center of the circle.
	* @param z z coordinate for the center of the circle.
	* @param scale, the radius that's defining the vertexes height to be modified
	* @param vector containing loaded terrains
	* @param an int pointing out to the terrain that got it by the ray
	* @param heightfield size
	* @param heightfields vector stack
	*
	* @return returning int pointing out to modified terrains from the vector stack
	**/
	std::vector<int> flattenTiles(float,
								  float,
								  float,
								  std::vector<gameplay::Terrain*>,
								  int,
								  int,
								  std::vector<HeightField*>);

};