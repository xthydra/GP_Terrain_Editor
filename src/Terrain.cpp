/*
	GP_Terrain_Editor - GamePlay3D Unoffical Third Party Terrain Editor
	Copyright (C) 2014 Anthony Belisle <xt.hydra@gmail.com>

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
				   const char* normalmap,
				   const char* blend1,
				   const char* blend2,
				   gameplay::Scene* scen)
{
	this->_Scene = scen;

	_terrain = gameplay::Terrain::create(heightField,
		scale,
		patchSize,
		LODQuality,
		skirtSize,
		normalmap,
		"res/materials/terrain.material");
	
	_terrain->setLayer(0, "res/common/terrain/grass.dds", gameplay::Vector2(ScaleTexture, ScaleTexture));
	_terrain->setLayer(1, "res/common/terrain/dirt.dds", gameplay::Vector2(ScaleTexture, ScaleTexture), blend1, 3);
	_terrain->setLayer(2, "res/common/terrain/rock.dds", gameplay::Vector2(ScaleTexture, ScaleTexture), blend2, 3);

	terrainNode = gameplay::Node::create("terrain");
	terrainNode->setTranslation(pos);
	terrainNode->setDrawable(_terrain);
	//terrainNode->setTerrain(_terrain);
	_Scene->addNode(terrainNode);

	gameplay::PhysicsRigidBody::Parameters *rigidParams = new gameplay::PhysicsRigidBody::Parameters();
	rigidParams->mass = 1;
	rigidParams->kinematic = true;
	_PhyObject = terrainNode->setCollisionObject(gameplay::PhysicsCollisionObject::RIGID_BODY, gameplay::PhysicsCollisionShape::heightfield(), rigidParams);
}

CTerrain::~CTerrain()
{
	_Scene->removeNode(terrainNode);
	int refcount = terrainNode->getRefCount();
	for (size_t i = 0; i < refcount; i++)
	{
		terrainNode->release();
		//SAFE_RELEASE(terrainNode);
	}
	refcount = _terrain->getRefCount();
	for (size_t i = 0; i < refcount; i++)
	{
		_terrain->release();
		//SAFE_RELEASE(_terrain);
	}
}
