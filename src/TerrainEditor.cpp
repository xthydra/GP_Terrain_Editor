/*
	GP_Terrain_Editor - GamePlay3D Unoffical Third Party Terrain Editor
	Copyright (C) 2015 Anthony Belisle <xt.hydra@gmail.com>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"

/**
* return the heightfield height based on gameplay implementation
*
* @return float
**/
float returnHeight(float x, float z, gameplay::HeightField * field, int heightFieldResolution, int scaleXZ)
{
	// Calculate the correct x, z position relative to the heightfield data.
	float cols = field->getColumnCount();
	float rows = field->getRowCount();

	GP_ASSERT(cols > 0);
	GP_ASSERT(rows > 0);

	Matrix _inverseWorldMatrix = Matrix::identity();

	// Apply local scale and invert
	_inverseWorldMatrix.scale(scaleXZ);
	_inverseWorldMatrix.invert();

	// Since the specified coordinates are in world space, we need to use the 
	// inverse of our world matrix to transform the world x,z coords back into
	// local heightfield coordinates for indexing into the height array.
	Vector3 v = _inverseWorldMatrix * Vector3(x, 0.0f, z);
	x = v.x + (cols - 1) * 0.5f;
	z = v.z + (rows - 1) * 0.5f;

	// Get the unscaled height value from the HeightField
	float height = field->getHeight(x, z);

	// Apply world scale
	//TODO : i don't think i'm suposed to multiply it by the terrain scale
	height *= scaleXZ;

	return height;

}

void TerrainEditor::aligningTerrainsVertexes(std::vector<std::vector<HeightField*> > heightFields, size_t heightFieldSize, int scaleXZ)
{
	for (size_t i = 0; i < heightFields.size(); i++)
	{
		for (size_t j = 0; j < heightFields.size(); j++)
		{
			for (size_t g = 0; g < heightFieldSize; g++)
			{
				if (j != (heightFields.size() - 1))
				{
					int x, z, x2, z2, x3, z3, x4, z4;
					//primary height 1
					x =  ((heightFieldSize*(heightFieldSize)) / 2);
					z = -(((heightFieldSize)* heightFieldSize) / 2);
					z += ((heightFieldSize)* g);

					//primary height 2
					x2 = -((heightFieldSize*(heightFieldSize)) / 2);
					z2 = -(((heightFieldSize)* heightFieldSize) / 2);
					z2 += ((heightFieldSize)* g);

					//secondary height 1
					x3 = ((heightFieldSize*(heightFieldSize)) / 2);
					z3 = -(((heightFieldSize)* heightFieldSize) / 2);
					z3 += ((heightFieldSize)* g) + heightFieldSize;

					//primary height 2
					x4 = -((heightFieldSize*(heightFieldSize)) / 2);
					z4 = -(((heightFieldSize)* heightFieldSize) / 2);
					z4 += ((heightFieldSize)* g) + heightFieldSize;

					//get heights
					float pHeight = (returnHeight(x, z, heightFields[i][j], heightFieldSize, scaleXZ) / heightFieldSize);//primary height 1
					float pHeight2 = (returnHeight(x2, z2, heightFields[i][j + 1], heightFieldSize, scaleXZ) / heightFieldSize);//primary height 2

					float sHeight = (returnHeight(x3, z3, heightFields[i][j], heightFieldSize, scaleXZ) / heightFieldSize);//secondary height 1
					float sHeight2 = (returnHeight(x4, z4, heightFields[i][j + 1], heightFieldSize, scaleXZ) / heightFieldSize);//secondary height 2

					//remove decimals
					float tmpHeight = ((pHeight + pHeight2) + (sHeight + sHeight2)) / 4;
					double tmpHeight2 = std::floor(tmpHeight * 100000000.) / 100000000.;


					//aligning vertexes
					float * fieldArray = heightFields[i][j]->getArray();
					fieldArray[(heightFieldSize * g) + heightFieldSize - 1] = tmpHeight2;//X

					fieldArray = heightFields[i][j + 1]->getArray();
					fieldArray[heightFieldSize * g] = tmpHeight2;//-X
				}
				if (i != (heightFields.size() - 1))
				{
					int x, z, x2, z2, x3, z3, x4, z4;
					//primary height 1
					x = -((heightFieldSize*(heightFieldSize)) / 2);
					x += ((heightFieldSize)* g);
					z = ((heightFieldSize*(heightFieldSize)) / 2);

					//primary height 2
					x2 = -((heightFieldSize*(heightFieldSize)) / 2);
					x2 += ((heightFieldSize)* g);
					z2 = -((heightFieldSize*(heightFieldSize)) / 2);

					//secondary height 1
					x3 = -((heightFieldSize*(heightFieldSize)) / 2);
					x3 += ((heightFieldSize)* g) + heightFieldSize;
					z3 = ((heightFieldSize*(heightFieldSize)) / 2);

					//primary height 2
					x4 = -((heightFieldSize*(heightFieldSize)) / 2);
					x4 += ((heightFieldSize)* g) + heightFieldSize;
					z4 = -((heightFieldSize*(heightFieldSize)) / 2);

					//get heights
					float pHeight = (returnHeight(x, z, heightFields[i][j], heightFieldSize, scaleXZ) / heightFieldSize);//primary height 1
					float pHeight2 = (returnHeight(x2, z2, heightFields[i + 1][j], heightFieldSize, scaleXZ) / heightFieldSize);//primary height 2

					float sHeight = (returnHeight(x3, z3, heightFields[i][j], heightFieldSize, scaleXZ) / heightFieldSize);//secondary height 1
					float sHeight2 = (returnHeight(x4, z4, heightFields[i + 1][j], heightFieldSize, scaleXZ) / heightFieldSize);//secondary height 2

					//remove decimals for precision
					float tmpHeight = ((pHeight + pHeight2) + (sHeight + sHeight2)) / 4;
					double tmpHeight2 = std::floor(tmpHeight * 100000000.) / 100000000.;

					//aligning vertexes
					float * fieldArray = heightFields[i][j]->getArray();
					fieldArray[(heightFieldSize * (heightFieldSize - 1)) + g] = tmpHeight2; //Z

					fieldArray = heightFields[i + 1][j]->getArray();
					fieldArray[g] = tmpHeight2;//-Z
				}
			}
		}
	}
}

