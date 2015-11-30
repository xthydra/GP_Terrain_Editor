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

CTerrain::CTerrain(gameplay::HeightField * heightField,
				   int LODQuality,
				   int ScaleTexture,
				   int skirtSize,
				   int patchSize,
				   gameplay::Vector3 scale,
				   gameplay::Vector3 pos,
				   std::string normalmap,
				   const char* blend1,
				   const char* blend2,
				   const char* terrainMaterial,
				   gameplay::Scene* scen)
{
	this->_scene = scen;

	if (normalmap.size() > 0)
	{
		terrain = gameplay::Terrain::create(heightField,
			scale,
			patchSize,
			LODQuality,
			skirtSize,
			normalmap.c_str(),
			terrainMaterial);
	}
	else {
		terrain = gameplay::Terrain::create(heightField,
			scale,
			patchSize,
			LODQuality,
			skirtSize,
			NULL,
			terrainMaterial);
	}
	
	terrain->setLayer(0, "res/common/terrain/grass.dds", gameplay::Vector2(ScaleTexture, ScaleTexture));
	terrain->setLayer(1, "res/common/terrain/dirt.dds", gameplay::Vector2(ScaleTexture, ScaleTexture), blend1, 3);
	terrain->setLayer(2, "res/common/terrain/rock.dds", gameplay::Vector2(ScaleTexture, ScaleTexture), blend2, 3);

	terrainNode = gameplay::Node::create("terrain");
	terrainNode->setTranslation(pos);
	terrainNode->setDrawable(terrain);

	_scene->addNode(terrainNode);

	gameplay::PhysicsRigidBody::Parameters *rigidParams = new gameplay::PhysicsRigidBody::Parameters();
	rigidParams->mass = 1;
	rigidParams->kinematic = true;
	_physicObject = terrainNode->setCollisionObject(gameplay::PhysicsCollisionObject::RIGID_BODY, gameplay::PhysicsCollisionShape::heightfield(), rigidParams);
}

CTerrain::~CTerrain()
{
	_scene->removeNode(terrainNode);
	int refcount = terrainNode->getRefCount();
	for (size_t i = 0; i < refcount; i++)
	{
		terrainNode->release();
	}
	refcount = terrain->getRefCount();
	for (size_t i = 0; i < refcount; i++)
	{
		terrain->release();
	}
}
