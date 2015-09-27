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

// heightfields[z][x]//only horizontal pager
// heightfields[z][x][y]//pager with vertical paging
// heightfields[z][x][y][w]//pager supporting negative world position

TerrainPager::TerrainPager(STerrainParameters param, gameplay::Scene* scen)
{
	this->_Scene = scen;

	// get parameters
	Param = param;

	if (Param.heightFieldList.size() > 0)
	{
		zoneList.resize(Param.tilesResolution);
		for (int i = 0; i < Param.tilesResolution; i++)
		{
			zoneList[i].resize(Param.tilesResolution);
			for (int j = 0; j < Param.tilesResolution; j++)
			{
#define TEST1
#ifdef TEST1
				float posX = j*param.BoundingBox;
				float posZ = i*param.BoundingBox;
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

				CBufferZone * zone = new CBufferZone(gameplay::Vector3(posX, 0, posZ), Param.Scale);

				zoneList[i][j] = zone;
			}
		}
	}
}

void TerrainPager::loadTerrain(int z, int x)
{
	CTerrain * Terr;
	if (Param.heightFieldList.size() > 0)
	{
			if (Param.generatedBlendmaps == false)
			{
				FilesSaver saver;
				saver.saveBlendmap(Param.blendMaps[z][x][0],
								   Param.blendMaps[z][x][1],
								   Param.BlendMapDIR,
								   z,
								   x,
								   Param.heightFieldResolution);
			}
			if (Param.generatedNormalmaps == false)
			{
				FilesSaver saver;
				saver.saveNormalmap(Param.normalMaps[z][x],
					Param.NormalMapDIR,
					z,
					x,
					Param.heightFieldResolution);
			}
			std::string blendName1, blendName2, normalName;
			blendName1 += Param.BlendMapDIR;
			normalName += Param.NormalMapDIR;

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

			normalName += "normalMap-";
			normalName += std::to_string(z);
			normalName += "-";
			normalName += std::to_string(x);
			normalName += ".png";

			Terr = new CTerrain(Param.heightFieldList[z][x],
				Param.LodQuality,
				Param.TextureScale,
				Param.skirtSize,
				Param.patchSize,
				Param.Scale,
				gameplay::Vector3(zoneList[z][x]->getPosition().x,
				0,
				zoneList[z][x]->getPosition().z),
				normalName.c_str(),
				blendName1.c_str(),
				blendName2.c_str(),
				this->_Scene);
	}

	zoneList[z][x]->setLoaded(true);
	zoneList[z][x]->setObjectInside(Terr);

	Param.loadedHeightfields.shrink_to_fit();
	Param.loadedTerrains.shrink_to_fit();

	Param.loadedTerrains.push_back(zoneList[z][x]->getObjectInside()->_terrain);
	Param.loadedHeightfields.push_back(Param.heightFieldList[z][x]);
}

void TerrainPager::reloadTerrain(int z, int x,HeightField * field)
{
	CTerrain * Terr;
	if (Param.heightFieldList.size() > 0)
	{
		if (Param.generatedBlendmaps == false)
		{
			FilesSaver saver;
			saver.saveBlendmap(Param.blendMaps[z][x][0],
				Param.blendMaps[z][x][1],
				Param.BlendMapDIR,
				z,
				x,
				Param.heightFieldResolution);
		}
		if (Param.generatedNormalmaps == false)
		{
			FilesSaver saver;
			saver.saveNormalmap(Param.normalMaps[z][x],
				Param.NormalMapDIR,
				z,
				x,
				Param.heightFieldResolution);
		}
		std::string blendName1, blendName2, normalName;
		blendName1 += Param.BlendMapDIR;
		normalName += Param.NormalMapDIR;

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

		normalName += "normalMap-";
		normalName += std::to_string(z);
		normalName += "-";
		normalName += std::to_string(x);
		normalName += ".png";

		Param.heightFieldList[z][x] = field;

		Terr = new CTerrain(field,
			Param.LodQuality,
			Param.TextureScale,
			Param.skirtSize,
			Param.patchSize,
			Param.Scale,
			gameplay::Vector3(zoneList[z][x]->getPosition().x,
				0,
				zoneList[z][x]->getPosition().z),
			normalName.c_str(),
			blendName1.c_str(),
			blendName2.c_str(),
			this->_Scene);
	}

	zoneList[z][x]->setLoaded(true);
	zoneList[z][x]->setObjectInside(Terr);

	Param.loadedTerrains.push_back(zoneList[z][x]->getObjectInside()->_terrain);
	Param.loadedHeightfields.push_back(Param.heightFieldList[z][x]);
}


