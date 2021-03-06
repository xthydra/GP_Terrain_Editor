﻿/*
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

using namespace gameplay;

/**
* This structure is called and destroyed only for generating large scale tasks related to heightfields
**/
struct TerrainGenerator
{

public:

	/**
	* Generate positions from a given object on every heightfields
	*
	* @param (Vector3) Worldscale(terrain world matrix scale)
	* @param (int) Terrains scaleXZ
	* @param (int) Terrains scaleY
	* @param (int) pourcentage of chance that the given object is going to spawn (1-100)
	* @param (size_t) Heightfield resolution
	* @param (std::vector<std::vector<gameplay::Heightfield*> >) Heightfields vectors
	* @param (gameplay::Node*) object Node
	*
	* @return std::vector<Vector3> objectsPosition
	**/
	std::vector<std::vector<std::vector<Vector3*> > >
		generateObjectsPosition(Vector3,
								int,
								int,
								int,
								size_t,
								std::vector<std::vector<gameplay::HeightField *> >,
								Node*);

	/**
	* Generate normalmaps as a PNG extension
	*
	* @param terrain scaleY
	* @param terrain scaleXZ
	* @param Heightfield resolution
	* @param Heightfields vectors
	*
	* @return RAW normalMaps
	**/
	std::vector<std::vector<std::vector<unsigned char> > >
		createNormalmaps(int,
						 int,
						 size_t,
						 std::vector<std::vector<gameplay::HeightField *> >);

	/**
	* Called to generate new random terrains from the given values.
	*
	* @param Heightfield resolution
	* @param Tiles resolution
	* @param minimum height of the terrains
	* @param maximum height of the terrains
	* @param Noise generator type
	* @param Noise Gain
	* @param Noise Amplitude
	* @param Boolean for Noise Amplitude
	* @param Boolean for Noise Gain
	*
	* @return std::vector<std::vector<gameplay::HeightField*>> Heightfield[z][x]
	**/
	std::vector<std::vector<gameplay::HeightField*> > buildTerrainTiles(size_t,
																	   size_t,
																	   int,
																	   int,
																	   int,
																	   int,
																	   int,
																	   bool,
																	   bool);

	/**
	* Generate new blend images for the texture mapping. The blend maps are based on characteristics of the terrain like height or slope.
	*
	* @param Terrains ScaleY
	* @param Terrains ScaleXZ
	* @param Intensity of Blendmap_1
	* @param Intensity of Blendmap_2
	* @param Opacity of Blendmap_1
	* @param Opacity of Blendmap_2
	* @param Heightfield resolution
	* @param Heightfields vectors
	*
	* @return RAW blendmaps
	**/
	std::vector<std::vector<std::vector<std::vector<unsigned char> > > >
		TerrainGenerator::createTiledTransparentBlendImages(int,
															int,
															float,
															float,
															float,
															float,
															size_t,
															std::vector<std::vector<gameplay::HeightField *> >);

	/*last directory used to output files*/
	char * lastDIR;
};
