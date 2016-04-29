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

struct PagerParameters
{
	/**
	* Folder name to access blendmaps,normalmaps, objects position, heightmaps and terrains information
	**/
	char * tmpFolder;

	/**
	* Parameters used to create gameplay::Terrain.
	**/
	int lodQuality, textureScale, boundingBox, minHeight, maxHeight, patchSize, skirtSize;
	
	std::string terrainMaterialPath;

	gameplay::Vector3 scale;
	
	/**
	* Resolution of the zone.(E.G : a resolution of 4 would equal to a total of 16 terrains)
	**/
	int zoneResolution;

	/**
	* Resolution of a terrain.
	**/
	int heightFieldResolution;

	/**
	* Distance required to load,unload or render a terrain.
	* Distance required for an object to be rendered.
	**/
	float distanceTerrainLoad, distanceTerrainUnload, distanceTerrainMaxRender, distanceMaxModelRender;

	/**
	* If every blendmaps,Heightmaps, Normalmaps or Objects have been encoded into files.
	* It's used to verify if a file need to be encoded before creating a terrain.
	**/
	bool generatedBlendmaps, generatedHeightmaps, generatedNormalmaps, generatedObjects;

	/*
	if you want to draw the terrain in wireframe
	*/
	bool debug;
};

/**
* Used in "zonelist" vector for the pager class
**/
class Zone
{
public:
	Zone(gameplay::Vector3 pos, gameplay::Vector3 scale)
	{
		_pos = pos;
		_scale = scale;
		setLoaded(false);
	}
	/**
	* Return a terrain
	**/
	CTerrain * getObjectInside() { return object; }
	gameplay::Vector3 getPosition(){ return _pos; }
	gameplay::Vector3 getScale(){ return _scale; }

	/**
	* used to check against during "pagingCheck" or "render"
	**/
	void setLoaded(bool state) { loaded = state; }

	/**
	* define a terrain
	**/
	void setObjectInside(CTerrain * terrain) { object = terrain; }

	/**
	* used to check against during "pagingcheck" or "render"
	**/
	bool isLoaded() { return loaded; }

	/**
	* Heightfields used for editing and loading gameplay::Terrain.
	**/
	gameplay::HeightField* heightField;

	/**
	* the first vector is either 0 or 1(blendmap 0 or 1)
	* the second vector is the blendmap image data
	**/
	std::vector<std::vector<unsigned char> > blendMaps;

	/**
	* the normalmap image data stacked into a vector
	**/
	std::vector<unsigned char> normalMap;

	/**
	* A vector stack of 3 string defining the texture location used by the blendmaps used by the terrain
	**/
	std::vector<std::string> texturesLocation;
private:
	/**
	* terrain position and scale
	**/
	gameplay::Vector3 _pos, _scale;

	/**
	* the terrain
	**/
	CTerrain * object;

	/**
	* used to check against during "pagingcheck" or "render"
	**/
	bool loaded = false;
};

/**
* The Pager will take care of loading,unloading and rendering various objects based on distance
**/
class Pager
{
public:
	/**
	* it would define the pager and compute the heightfields positions and storing in zonelist vector
	**/
	Pager(PagerParameters, gameplay::Scene*);

	/**
	it will render objects and terrain based on the distance between the camera and the object
	**/
	void render();

	/*it's used to define if a terrain should be created or deleted*/
	void pagingCheck();

	/*it load a terrain
	* @param int pointing to the Z positioning in the zonelist[][] vector
	* @param int pointing to the X positioning in the zonelist[][] vector
	*/
	void loadTerrain(int, int);

	/*it remove a terrain
	* @param int pointing to the Z positioning in the zonelist[][] vector
	* @param int pointing to the X positioning in the zonelist[][] vector
	*/
	void removeTerrain(int, int);
	
	/**
	* Remove all terrains.
	**/
	void removeTerrains();

	/**
	Recreate the "zonelist" vector by calculating the position of each heightfields in the heightFieldList vector.
	**/	
	void computePositions();

	/**
	* Remove all objects stored in "objects" vector and release the objects from the gameplay Scene.
	**/
	void removeObjects();
	
	/**
	* Reload all renderable terrains.
	**/
	void reloadTerrains();

	/**
	* Reload given terrains.
	*
	* @param vector position to current loaded terrains
	* @return void
	**/
	void reload(std::vector<int>);

	/**
	* Find terrain under cursor.
	*
	* @param cursor position
	* @param window resolution
	* @return int pointing to the terrain in the "loadedTerrains" vector
	**/
	int findTerrain(Vector2, Vector2);

	/**
	* Find the nearest terrain from a vector3 world position
	*
	* @param Vector3 coordinates
	* @return int pointing to the terrain in the "loadedTerrains" vector
	**/
	int findTerrain(Vector3);

	/**
	* Used to define alot of parameters for the pager.
	**/
	PagerParameters parameters;

	/**
	* position X defined by world space position
	* position Z defined by world space position
	* vector stack of gameplay::Vector3 defining the number of objects on one terrain
	**/
	std::vector<std::vector<std::vector<Vector3*> > > objectsPosition;

	/**
	* position X defined by world space position
	* position Z defined by world space position
	* vector stack of gameplay::Node used in rendering and used as a reference to delete
	**/
	std::vector<std::vector<std::vector<Node*> > > objects;
	std::vector<char*> objectsFilename;

	/**
	* Loaded terrains used for rendering and editing.
	**/
	std::vector<gameplay::Terrain*> loadedTerrains;
	std::vector<gameplay::HeightField*> loadedHeightfields;

	/**
	* Used to store heightfields,blendmaps and normalmaps if a terrain need to be created or modified.
	**/
	std::vector<std::vector<Zone*> > zoneList;
private:

	/**
	* Pointer to the gameplay::Scene created in "Main.cpp".
	**/
	gameplay::Scene * _Scene;

};