std::vector<int> TerrainEditor::flatten(BoundingSphere selectionRing, int scaleXZ, int scaleY, std::vector<gameplay::Terrain*> terrains, std::vector<HeightField*> heightFields)
{
//#define OLD
#ifdef OLD
	std::vector<int> fields;

	size_t heightFieldSize = heightFields[0]->getRowCount();

	//TODO: i obviously messed this up, it's flattening 1 heightfield at a time and it isnt making a cumulative height of every heightfields

	for (size_t i = 0; i < terrains.size(); i++)
	{
		Vector3 terrainPos = terrains[i]->getNode()->getTranslationWorld();
		//terrain bounding box world space
		int boxSize = terrains[i]->getBoundingBox().max.x;
		BoundingBox worldBox(
			(terrainPos.z - boxSize),
			terrains[i]->getBoundingBox().min.y,
			(terrainPos.z - boxSize),
			(terrainPos.x + boxSize),
			terrains[i]->getBoundingBox().max.y,
			(terrainPos.z + boxSize)
			);

		if (worldBox.intersects(selectionRing))
		{
			fields.push_back(i);
			float * heights = heightFields[i]->getArray();

			std::vector<int> vertexesModified;
			std::vector<int> vertexesModifiedHeight;

			for (size_t x = 0; x < heightFieldSize; x++)
			{
				for (size_t z = 0; z < heightFieldSize; z++)
				{
					//vertexe position in world space
					Vector3 vertexePosition((x*scaleXZ) + (terrainPos.x - boxSize),
						heights[x + (z * heightFieldSize)] * scaleY,
						(z*scaleXZ) + (terrainPos.z - boxSize));


					//check against distance and apply strength
					float dist = vertexePosition.distance(selectionRing.center);

					if (dist <= selectionRing.radius)
					{
						vertexesModified.push_back(x + (z * heightFieldSize));
						vertexesModifiedHeight.push_back(heights[x + (z * heightFieldSize)]);
					}
				}
			}
			float cumulativeHeight=0;
			for (size_t f = 0; f < vertexesModified.size(); f++)
			{
				cumulativeHeight += vertexesModifiedHeight[f];
			}
			cumulativeHeight /= vertexesModified.size();
			for (size_t f = 0; f< vertexesModified.size(); f++)
			{
				heights[vertexesModified[f]] += (cumulativeHeight - heights[vertexesModified[f]]) * 0.03;
			}
		}
	}
	return fields;
#else
	std::vector<int> fields;

	size_t heightFieldSize = heightFields[0]->getRowCount();

	//TODO: i obviously messed this up, it's flattening 1 heightfield at a time and it isnt making a cumulative height of every heightfields

	std::vector<std::vector<int>> vertexesModified;
	std::vector<std::vector<int>> vertexesModifiedHeight;

	vertexesModified.resize(heightFields.size());
	vertexesModifiedHeight.resize(heightFields.size());

	for (size_t i = 0; i < terrains.size(); i++)
	{
		Vector3 terrainPos = terrains[i]->getNode()->getTranslationWorld();
		//terrain bounding box world space
		int boxSize = terrains[i]->getBoundingBox().max.x;
		BoundingBox worldBox(
			(terrainPos.z - boxSize),
			terrains[i]->getBoundingBox().min.y,
			(terrainPos.z - boxSize),
			(terrainPos.x + boxSize),
			terrains[i]->getBoundingBox().max.y,
			(terrainPos.z + boxSize)
			);

		if (worldBox.intersects(selectionRing))
		{
			fields.push_back(i);
			float * heights = heightFields[i]->getArray();

			for (size_t x = 0; x < heightFieldSize; x++)
			{
				for (size_t z = 0; z < heightFieldSize; z++)
				{
					//vertexe position in world space
					Vector3 vertexePosition((x*scaleXZ) + (terrainPos.x - boxSize),
						heights[x + (z * heightFieldSize)] * scaleY,
						(z*scaleXZ) + (terrainPos.z - boxSize));


					//check against distance and apply strength
					float dist = vertexePosition.distance(selectionRing.center);

					if (dist <= selectionRing.radius)
					{
						vertexesModified[i].push_back(x + (z * heightFieldSize));
						vertexesModifiedHeight[i].push_back(heights[x + (z * heightFieldSize)]);
					}
				}
			}
		}
	}
	float cumulativeHeight = 0;
	int vertsModified = 0;
	for (size_t i = 0; i < vertexesModified.size(); i++)
	{
		if(vertexesModified[i].size() > 0)
		{
			for (size_t f = 0; f < vertexesModified[i].size(); f++)
			{
				cumulativeHeight += vertexesModifiedHeight[i][f];
				vertsModified++;
			}
		}
	}
	cumulativeHeight /= vertsModified;
	for (size_t i = 0; i < vertexesModified.size(); i++)
	{
		if (vertexesModified[i].size() > 0)
		{
			float * heights = heightFields[i]->getArray();
			for (size_t f = 0; f < vertexesModified.size(); f++)
			{
				heights[vertexesModified[i][f]] += (cumulativeHeight - heights[vertexesModified[i][f]]) * 0.03;
			}
		}
	}
	return fields;
#endif
}

