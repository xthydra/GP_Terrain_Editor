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

struct CTerrain
{
public:
	/**
	* Heightfield constructor
	*
	* @param (Heightfield*)heightfield
	* @param (int)level of detail quality
	* @param (int)scale of texture
	* @param (int)skirt size
	* @param (int)patch size
	* @param (Vector3)scale
	* @param (Vector3)position
	* @param vector stack of 3 strings defining the texture locations used by the blendmaps
	* @param (std::string) normalmap
	* @param (const char *)blendmap1
	* @param (const char *)blendmap2
	* @param (const char *)terrain material path
	* @param (Scene*)scene
	*
	* @return void
	**/
	CTerrain(gameplay::HeightField * heightField,
		int LODQuality,
		int ScaleTexture,
		int skirtSize,
		int patchSize,
		gameplay::Vector3,
		gameplay::Vector3,
		std::vector<std::string>,
		std::string,
		const char*,
		const char*,
		const char*,
		gameplay::Scene*);

	/*the terrain*/
	gameplay::Terrain * terrain;

	/*the terrain node*/
	gameplay::Node *terrainNode;

	~CTerrain();
private:

	/*the physical object of the terrain*/
	gameplay::PhysicsCollisionObject * _physicObject;

	/*a pointer to the scene created in "Main.cpp"*/
	gameplay::Scene* _scene;
};