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

// heightfields[z][x]//only horizontal pager
// heightfields[z][x][y]//pager with vertical paging
// heightfields[z][x][y][w]//pager supporting negative world position

Pager::Pager(PagerParameters param, gameplay::Scene* scen)
{
	this->_Scene = scen;

	// get parameters
	parameters = param;

	if (heightFieldList.size() > 0)
	{
		zoneList.resize(parameters.tilesResolution);
		for (int i = 0; i < parameters.tilesResolution; i++)
		{
			zoneList[i].resize(parameters.tilesResolution);
			for (int j = 0; j < parameters.tilesResolution; j++)
			{
#define TEST1
#ifdef TEST1
				float posX = j*param.boundingBox;
				float posZ = i*param.boundingBox;
#endif
#ifdef TEST2
				float posX = -j*param.BoundingBox;
				float posZ = i*param.BoundingBox;
#endif
#ifdef TEST3
				float posX = j*param.BoundingBox;
				float posZ = -i*param.BoundingBox;
#endif
#ifdef TEST4
				float posX = -j*param.BoundingBox;
				float posZ = -i*param.BoundingBox;
#endif
				////////////////////////////////////////////////////////
#ifdef TEST5
				float posX = i*param.BoundingBox;
				float posZ = j*param.BoundingBox;
#endif

#ifdef TEST6
				float posX = -i*param.BoundingBox;
				float posZ = j*param.BoundingBox;
#endif
#ifdef TEST7
				float posX = i*param.BoundingBox;
				float posZ = -j*param.BoundingBox;
#endif

#ifdef TEST8
				float posX = -i*param.BoundingBox;
				float posZ = -j*param.BoundingBox;
#endif

				BufferZone * zone = new BufferZone(gameplay::Vector3(posX, 0, posZ), parameters.scale);

				zoneList[i][j] = zone;
			}
		}
	}
}

void Pager::loadTerrain(int z, int x)
{
	CTerrain * Terr;

	if (heightFieldList.size() > 0)
	{
		std::string blendName1, blendName2, normalName;
		if (parameters.generatedBlendmaps == false)
		{
			if (blendMaps.size() > 0)
			{
				FilesSaver saver;
				saver.saveBlendmap(blendMaps[z][x][0],
					blendMaps[z][x][1],
					parameters.blendMapDIR,
					z,
					x,
					parameters.heightFieldResolution);
			}
		}
		if (parameters.generatedNormalmaps == false)
		{
			if (normalMaps.size() > 0)
			{
				FilesSaver saver;
				saver.saveNormalmap(normalMaps[z][x],
					parameters.normalMapDIR,
					z,
					x,
					parameters.heightFieldResolution);
			}
			normalName += parameters.normalMapDIR;

			normalName += "normalMap-";
			normalName += std::to_string(z);
			normalName += "-";
			normalName += std::to_string(x);
			normalName += ".png";
		}
		//find the blendmap and normal map filename
		blendName1 += parameters.blendMapDIR;

		blendName1 += "blend-";
		blendName1 += std::to_string(z);
		blendName1 += "-";
		blendName1 += std::to_string(x);
		blendName1 += "_";

		blendName2 += blendName1;

		blendName1 += std::to_string(1);
		blendName1 += ".png";

		blendName2 += std::to_string(2);
		blendName2 += ".png";

		if (parameters.generatedNormalmaps == true)
		{
			normalName += parameters.normalMapDIR;

			normalName += "normalMap-";
			normalName += std::to_string(z);
			normalName += "-";
			normalName += std::to_string(x);
			normalName += ".png";
		}

		//I copy the heightfield before sending to create a terrain
		int resolution = parameters.heightFieldResolution;

		gameplay::HeightField* field = HeightField::create(resolution, resolution);

		for (size_t i = 0; i < resolution; i++)
		{
			for (size_t j = 0; j < resolution; j++)
			{
				size_t vertexe = (j*resolution) + i;
				float * vertex = field->getArray();
				vertex[vertexe] = heightFieldList[z][x]->getHeight(i, j);
			}
		}
	
		//create the terrain
		Terr = new CTerrain(field,
			parameters.lodQuality,
			parameters.textureScale,
			parameters.skirtSize,
			parameters.patchSize,
			parameters.scale,
			gameplay::Vector3(zoneList[z][x]->getPosition().x,
				0,
				zoneList[z][x]->getPosition().z),
			normalName,
			blendName1.c_str(),
			blendName2.c_str(),
			parameters.terrainMaterialPath.c_str(),
			this->_Scene);
	}

	//terrain pager values to check against
	zoneList[z][x]->setLoaded(true);
	zoneList[z][x]->setObjectInside(Terr);

	loadedHeightfields.shrink_to_fit();
	loadedTerrains.shrink_to_fit();

	//terrain pager values to check against
	loadedTerrains.push_back(zoneList[z][x]->getObjectInside()->terrain);
	loadedHeightfields.push_back(heightFieldList[z][x]);
}