std::vector<int> TerrainEditor::lower(BoundingSphere selectionRing, int scaleXZ, int scaleY, std::vector<gameplay::Terrain*> terrains, std::vector<HeightField*> heightFields)
{
	std::vector<int> fields;

	size_t heightFieldSize = heightFields[0]->getRowCount();

	for (size_t i = 0; i < terrains.size(); i++)
	{
		Vector3 terrainPos = terrains[i]->getNode()->getTranslationWorld();
		//terrain bounding box world space
		int boxSize = terrains[i]->getBoundingBox().max.x;
		BoundingBox worldBox(
			(terrainPos.z - boxSize),
			terrains[i]->getBoundingBox().min.y,
			(terrainPos.z - boxSize),
			(terrainPos.x + boxSize),
			terrains[i]->getBoundingBox().max.y,
			(terrainPos.z + boxSize)
			);

		if (worldBox.intersects(selectionRing))
		{
			fields.push_back(i);
			float * heights = heightFields[i]->getArray();
			for (size_t x = 0; x < heightFieldSize; x++)
			{
				for (size_t z = 0; z < heightFieldSize; z++)
				{
					//vertexe position in world space
					Vector3 vertexePosition((x*scaleXZ) + (terrainPos.x - boxSize),
						heights[x + (z * heightFieldSize)] * scaleY,
						(z*scaleXZ) + (terrainPos.z - boxSize));


					//check against distance and apply strength
					float dist = vertexePosition.distance(selectionRing.center);

					if (dist <= selectionRing.radius)
					{
						heights[x + (z * heightFieldSize)] -= (selectionRing.radius - dist) * 0.01;
					}
				}
			}
		}
	}
	return fields;
}
std::vector<int> TerrainEditor::raise(BoundingSphere selectionRing, int scaleXZ, int scaleY, std::vector<gameplay::Terrain*> terrains, std::vector<HeightField*> heightFields)
{
	std::vector<int> fields;

	size_t heightFieldSize = heightFields[0]->getRowCount();

	for (size_t i = 0; i < terrains.size(); i++)
	{
		Vector3 terrainPos=terrains[i]->getNode()->getTranslationWorld();
		//terrain bounding box world space
		int boxSize=terrains[i]->getBoundingBox().max.x;
		BoundingBox worldBox(
			(terrainPos.z - boxSize),
			terrains[i]->getBoundingBox().min.y,
			(terrainPos.z - boxSize),
			(terrainPos.x + boxSize),
			terrains[i]->getBoundingBox().max.y,
			(terrainPos.z + boxSize)
			);
		
		if (worldBox.intersects(selectionRing))
		{
			fields.push_back(i);
			float * heights = heightFields[i]->getArray();
			for (size_t x = 0; x < heightFieldSize; x++)
			{
				for (size_t z = 0; z < heightFieldSize; z++)
				{
					//vertexe position in world space
					Vector3 vertexePosition((x*scaleXZ)+(terrainPos.x - boxSize),
						heights[x + (z * heightFieldSize)]*scaleY,
						(z*scaleXZ) + (terrainPos.z - boxSize));


					//check against distance and apply strength
					float dist = vertexePosition.distance(selectionRing.center);

					if(dist <= selectionRing.radius)
					{
						heights[x + (z * heightFieldSize)] += (selectionRing.radius - dist) * 0.01;
					}
				}
			}
		}
	}
	return fields;
}