void TerrainPager::removeTerrain(int z, int x)
{
	for (size_t g = 0; g < Param.loadedTerrains.size(); g++)
	{
		if (Param.loadedTerrains[g]->getNode()->getTranslationWorld() == zoneList[z][x]->getObjectInside()->terrainNode->getTranslationWorld())
		{
			Param.loadedTerrains.erase(Param.loadedTerrains.begin() + g);
			Param.loadedHeightfields.erase(Param.loadedHeightfields.begin() + g);
		}
	}
	zoneList[z][x]->getObjectInside()->~CTerrain();

	zoneList[z][x]->setLoaded(false);
}

void TerrainPager::removeTerrains()
{
	std::vector<int> PosX, PosZ;
	for (size_t i = 0; i < Param.loadedTerrains.size(); i++)
	{
		PosX.push_back((Param.loadedTerrains[i]->getNode()->getTranslationWorld().x / Param.heightFieldResolution) / (Param.heightFieldResolution - 1));
		PosZ.push_back((Param.loadedTerrains[i]->getNode()->getTranslationWorld().z / Param.heightFieldResolution) / (Param.heightFieldResolution - 1));
	}
	for (size_t i = 0; i < Param.loadedTerrains.size(); i++)
	{
		removeTerrain(PosZ[i], PosX[i]);
	}
	PosX.~vector();
	PosZ.~vector();
}

void TerrainPager::reloadTerrains()
{
	std::vector<int> PosX, PosZ;
	for (size_t i = 0; i < Param.loadedTerrains.size(); i++)
	{
		PosX.push_back((Param.loadedTerrains[i]->getNode()->getTranslationWorld().x / Param.heightFieldResolution) / (Param.heightFieldResolution - 1));
		PosZ.push_back((Param.loadedTerrains[i]->getNode()->getTranslationWorld().z / Param.heightFieldResolution) / (Param.heightFieldResolution - 1));
	}
	for (size_t i = 0; i < Param.loadedTerrains.size(); i++)
	{
		removeTerrain(PosZ[i], PosX[i]);
		loadTerrain(PosZ[i], PosX[i]);
	}
	PosX.~vector();
	PosZ.~vector();
}

void TerrainPager::reload(std::vector<int> pos)
{
	std::vector<int> PosX, PosZ;
	for (size_t i = 0; i < pos.size(); i++)
	{
		//get the heightfield vector position by dividing by scaling then divide by heightfield resolution
		PosX.push_back(Param.loadedTerrains[pos[i]]->getNode()->getTranslationWorld().x);
		PosZ.push_back(Param.loadedTerrains[pos[i]]->getNode()->getTranslationWorld().z);
	}
	for (size_t i = 0; i < pos.size(); i++)
	{

		int fieldX = (PosX[i] / Param.heightFieldResolution) / (Param.heightFieldResolution - 1);
		int fieldZ = (PosZ[i] / Param.heightFieldResolution) / (Param.heightFieldResolution - 1);

		int resolution = Param.heightFieldList[fieldZ][fieldX]->getColumnCount();

		gameplay::HeightField* field=HeightField::create(resolution,resolution);

		for (size_t i = 0; i < resolution; i++)
		{
			for (size_t j = 0; j < resolution; j++)
			{
				size_t vertexe = (j*resolution) + i;
				float * vertex = field->getArray();
				vertex[vertexe] = Param.heightFieldList[fieldZ][fieldX]->getHeight(i, j);
			}
		}

		removeTerrain(fieldZ, fieldX);
		reloadTerrain(fieldZ, fieldX, field);
	}
	PosX.~vector();
	PosZ.~vector();
}

int TerrainPager::findTerrain(Vector2 pos, Vector2 resolution)
{
	Ray pickRay;
	//_Scene->getActiveCamera()->pickRay(Rectangle(0, 0, width, height), posX, posY, &pickRay);
	_Scene->getActiveCamera()->pickRay(Rectangle(0, 0, resolution.x, resolution.y), pos.x, pos.y, &pickRay);

	for (size_t i = 0; i < Param.loadedTerrains.size(); i++)
	{
		TerrainHitFilter hitFilter(Param.loadedTerrains[i]);
		PhysicsController::HitResult hitResult;
		if (Game::getInstance()->getPhysicsController()->rayTest(pickRay,
			1000000,
			&hitResult,
			&hitFilter))
		{
			if (hitResult.object == Param.loadedTerrains[i]->getNode()->getCollisionObject())
			{
				return i;
			}
		}
	}
	return -1;
}

