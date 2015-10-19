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

	//std::vector<std::vector<std::vector<Vector3*> > >;//unused //it would be for tiles offset

	gameplay::Vector3 Scale;

	/*
	folder name to access blendmap files encoded in PNG
	*/
	char * BlendMapDIR;

	/*
	folder name to access normalmap files encoded in PNG
	*/
	char * NormalMapDIR;

	/*
	folder name to access heightmap files encoded in PNG or binary
	*/
	char * HeightMapDIR;

	/*
	Parameters used to create gameplay::Terrain
	*/
	int LodQuality;
	int TextureScale;
	int BoundingBox;
	int minHeight;
	int maxHeight;
	int patchSize;
	int skirtSize;
	
	/*
	resolution of paged terrains
	*/
	int tilesResolution;

	/*
	resolution of a terrain
	*/
	int heightFieldResolution;

	/*
	distance required to load a terrain
	*/
	float distanceLoad;
	
	/*
	distance required to unload a terrain
	*/
	float distanceUnload;
	
	/*
	the maximum distance at which a terrain can be rendered
	*/
	float distanceMaxRender;
	
	/*
	the maximum distance at which an object can be rendered
	*/
	float distanceMaxModelRender;

	/*
	if every blendmaps have been generated
	*/
	bool generatedBlendmaps;
	
	/*
	if every normalmaps have been generated
	*/
	bool generatedNormalmaps;
	
	/*
	if every heightmaps have been generated
	*/
	bool generatedHeightmaps;
	
	/*
	if every objects have been generated
	*/
	bool generatedObjects;

	/*
	if you want to draw the terrain in wireframe
	*/
	bool Debug;
};

/*used in a vector for the pager class*/
class CBufferZone
{
public:
	CBufferZone(gameplay::Vector3 pos, gameplay::Vector3 scale)
	{
		_pos = pos;
		_scale = scale;
		setLoaded(false);
	}
	/*return a terrain*/
	CTerrain * getObjectInside() { return object; }
	gameplay::Vector3 getPosition(){ return _pos; }
	gameplay::Vector3 getScale(){ return _scale; }

	/*used to check against during "pagingcheck" or "render"*/
	void setLoaded(bool state) { loaded = state; }

	/*define a terrain*/
	void setObjectInside(CTerrain * terrain) { object = terrain; }
	
	/*used to check against during "pagingcheck" or "render"*/
	bool isLoaded() { return loaded; }
private:
	/*terrain position and scale*/
	gameplay::Vector3 _pos, _scale;
	
	/*a terrain*/
	CTerrain * object;

	/*a pointer to the scene created in TerrainToolMain.cpp*/
	gameplay::Scene* _Scene;
	
	/*used to check against during "pagingcheck" or "render"*/
	bool loaded = false;
};

class TerrainPager
{
public:
	/*it would define the pager and compute the heightfields positions and storing in zonelist vector*/
	TerrainPager(STerrainParameters param, gameplay::Scene* scen);
	
	/*it will render objects and terrain based on the distance between the camera and the object*/
	void render();

	/*it's used to define if a terrain should be created or deleted*/
	void PagingCheck();

	/*it load a terrain
	* @param int pointing to the Z positioning in the zonelist[][] vector
	* @param int pointing to the X positioning in the zonelist[][] vector
	*/
	void loadTerrain(int, int);

	/*it load a terrain
	* @param int pointing to the Z positioning in the zonelist[][] vector
	* @param int pointing to the X positioning in the zonelist[][] vector
	*/
	void removeTerrain(int, int);
	
	/*remove all terrains*/
	void removeTerrains();

	/*recreate the zonelist vector by recomputing the position of each heightfields in heightFieldList vector*/
	void computePositions();

	/*populate heightfieldlist vector by loading PNG files*/
	void loadHeightfields();

	/**
	* Remove all objects stored in obj vector and delete the objects
	**/
	void removeObjects();
	
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

	/*storing heightfields position to conditionally create terrains*/
	std::vector<std::vector<CBufferZone*> > zoneList;
	
	/*used to define alot of parameters for the pager*/
	STerrainParameters Param;
private:
	/*a pointer to the scene created in "TerrainToolMain.cpp"*/
	gameplay::Scene * _Scene;

};