std::vector<int> TerrainEditor::smooth(BoundingSphere selectionRing, int scaleXZ, int scaleY, std::vector<gameplay::Terrain*> terrains, std::vector<HeightField*> heightFields)
{
	//TODO : obviously isnt meant to be used right now
	/*
	std::vector<int> fields;

	size_t heightFieldSize = heightFields[0]->getRowCount();

	for (size_t i = 0; i < terrains.size(); i++)
	{
		Vector3 terrainPos = terrains[i]->getNode()->getTranslationWorld();
		//terrain bounding box world space
		int boxSize = terrains[i]->getBoundingBox().max.x;
		BoundingBox worldBox(
			(terrainPos.z - boxSize),
			terrains[i]->getBoundingBox().min.y,
			(terrainPos.z - boxSize),
			(terrainPos.x + boxSize),
			terrains[i]->getBoundingBox().max.y,
			(terrainPos.z + boxSize)
			);

		if (worldBox.intersects(selectionRing))
		{
			fields.push_back(i);
			float * heights = heightFields[i]->getArray();
			for (size_t x = 0; x < heightFieldSize; x++)
			{
				for (size_t z = 0; z < heightFieldSize; z++)
				{
					//vertexe position in world space
					Vector3 vertexePosition((x*scaleXZ) + (terrainPos.x - boxSize),
						heights[x + (z * heightFieldSize)] * scaleY,
						(z*scaleXZ) + (terrainPos.z - boxSize));


					//check against distance and apply strength
					float dist = vertexePosition.distance(selectionRing.center);

					if (dist <= selectionRing.radius)
					{
						heights[x + (z * heightFieldSize)] += (selectionRing.radius - dist) * 0.01;
					}
				}
			}
		}
	}
	return fields;
	*/
	return std::vector<int>();
}