void TerrainPager::computePositions()
{
	if (Param.heightFieldList.size() > 0)
	{
		zoneList.resize(Param.tilesResolution);
		for (int i = 0; i < Param.tilesResolution; i++)
		{
			zoneList[i].resize(Param.tilesResolution);
			for (int j = 0; j < Param.tilesResolution; j++)
			{
				float posX = j*Param.BoundingBox;
				float posZ = i*Param.BoundingBox;

				CBufferZone * zone = new CBufferZone(gameplay::Vector3(posX, 0, posZ), Param.Scale);

				zoneList[i][j] = zone;
			}
		}
	}

}

void TerrainPager::loadHeightfields()
{
	if (Param.heightMapPNGList.size() > 0)
	{
		Param.heightFieldList.clear();

		Param.heightFieldList.resize(Param.tilesResolution);
		for (int i = 0; i < Param.tilesResolution; i++)
		{
			Param.heightFieldList[i].resize(Param.tilesResolution);
			for (int j = 0; j < Param.tilesResolution; j++)
			{
				std::string heightName;
				heightName += "res/tmp/";
				heightName += Param.HeightMapDIR;
				heightName += "/";
				heightName += Param.heightMapPNGList[i][j];

				Param.heightFieldList[i][j] = gameplay::HeightField::createFromImage(heightName.c_str(), Param.minHeight, Param.maxHeight);
			}
		}
		Param.heightMapPNGList.clear();
	}
	if (Param.heightMapRAWList.size() > 0)
	{
		Param.heightFieldList.clear();

		Param.heightFieldList.resize(Param.tilesResolution);
		for (int i = 0; i < Param.tilesResolution; i++)
		{
			Param.heightFieldList[i].resize(Param.tilesResolution);
			for (int j = 0; j < Param.tilesResolution; j++)
			{
				std::string heightName;
				heightName += "res/tmp/";
				heightName += Param.HeightMapDIR;
				heightName += "/";
				heightName += Param.heightMapRAWList[i][j];

				gameplay::HeightField * hf = 
					gameplay::HeightField::createFromRAW(heightName.c_str(),
														 Param.heightFieldResolution,
														 Param.heightFieldResolution,
														 Param.minHeight,
														 Param.maxHeight);

				Param.heightFieldList[i][j] = hf;
			}
		}
		Param.heightMapRAWList.clear();
	}
}

void TerrainPager::PagingCheck()
{
	for (int i = 0; i<Param.tilesResolution; i++)
	{
		for (int j = 0; j<Param.tilesResolution; j++)
		{
			//float ActualDistance = _Scene->getActiveCamera()->getNode()->getTranslationWorld().distance(zoneList[i][j]->getPosition());

			Vector3 v = _Scene->getActiveCamera()->getNode()->getTranslationWorld();
			Vector3 v2 = zoneList[i][j]->getPosition();

			float dx = v.x - v2.x;
			float dz = v.z - v2.z;

			int ActualDistance = sqrt(dx * dx + dz * dz);

			bool load = zoneList[i][j]->isLoaded();

			if (ActualDistance < Param.DistanceLoad && load == false)
			{
				loadTerrain(i, j);
			}
			if (ActualDistance > Param.DistanceUnload && load == true)
			{
				removeTerrain(i, j);
			}
		}
	}
}

void TerrainPager::render()
{
	for (size_t i = 0; i < Param.loadedTerrains.size(); i++)
	{
#ifdef DISTANCE1
		//int ActualDistance = _Scene->getActiveCamera()->getNode()->getTranslationWorld().distance(Param.loadedTerrains[i]->getNode()->getTranslationWorld());
#endif
#define DISTANCE2
#ifdef DISTANCE2
		Vector3 v = _Scene->getActiveCamera()->getNode()->getTranslationWorld();
		Vector3 v2 = Param.loadedTerrains[i]->getNode()->getTranslationWorld();

		float dx = v.x - v2.x;
		float dz = v.z - v2.z;

		int ActualDistance = sqrt(dx * dx + dz * dz);
#endif
		if (ActualDistance < Param.DistanceMaxRender)
		{
			Param.loadedTerrains[i]->draw(Param.Debug);
		}
	}
}