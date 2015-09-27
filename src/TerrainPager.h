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

#include "gameplay.h"
#include "main.h"

struct STerrainParameters
{
	/*
	Heightfields used for editing and loading terrains
	*/
	std::vector<std::vector<gameplay::HeightField*> > heightFieldList;


	/*
	Files List to load
	*/
	std::vector<std::vector<char*> >			   normalMapList;
	std::vector<std::vector<char*> >			   heightMapPNGList;
	std::vector<std::vector<char*> >			   heightMapRAWList;
	std::vector<std::vector<std::vector<char*> > > blendMapList;

	/*
	Files List to encode
	*/
	std::vector<std::vector<std::vector<std::vector<unsigned char> > > > blendMaps;
	std::vector<std::vector<std::vector<unsigned char> > >				 normalMaps;

	/*
	Objects List
	*/
	std::vector<std::vector<std::vector<Vector3*> > > objectsPosition;
	std::vector<std::vector<std::vector<Node*> > > objects;
	std::vector<char*> objectsFilename;

	/*
	Loaded terrains used for rendering and editing
	*/
	std::vector<gameplay::Terrain*> loadedTerrains;
	std::vector<gameplay::HeightField*> loadedHeightfields;

	//gameplay::Vector3 Position;//unused //it would be for tiles offset
	//std::vector<std::vector<std::vector<Vector3*> > >
	gameplay::Vector3 Scale;

	/*
	folder name to access files during terrain init
	*/
	char * BlendMapDIR;
	char * NormalMapDIR;
	char * HeightMapDIR;

	int LodQuality;
	int TextureScale;
	int BoundingBox;
	int minHeight;
	int maxHeight;
	int patchSize;
	int skirtSize;
	int tilesResolution;
	int heightFieldResolution;

	float DistanceLoad;
	float DistanceUnload;
	float DistanceMaxRender;
	float DistanceMaxModelRender;

	bool generatedBlendmaps;
	bool generatedNormalmaps;
	bool generatedHeightmaps;
	bool generatedObjects;
	bool Debug;//it's for terrain->draw(wireframe)
};

class CBufferZone
{
public:
	CBufferZone(gameplay::Vector3 pos, gameplay::Vector3 scale)
	{
		_pos = pos;
		_scale = scale;
		setLoaded(false);
	}
	CTerrain * getObjectInside() { return object; }
	gameplay::Vector3 getPosition(){ return _pos; }
	gameplay::Vector3 getScale(){ return _scale; }

	void setLoaded(bool state) { loaded = state; }
	void setObjectInside(CTerrain * terrain) { object = terrain; }
	bool isLoaded() { return loaded; }
private:
	gameplay::Vector3 _pos, _scale;
	CTerrain * object;
	gameplay::Scene* _Scene;
	bool loaded;
};

class TerrainPager
{
public:
	TerrainPager(STerrainParameters param, gameplay::Scene* scen);
	void render();
	void PagingCheck();
	void loadTerrain(int, int);
	void reloadTerrain(int, int, HeightField*);
	void removeTerrain(int, int);
	void removeTerrains();
	void computePositions();
	void loadHeightfields();
	
	/**
	* Reload all renderable terrains
	**/
	void reloadTerrains();

	/**
	* Reload given terrains
	*
	* @param vector position to current loaded terrains
	* @return void
	**/
	void reload(std::vector<int>);

	/**
	* Find terrain under cursor
	*
	* @param cursor position
	* @param window resolution
	* @return int pointing to the terrain in the vector stack
	**/
	int findTerrain(Vector2 pos, Vector2 resolution);

	std::vector<std::vector<CBufferZone*> > zoneList;
	//std::vector<std::vector<gameplay::HeightField*> > heightFieldList;
	STerrainParameters Param;
private:
	gameplay::Scene * _Scene;

	//std::vector<std::vector<char*> > heightMapList;
	//std::vector<std::vector<char*> > normalMapList;
};
