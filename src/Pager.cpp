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

#include "Main.h"

// heightfields[z][x]//only horizontal pager
// heightfields[z][x][y]//pager with vertical paging
// heightfields[z][x][y][w]//pager supporting negative world position


Pager::Pager(PagerParameters param, gameplay::Scene* scen)
{
	this->_Scene = scen;

	// get parameters
	parameters = param;

	zoneList.resize(parameters.zoneResolution);
	for (int i = 0; i < parameters.zoneResolution; i++)
	{
		zoneList[i].resize(parameters.zoneResolution);
		for (int j = 0; j < parameters.zoneResolution; j++)
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

			Zone * zone = new Zone(gameplay::Vector3(posX, 0, posZ), parameters.scale);

			zoneList[i][j] = zone;
		}
	}
}

void Pager::loadTerrain(int z, int x)
{
	CTerrain * Terr;

	if (zoneList.size() > 0)
	{
		std::string blendName1, blendName2, normalName;
		//it make sure the blend maps image have been saved to the disk
		if (parameters.generatedBlendmaps == false)
		{
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

			std::ifstream in(blendName1, std::ios::binary | std::ios::ate);
			std::ifstream in2(blendName2, std::ios::binary | std::ios::ate);
			int fileSize = in.tellg();
			int fileSize2 = in2.tellg();

			if (fileSize == -1 || fileSize2 == -1)
			{
				if (zoneList[z][x]->blendMaps.size() > 0)
				{
					FilesSaver saver;
					saver.saveBlendmap(zoneList[z][x]->blendMaps[0],
						zoneList[z][x]->blendMaps[1],
						parameters.blendMapDIR,
						z,
						x,
						parameters.heightFieldResolution);
				}
				else
				{
					blendName1.clear();
					blendName2.clear();
				}
			}
			if (zoneList[z][x]->blendMaps.size() > 0)
			{
				FilesSaver saver;
				saver.saveBlendmap(zoneList[z][x]->blendMaps[0],
					zoneList[z][x]->blendMaps[1],
					parameters.blendMapDIR,
					z,
					x,
					parameters.heightFieldResolution);
			}
		}
		else 
		{
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
		}
		//it make sure the normal maps image have been saved to the disk
		if (parameters.generatedNormalmaps == false)
		{
			normalName += parameters.normalMapDIR;

			normalName += "normalMap-";
			normalName += std::to_string(z);
			normalName += "-";
			normalName += std::to_string(x);
			normalName += ".png";

			std::ifstream in(normalName, std::ios::binary | std::ios::ate);
			int fileSize = in.tellg();

			if (fileSize == -1)
			{
				if (zoneList[z][x]->normalMap.size() > 0)
				{
					FilesSaver saver;
					saver.saveNormalmap(zoneList[z][x]->normalMap,
						parameters.normalMapDIR,
						z,
						x,
						parameters.heightFieldResolution);
				}
				else 
				{
					normalName.clear();
				}
			}
		}
		else 
		{
			normalName += parameters.normalMapDIR;

			normalName += "normalMap-";
			normalName += std::to_string(z);
			normalName += "-";
			normalName += std::to_string(x);
			normalName += ".png";
		}
		//copying the heightfield before sending it because if i ask gameplay to delete a terrain it would delete the heightfield too
		int resolution = parameters.heightFieldResolution;
		gameplay::HeightField* field = HeightField::create(resolution, resolution);
		memcpy(field->getArray(), zoneList[z][x]->heightField->getArray(), sizeof(float)*(resolution*resolution));
	
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
			zoneList[z][x]->texturesLocation,
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
	loadedHeightfields.push_back(zoneList[z][x]->heightField);
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

int Pager::findTerrain(Vector3 pos)
{
	int nearest = -1;
	float distance1=-1, distance2=-1;
	for (size_t i = 0; i < loadedTerrains.size(); i++)
	{
		float distance1 = pos.distance(loadedTerrains[i]->getNode()->getTranslationWorld());
		if (distance1 > distance2)
		{
			nearest = i;
		}
		distance2 = distance1;
	}
	return nearest;
}

void Pager::computePositions()
{
	if (zoneList.size() != 0)
	{
		zoneList.resize(parameters.zoneResolution);
		for (int i = 0; i < parameters.zoneResolution; i++)
		{
			zoneList[i].resize(parameters.zoneResolution);
			for (int j = 0; j < parameters.zoneResolution; j++)
			{
				float posX = j*parameters.boundingBox;
				float posZ = i*parameters.boundingBox;

				Zone * zone = new Zone(gameplay::Vector3(posX, 0, posZ), parameters.scale);

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
	for (int i = 0; i < parameters.zoneResolution; i++)
	{
		for (int j = 0; j < parameters.zoneResolution; j++)
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
				loadedTerrains[i]->draw(parameters.debug);

				if (objects.size() > 0)
				{
					int posX = (loadedTerrains[i]->getNode()->getTranslationWorld().x / parameters.scale.x) / (parameters.heightFieldResolution - 1);
					int posZ = (loadedTerrains[i]->getNode()->getTranslationWorld().z / parameters.scale.x) / (parameters.heightFieldResolution - 1);

					for (size_t j = 0; j < objects[posZ][posX].size(); j++)
					{
						//BoundingSphere objectBound = objects[posZ][posX][j]->getBoundingSphere();
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