void Pager::removeTerrain(int z, int x)
{
	for (size_t g = 0; g < loadedTerrains.size(); g++)
	{
		if (loadedTerrains[g]->getNode()->getTranslationWorld() == zoneList[z][x]->getObjectInside()->terrainNode->getTranslationWorld())
		{
			loadedTerrains.erase(loadedTerrains.begin() + g);
			loadedHeightfields.erase(loadedHeightfields.begin() + g);
		}
	}
	zoneList[z][x]->getObjectInside()->~CTerrain();

	zoneList[z][x]->setLoaded(false);
}

void Pager::removeTerrains()
{
	std::vector<int> PosX, PosZ;
	for (size_t i = 0; i < loadedTerrains.size(); i++)
	{
		PosX.push_back((loadedTerrains[i]->getNode()->getTranslationWorld().x / parameters.scale.x) / (parameters.heightFieldResolution - 1));
		PosZ.push_back((loadedTerrains[i]->getNode()->getTranslationWorld().z / parameters.scale.x) / (parameters.heightFieldResolution - 1));
	}
	for (size_t i = 0; i < loadedTerrains.size(); i++)
	{
		removeTerrain(PosZ[i], PosX[i]);
	}
	PosX.~vector();
	PosZ.~vector();
}

void Pager::reloadTerrains()
{
	std::vector<int> PosX, PosZ;
	for (size_t i = 0; i < loadedTerrains.size(); i++)
	{
		PosX.push_back((loadedTerrains[i]->getNode()->getTranslationWorld().x / parameters.scale.x) / (parameters.heightFieldResolution - 1));
		PosZ.push_back((loadedTerrains[i]->getNode()->getTranslationWorld().z / parameters.scale.x) / (parameters.heightFieldResolution - 1));
	}
	for (size_t i = 0; i < loadedTerrains.size(); i++)
	{
		removeTerrain(PosZ[i], PosX[i]);
		loadTerrain(PosZ[i], PosX[i]);
	}
	PosX.~vector();
	PosZ.~vector();
}

void Pager::reload(std::vector<int> pos)
{
	std::vector<int> PosX, PosZ;
	for (size_t i = 0; i < pos.size(); i++)
	{
		PosX.push_back(loadedTerrains[pos[i]]->getNode()->getTranslationWorld().x);
		PosZ.push_back(loadedTerrains[pos[i]]->getNode()->getTranslationWorld().z);
	}
	for (size_t i = 0; i < pos.size(); i++)
	{
		//get the heightfield vector position by dividing by scaling then divide by heightfield resolution
		int fieldX = (PosX[i] / parameters.scale.x) / (parameters.heightFieldResolution - 1);
		int fieldZ = (PosZ[i] / parameters.scale.x) / (parameters.heightFieldResolution - 1);

		removeTerrain(fieldZ, fieldX);
		loadTerrain(fieldZ, fieldX);
	}
	PosX.~vector();
	PosZ.~vector();
}

int Pager::findTerrain(Vector2 pos, Vector2 resolution)
{
	Ray pickRay;
	//_Scene->getActiveCamera()->pickRay(Rectangle(0, 0, width, height), posX, posY, &pickRay);
	_Scene->getActiveCamera()->pickRay(gameplay::Rectangle(0, 0, resolution.x, resolution.y), pos.x, pos.y, &pickRay);

	for (size_t i = 0; i < loadedTerrains.size(); i++)
	{
		TerrainHitFilter hitFilter(loadedTerrains[i]);
		PhysicsController::HitResult hitResult;
		if (Game::getInstance()->getPhysicsController()->rayTest(pickRay,
			1000000,
			&hitResult,
			&hitFilter))
		{
			if (hitResult.object == loadedTerrains[i]->getNode()->getCollisionObject())
			{
				return i;
			}
		}
	}
	return -1;
}

void Pager::computePositions()
{
	if (heightFieldList.size() > 0)
	{
		zoneList.resize(parameters.tilesResolution);
		for (int i = 0; i < parameters.tilesResolution; i++)
		{
			zoneList[i].resize(parameters.tilesResolution);
			for (int j = 0; j < parameters.tilesResolution; j++)
			{
				float posX = j*parameters.boundingBox;
				float posZ = i*parameters.boundingBox;

				BufferZone * zone = new BufferZone(gameplay::Vector3(posX, 0, posZ), parameters.scale);

				zoneList[i][j] = zone;
			}
		}
	}
}

void Pager::removeObjects()
{
	for (size_t i = 0; i < objects.size();)
	{
		for (size_t j = 0; j < objects[i].size();)
		{
			for (size_t g = 0; g < objects[i][j].size(); g++)
			{
				objects[i][j][g]->removeAllChildren();
				_Scene->removeNode(objects[i][j][g]);
				int refcount = objects[i][j][g]->getRefCount();
				for (size_t f = 0; f < refcount; f++)
				{
					objects[i][j][g]->release();
				}
			}
			j++;
		}
		i++;
	}
	for (size_t i = 0; i < objects.size();)
	{
		for (size_t j = 0; j < objects[i].size();)
		{
			objects[i][j].clear();
			j++;
		}
		objects[i].clear();
		i++;
	}
	objects.clear();
}

void Pager::pagingCheck()
{
	for (int i = 0; i < parameters.tilesResolution; i++)
	{
		for (int j = 0; j < parameters.tilesResolution; j++)
		{
			Vector3 v = _Scene->getActiveCamera()->getNode()->getTranslationWorld();
			Vector3 v2 = zoneList[i][j]->getPosition();

			float dx = v.x - v2.x;
			float dz = v.z - v2.z;

			int ActualDistance = sqrt(dx * dx + dz * dz);

			bool load = zoneList[i][j]->isLoaded();

			if (ActualDistance < parameters.distanceTerrainLoad && load == false)
			{
				loadTerrain(i, j);
			}
			if (ActualDistance > parameters.distanceTerrainUnload && load == true)
			{
				removeTerrain(i, j);
			}
		}
	}
}

void Pager::render()
{
	for (size_t i = 0; i < loadedTerrains.size(); i++)
	{	
		BoundingBox bound(
			(loadedTerrains[i]->getNode()->getTranslationWorld().x - (parameters.boundingBox*0.5)),
			loadedTerrains[i]->getBoundingBox().min.y,
			(loadedTerrains[i]->getNode()->getTranslationWorld().z - (parameters.boundingBox*0.5)),
			(loadedTerrains[i]->getNode()->getTranslationWorld().x + (parameters.boundingBox*0.5)),
			loadedTerrains[i]->getBoundingBox().max.y,
			(loadedTerrains[i]->getNode()->getTranslationWorld().z + (parameters.boundingBox*0.5))
			);
		if (_Scene->getActiveCamera()->getFrustum().intersects(bound))
		{
			Vector3 v = _Scene->getActiveCamera()->getNode()->getParent()->getTranslationWorld();
			Vector3 v2 = loadedTerrains[i]->getNode()->getTranslationWorld();

			float dx = v.x - v2.x;
			float dz = v.z - v2.z;

			int ActualDistance = sqrt(dx * dx + dz * dz);

			if (ActualDistance < parameters.distanceTerrainMaxRender)
			{
				loadedTerrains[i]->draw(parameters.Debug);

				if (objects.size() > 0)
				{
					int posX = (loadedTerrains[i]->getNode()->getTranslationWorld().x / parameters.scale.x) / (parameters.heightFieldResolution - 1);
					int posZ = (loadedTerrains[i]->getNode()->getTranslationWorld().z / parameters.scale.x) / (parameters.heightFieldResolution - 1);

					for (size_t j = 0; j < objects[posZ][posX].size(); j++)
					{
						BoundingSphere test = objects[posZ][posX][j]->getBoundingSphere();
						if (_Scene->getActiveCamera()->getFrustum().intersects(
							objects[posZ][posX][j]->getBoundingSphere()))
						{
							Vector3 v = _Scene->getActiveCamera()->getNode()->getTranslationWorld();
							Vector3 v2 = objects[posZ][posX][j]->getTranslationWorld();

							float dx = v.x - v2.x;
							float dz = v.z - v2.z;

							int ActualDistance = sqrt(dx * dx + dz * dz);

							if (ActualDistance < parameters.distanceMaxModelRender)
							{

								if (objects[posZ][posX][j]->getDrawable())
								{
									objects[posZ][posX][j]->getDrawable()->draw();
								}
								if (objects[posZ][posX][j]->getChildCount() > 0)
								{
									Node *childs = objects[posZ][posX][j]->getFirstChild();
									while (childs)
									{
										if (childs->getDrawable())
										{
											//if(objectsParameters[0] > 0)
											//Model* model = dynamic_cast<Model*>(childs->getDrawable());
											//int texRepeat = objectsParameters[0]*(ActualDistance / parameters.distanceMaxModelRender);
											//model->getMaterial()->getParameter("u_textureRepeat")->setValue(texRepeat);
											childs->getDrawable()->draw();
										}
										childs = childs->getNextSibling();
									}
								}
							}
						}
					}
				}
			}
		}
	}
}