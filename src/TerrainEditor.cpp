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
float returnHeight(float x, float z, gameplay::HeightField * field, int heightFieldResolution)
{
	// Calculate the correct x, z position relative to the heightfield data.
	float cols = field->getColumnCount();
	float rows = field->getRowCount();

	GP_ASSERT(cols > 0);
	GP_ASSERT(rows > 0);

	Matrix _inverseWorldMatrix = Matrix::identity();

	// Apply local scale and invert
	_inverseWorldMatrix.scale(heightFieldResolution);
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
	height *= heightFieldResolution;

	return height;

}

float distanceFromCenter(float x, float z, float centerx, float centerz) {
	return sqrtf(powf(x - centerx, 2.0f) + powf(z - centerz, 2.0f));
}

/**
* Used to map from heightmap coordinates to real world coordinates.
**/
Matrix getInverseWorldMatrix(gameplay::Terrain * _terrain, int heightFieldResolution)
{
	gameplay::Matrix _inverseWorldMatrix;
	_inverseWorldMatrix.set(_terrain->getNode()->getWorldMatrix());

	// Apply local scale and invert
	_inverseWorldMatrix.scale(heightFieldResolution);
	_inverseWorldMatrix.invert();

	return _inverseWorldMatrix;
}

/**
* Helper method to compute the average height for a circle in the terrain.
*
* @param x x coordinate for the center of the circle.
* @param z z coordinate for the center of the circle.
* @param scale the radius of the circle
* @return float
**/
float average(float x,
			  float z,
			  float scale,
			  gameplay::Terrain* terrain,
			  size_t _heightFieldSize,
			  HeightField* _heightField)
{
	//reverse the heightfield position by dividing by scaling then divide by heightfield resolution(or vice versa im not sure lol)
	int fieldX = (terrain->getNode()->getTranslationWorld().x / _heightFieldSize) / (_heightFieldSize - 1);
	int fieldY = (terrain->getNode()->getTranslationWorld().z / _heightFieldSize) / (_heightFieldSize - 1);

	float cols = _heightField->getColumnCount();
	float rows = _heightField->getRowCount();
	float *usedHeights = _heightField->getArray();
	size_t i, j;

	GP_ASSERT(cols > 0);
	GP_ASSERT(rows > 0);

	// Since the specified coordinates are in world space, we need to use the 
	// inverse of our world matrix to transform the world x,z coords back into
	// local heightfield coordinates for indexing into the height array.
	Vector3 v = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(x, 0.0f, z);
	Vector3 s = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(scale, 0.0f, 0.0f);

	float localx = v.x + (cols - 1) * 0.5f;
	float localz = v.z + (rows - 1) * 0.5f;
	float localscale = s.x;

	float average = 0.0f;
	size_t count = 0;

	for (i = 0; i < _heightFieldSize; i++) {
		for (j = 0; j < _heightFieldSize; j++) {
			float dist = distanceFromCenter((float)i, (float)j, localx, localz);

			float strength = localscale - dist;

			if (strength > 0) {
				average += usedHeights[i + (j * _heightFieldSize)];
				count++;
			}
		}
	}

	if (count == 0) {
		count = 1;
	}
	return average / count;
}

bool flatten(float Faverage,
			 size_t _heightFieldSize,
			 int tmpX2,
			 int tmpZ2,
			 int scale,
			 Terrain * terrain,
			 HeightField * heightfield)
{
	float cols = heightfield->getColumnCount();
	float rows = heightfield->getRowCount();
	float *usedHeights = heightfield->getArray();
	size_t g, k;

	GP_ASSERT(cols > 0);
	GP_ASSERT(rows > 0);

	// Since the specified coordinates are in world space, we need to use the 
	// inverse of our world matrix to transform the world x,z coords back into
	// local heightfield coordinates for indexing into the height array.
	Vector3 v = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(tmpX2, 0.0f, tmpZ2);
	Vector3 s = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(scale, 0.0f, 0.0f);

	float localx = v.x + (cols - 1) * 0.5f;
	float localz = v.z + (rows - 1) * 0.5f;
	float localscale = s.x;

	bool modified = false;
	for (g = 0; g < _heightFieldSize; g++)
	{
		for (k = 0; k < _heightFieldSize; k++)
		{
			float dist = distanceFromCenter((float)g, (float)k, localx, localz);

			float strength = localscale - dist;

			if (strength > 0)
			{
				modified = true;
				usedHeights[g + (k * _heightFieldSize)] = Faverage;
			}
		}
	}
	return modified;
}

bool lower(size_t _heightFieldSize,
		   int tmpX2,
		   int tmpZ2,
		   int scale,
		   Terrain * terrain,
		   HeightField * heightfield)
{
	float cols = heightfield->getColumnCount();
	float rows = heightfield->getRowCount();
	float *usedHeights = heightfield->getArray();
	size_t g, k;

	GP_ASSERT(cols > 0);
	GP_ASSERT(rows > 0);

	// Since the specified coordinates are in world space, we need to use the 
	// inverse of our world matrix to transform the world x,z coords back into
	// local heightfield coordinates for indexing into the height array.
	Vector3 v = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(tmpX2, 0.0f, tmpZ2);
	Vector3 s = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(scale, 0.0f, 0.0f);

	float localx = v.x + (cols - 1) * 0.5f;
	float localz = v.z + (rows - 1) * 0.5f;
	float localscale = s.x;

	bool modified = false;
	for (g = 0; g < _heightFieldSize; g++)
	{
		for (k = 0; k < _heightFieldSize; k++)
		{
			float dist = distanceFromCenter((float)g, (float)k, localx, localz);

			float strength = localscale - dist;

			if (strength > 0)
			{
				modified = true;
				usedHeights[g + (k * _heightFieldSize)] -= strength;
			}
		}
	}
	return modified;
}

bool raise(size_t _heightFieldSize,
		   int tmpX2,
		   int tmpZ2,
		   int scale,
		   Terrain * terrain,
		   HeightField * heightfield)
{
	float cols = heightfield->getColumnCount();
	float rows = heightfield->getRowCount();
	float *usedHeights = heightfield->getArray();
	size_t g, k;

	GP_ASSERT(cols > 0);
	GP_ASSERT(rows > 0);

	// Since the specified coordinates are in world space, we need to use the 
	// inverse of our world matrix to transform the world x,z coords back into
	// local heightfield coordinates for indexing into the height array.
	Vector3 v = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(tmpX2, 0.0f, tmpZ2);
	Vector3 s = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(scale, 0.0f, 0.0f);

	float localx = v.x + (cols - 1) * 0.5f;
	float localz = v.z + (rows - 1) * 0.5f;
	float localscale = s.x;

	bool modified = false;
	for (g = 0; g < _heightFieldSize; g++)
	{
		for (k = 0; k < _heightFieldSize; k++)
		{
			float dist = distanceFromCenter((float)g, (float)k, localx, localz);

			float strength = localscale - dist;

			if (strength > 0)
			{
				modified = true;
				usedHeights[g + (k * _heightFieldSize)] += strength;
			}
		}
	}
	return modified;
}

bool smooth(size_t _heightFieldSize,
		    int tmpX2,
			int tmpZ2,
			int scale,
			Terrain * terrain,
			HeightField * heightfield)
{

	float cols = heightfield->getColumnCount();
	float rows = heightfield->getRowCount();
	float *usedHeights = heightfield->getArray();
	size_t i, j, repeats;
	size_t iminus, iplus, jminus, jplus;

	GP_ASSERT(cols > 0);
	GP_ASSERT(rows > 0);

	// Since the specified coordinates are in world space, we need to use the 
	// inverse of our world matrix to transform the world x,z coords bacj into
	// local heightfield coordinates for indexing into the height array.
	Vector3 v = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(tmpX2, 0.0f, tmpZ2);
	Vector3 s = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(scale, 0.0f, 0.0f);

	float localx = v.x + (cols - 1) * 0.5f;
	float localz = v.z + (rows - 1) * 0.5f;
	float localscale = s.x;

	bool modified = false;
	for (repeats = 0; repeats < 2; repeats++)
	{
		for (i = 0; i < _heightFieldSize; i++)
		{
			for (j = 0; j < _heightFieldSize; j++)
			{
				float dist = distanceFromCenter((float)i, (float)j, localx, localz);
				float strength = localscale - dist;

				if (strength > 0)
				{
					modified = true;
					iminus = i - 1;
					iplus = i + 1;
					jminus = j - 1;
					jplus = j + 1;
					if (iminus >= _heightFieldSize)
					{
						iminus = 0;
					}
					if (jminus >= _heightFieldSize)
					{
						jminus = 0;
					}	
					if (iplus >= _heightFieldSize)
					{
						iplus = _heightFieldSize - 1;
					}
					if (jplus >= _heightFieldSize)
					{
						jplus = _heightFieldSize - 1;
					}

					usedHeights[i + (j * _heightFieldSize)] = (
						usedHeights[iminus + (jminus * _heightFieldSize)] +
						usedHeights[i + (jminus * _heightFieldSize)] +
						usedHeights[iplus + (jminus * _heightFieldSize)] +
						usedHeights[iminus + (j * _heightFieldSize)] +
						usedHeights[i + (j * _heightFieldSize)] +
						usedHeights[iplus + (j * _heightFieldSize)] +
						usedHeights[iminus + (jplus * _heightFieldSize)] +
						usedHeights[i + (jplus * _heightFieldSize)] +
						usedHeights[iplus + (jplus * _heightFieldSize)]) / 9.0f;
				}
			}
		}
	}	
	return modified;
}

int setXPos(bool xpos,int size)
{
	if (!xpos){ return size; }
	if (xpos){ return (2*size); }
}

int setZPos(bool zpos, int size)
{
	if (!zpos){ return size; }
	if (zpos){ return (2 * size); }
}

bool smooth2(size_t _heightFieldSize,
			 int tmpX2,
			 int tmpZ2,
			 int scale,
			 bool xpos,
			 bool zpos,
			 Terrain * terrain,
			 HeightField * heightfield,
			 HeightField * heightfield2)
{

	float cols = heightfield->getColumnCount();
	float rows = heightfield->getRowCount();
	float *usedHeights = heightfield->getArray();
	float *usedHeights2 = heightfield2->getArray();
	size_t i, j, repeats;
	size_t iminus, iplus, jminus, jplus;

	GP_ASSERT(cols > 0);
	GP_ASSERT(rows > 0);

	// Since the specified coordinates are in world space, we need to use the 
	// inverse of our world matrix to transform the world x,z coords bacj into
	// local heightfield coordinates for indexing into the height array.
	Vector3 v = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(tmpX2, 0.0f, tmpZ2);
	Vector3 s = getInverseWorldMatrix(terrain, _heightFieldSize) * Vector3(scale, 0.0f, 0.0f);

	float localx = v.x + (cols - 1) * 0.5f;
	float localz = v.z + (rows - 1) * 0.5f;
	float localscale = s.x;

	bool modified = false;

	for (repeats = 0; repeats < 2; repeats++)
	{
		for (i = 0; i < setZPos(zpos, _heightFieldSize); i++)
		{	
			for (j = 0; j < setXPos(xpos,_heightFieldSize); j++)
			{
				//conditional iteration up to 2 terrain
				//cannot process 3 terrains only pair number in a conditional environement

				//i must make sure the vertexes variables are callen to be modified within a condition

				//if the Xpos is true then size will be 256


				// i think i need to nest the proccess for each kind of possibilities

				float dist = distanceFromCenter((float)i, (float)j, localx, localz);
				float strength = localscale - dist;

				if (strength > 0)
				{
					int vert1,
						vert2,
						vert3,
						vert4,
						vert5,
						vert6,
						vert7,
						vert8,
						vert9;

					iminus = i - 1;
					iplus = i + 1;
					jminus = j - 1;
					jplus = j + 1;

					/*if (iminus >= _heightFieldSize)
					{
						iminus = 0;
					}
					if (jminus >= _heightFieldSize)
					{
						jminus = 0;
					}
					if (iplus >= _heightFieldSize)
					{
						iplus = _heightFieldSize - 1;
					}
					if (jplus >= _heightFieldSize)
					{
						jplus = _heightFieldSize - 1;
					}




					if (iminus >= _heightFieldSize && !zpos)
					{
						iminus = 0;
					}
					if (jminus >= _heightFieldSize && !xpos)
					{
						jminus = 0;
					}
					if (iplus >= _heightFieldSize && zpos)
					{
						iplus = _heightFieldSize - 1;
					}
					if (jplus >= _heightFieldSize && xpos)
					{
						jplus = _heightFieldSize - 1;
					}*/ 

					if (xpos)
					{

					}
					if (zpos)
					{

					}


					usedHeights[i + (j * _heightFieldSize)] = (
						usedHeights[vert1] +//1
						usedHeights[vert2] +//2
						usedHeights[vert3] +//3
						usedHeights[vert4] +//4
						usedHeights[vert5] +//5
						usedHeights[vert6] +//6
						usedHeights[vert7] +//7
						usedHeights[vert8] +//8
						usedHeights[vert9]) / 9.0f;//9


					/*					usedHeights[i + (j * _heightFieldSize)] = (
						usedHeights[iminus + (jminus * _heightFieldSize)] +//1
						usedHeights[i + (jminus * _heightFieldSize)] +//2
						usedHeights[iplus + (jminus * _heightFieldSize)] +//3
						usedHeights[iminus + (j * _heightFieldSize)] +//4
						usedHeights[i + (j * _heightFieldSize)] +//5
						usedHeights[iplus + (j * _heightFieldSize)] +//6
						usedHeights[iminus + (jplus * _heightFieldSize)] +//7
						usedHeights[i + (jplus * _heightFieldSize)] +//8
						usedHeights[iplus + (jplus * _heightFieldSize)]) / 9.0f;//9*/

					modified = true;
				}
			}
		}
	}
	return modified;
}

void TerrainEditor::aligningTerrainsVertexes(std::vector<std::vector<HeightField*> > _heightFields, size_t _heightFieldSize)
{
	for (size_t i = 0; i < _heightFields.size(); i++)
	{
		for (size_t j = 0; j < _heightFields.size(); j++)
		{
			for (size_t g = 0; g < _heightFieldSize; g++)
			{
				if (j != (_heightFields.size() - 1))
				{
					int x, z, x2, z2, x3, z3, x4, z4;
					//primary height 1
					x =  ((_heightFieldSize*(_heightFieldSize)) / 2);
					z = -(((_heightFieldSize)* _heightFieldSize) / 2);
					z += ((_heightFieldSize)* g);

					//primary height 2
					x2 = -((_heightFieldSize*(_heightFieldSize)) / 2);
					z2 = -(((_heightFieldSize)* _heightFieldSize) / 2);
					z2 += ((_heightFieldSize)* g);

					//secondary height 1
					x3 = ((_heightFieldSize*(_heightFieldSize)) / 2);
					z3 = -(((_heightFieldSize)* _heightFieldSize) / 2);
					z3 += ((_heightFieldSize)* g) + _heightFieldSize;

					//primary height 2
					x4 = -((_heightFieldSize*(_heightFieldSize)) / 2);
					z4 = -(((_heightFieldSize)* _heightFieldSize) / 2);
					z4 += ((_heightFieldSize)* g) + _heightFieldSize;

					//get heights
					float pHeight = (returnHeight(x, z, _heightFields[i][j], _heightFieldSize) / _heightFieldSize);//primary height 1
					float pHeight2 = (returnHeight(x2, z2, _heightFields[i][j + 1], _heightFieldSize) / _heightFieldSize);//primary height 2

					float sHeight = (returnHeight(x3, z3, _heightFields[i][j], _heightFieldSize) / _heightFieldSize);//secondary height 1
					float sHeight2 = (returnHeight(x4, z4, _heightFields[i][j + 1], _heightFieldSize) / _heightFieldSize);//secondary height 2

					//remove decimals
					float tmpHeight = ((pHeight + pHeight2) + (sHeight + sHeight2)) / 4;
					double tmpHeight2 = std::floor(tmpHeight * 100000000.) / 100000000.;


					//aligning vertexes
					float * fieldArray = _heightFields[i][j]->getArray();
					fieldArray[(_heightFieldSize * g) + _heightFieldSize - 1] = tmpHeight2;//X

					fieldArray = _heightFields[i][j + 1]->getArray();
					fieldArray[_heightFieldSize * g] = tmpHeight2;//-X
				}
				if (i != (_heightFields.size() - 1))
				{
					int x, z, x2, z2, x3, z3, x4, z4;
					//primary height 1
					x = -((_heightFieldSize*(_heightFieldSize)) / 2);
					x += ((_heightFieldSize)* g);
					z = ((_heightFieldSize*(_heightFieldSize)) / 2);

					//primary height 2
					x2 = -((_heightFieldSize*(_heightFieldSize)) / 2);
					x2 += ((_heightFieldSize)* g);
					z2 = -((_heightFieldSize*(_heightFieldSize)) / 2);

					//secondary height 1
					x3 = -((_heightFieldSize*(_heightFieldSize)) / 2);
					x3 += ((_heightFieldSize)* g) + _heightFieldSize;
					z3 = ((_heightFieldSize*(_heightFieldSize)) / 2);

					//primary height 2
					x4 = -((_heightFieldSize*(_heightFieldSize)) / 2);
					x4 += ((_heightFieldSize)* g) + _heightFieldSize;
					z4 = -((_heightFieldSize*(_heightFieldSize)) / 2);

					//get heights
					float pHeight = (returnHeight(x, z, _heightFields[i][j], _heightFieldSize) / _heightFieldSize);//primary height 1
					float pHeight2 = (returnHeight(x2, z2, _heightFields[i + 1][j], _heightFieldSize) / _heightFieldSize);//primary height 2

					float sHeight = (returnHeight(x3, z3, _heightFields[i][j], _heightFieldSize) / _heightFieldSize);//secondary height 1
					float sHeight2 = (returnHeight(x4, z4, _heightFields[i + 1][j], _heightFieldSize) / _heightFieldSize);//secondary height 2

					//remove decimals for precision
					float tmpHeight = ((pHeight + pHeight2) + (sHeight + sHeight2)) / 4;
					double tmpHeight2 = std::floor(tmpHeight * 100000000.) / 100000000.;

					//aligning vertexes
					float * fieldArray = _heightFields[i][j]->getArray();
					fieldArray[(_heightFieldSize * (_heightFieldSize - 1)) + g] = tmpHeight2; //Z

					fieldArray = _heightFields[i + 1][j]->getArray();
					fieldArray[g] = tmpHeight2;//-Z
				}
			}
		}
	}
}

std::vector<int> TerrainEditor::flattenTiles(float x, float z, float scale, std::vector<gameplay::Terrain*> terrains, int nearest, int _heightFieldSize, std::vector<HeightField*> _heightFields)
{
	float tmpx = x, tmpz = z;

	//reposition the ray hit as if the terrain underneath the ray was at position 0
	/*while (tmpx >= ((_heightFieldSize * (_heightFieldSize - 1)) * 0.5))
	{
		tmpx -= ((_heightFieldSize * (_heightFieldSize - 1)) * 0.5);
	}
	while (tmpz >= ((_heightFieldSize * (_heightFieldSize - 1)) * 0.5))
	{
		tmpz -= ((_heightFieldSize * (_heightFieldSize - 1)) * 0.5);
	}*/

	//bounding object used as if it was the selection ring
	Vector3 boundingObjectMin;
	Vector3 boundingObjectMax;

	boundingObjectMin.x = tmpx - scale;
	boundingObjectMin.z = tmpz - scale;

	boundingObjectMax.x = tmpx + scale;
	boundingObjectMax.z = tmpz + scale;

	std::vector<int> fields;

	bool atleast1 = false;
	for (size_t i = 0; i < terrains.size(); i++)
	{
		if (boundingObjectMin.x < terrains[i]->getBoundingBox().min.x)
		{
			if (terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x - (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMin.x - terrains[i]->getBoundingBox().min.x;
				float tmpX2 = (terrains[i]->getBoundingBox().max.x + distance) + scale;
				float tmpZ2 = tmpz;

				float Faverage1 = average(tmpx, tmpz, scale, terrains[nearest], _heightFieldSize, _heightFields[nearest]);
				float Faverage2 = average(tmpX2, tmpZ2, scale, terrains[i], _heightFieldSize, _heightFields[i]);
				float Faverage = ((Faverage1 + Faverage2) / 2);

				bool modified = flatten(Faverage, _heightFieldSize, tmpx, tmpz, scale, terrains[nearest], _heightFields[nearest]);

				if (modified == true)
				{
					fields.push_back(nearest);
				}
				
				modified = false;
				modified = flatten(Faverage, _heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);
				
				if (modified == true)
				{
					fields.push_back(i);
				}
				atleast1 = true;
			}
		}
		if (boundingObjectMin.z < terrains[i]->getBoundingBox().min.z)
		{
			if (terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z - (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMin.z - terrains[i]->getBoundingBox().min.z;
				float tmpZ2 = (terrains[i]->getBoundingBox().max.z + distance) + scale;
				float tmpX2 = tmpx;

				float Faverage1 = average(tmpx, tmpz, scale, terrains[nearest], _heightFieldSize, _heightFields[nearest]);
				float Faverage2 = average(tmpX2, tmpZ2, scale, terrains[i], _heightFieldSize, _heightFields[i]);
				float Faverage = ((Faverage1 + Faverage2) / 2);

				bool modified = flatten(Faverage, _heightFieldSize, tmpx, tmpz, scale, terrains[nearest], _heightFields[nearest]);

				if (modified == true)
				{
					fields.push_back(nearest);
				}

				modified = false;
				modified = flatten(Faverage, _heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
				atleast1 = true;
			}
		}
		if (boundingObjectMax.x > terrains[i]->getBoundingBox().max.x)
		{
			if (terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x + (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMax.x - terrains[i]->getBoundingBox().max.x;
				float tmpX2 = (terrains[i]->getBoundingBox().min.x + distance) - scale;
				float tmpZ2 = tmpz;

				float Faverage1 = average(tmpx, tmpz, scale, terrains[nearest], _heightFieldSize, _heightFields[nearest]);
				float Faverage2 = average(tmpX2, tmpZ2, scale, terrains[i], _heightFieldSize, _heightFields[i]);
				float Faverage = ((Faverage1 + Faverage2) / 2);

				bool modified = flatten(Faverage, _heightFieldSize, tmpx, tmpz, scale, terrains[nearest], _heightFields[nearest]);

				if (modified == true)
				{
					fields.push_back(nearest);
				}

				modified = false;
				modified = flatten(Faverage, _heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
				atleast1 = true;
			}
		}
		if (boundingObjectMax.z > terrains[i]->getBoundingBox().max.z)
		{
			if (terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z + (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMax.z - terrains[i]->getBoundingBox().max.z;
				float tmpZ2 = (terrains[i]->getBoundingBox().min.z + distance) - scale;
				float tmpX2 = tmpx;

				float Faverage1 = average(tmpx, tmpz, scale, terrains[nearest], _heightFieldSize, _heightFields[nearest]);
				float Faverage2 = average(tmpX2, tmpZ2, scale, terrains[i], _heightFieldSize, _heightFields[i]);
				float Faverage = ((Faverage1 + Faverage2) / 2);

				bool modified = flatten(Faverage, _heightFieldSize, tmpx, tmpz, scale, terrains[nearest], _heightFields[nearest]);

				if (modified == true)
				{
					fields.push_back(nearest);
				}

				modified = false;
				modified = flatten(Faverage, _heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
				atleast1 = true;
			}
		}
	}

	if (!atleast1)
	{
		float Faverage = average(tmpx, tmpz, scale, terrains[nearest], _heightFieldSize, _heightFields[nearest]);

		bool modified = flatten(Faverage, _heightFieldSize, tmpx, tmpz, scale, terrains[nearest], _heightFields[nearest]);

		if (modified == true)
		{
			fields.push_back(nearest);
		}
	}

	smoothTiles(x, z, scale, terrains, nearest,_heightFieldSize,_heightFields);

	return fields;


}
std::vector<int> TerrainEditor::lowerTiles(float x, float z, float scale, std::vector<gameplay::Terrain*> terrains, int nearest, int _heightFieldSize, std::vector<HeightField*> _heightFields)
{
	float tmpx = x, tmpz = z;

	//bounding object used as if it was the selection ring
	Vector3 boundingObjectMin;
	Vector3 boundingObjectMax;

	boundingObjectMin.x = tmpx - scale;
	boundingObjectMin.z = tmpz - scale;

	boundingObjectMax.x = tmpx + scale;
	boundingObjectMax.z = tmpz + scale;

	std::vector<int> fields;

	for (size_t i = 0; i < terrains.size(); i++)
	{
		if (boundingObjectMin.x < terrains[i]->getBoundingBox().min.x)
		{
			if (terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x - (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMin.x - terrains[i]->getBoundingBox().min.x;

				float tmpX2 = (terrains[i]->getBoundingBox().max.x + distance) + scale;

				float tmpZ2 = tmpz;

				bool modified = lower(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
		if (boundingObjectMin.z < terrains[i]->getBoundingBox().min.z)
		{
			if (terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z - (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMin.z - terrains[i]->getBoundingBox().min.z;

				float tmpZ2 = (terrains[i]->getBoundingBox().max.z + distance) + scale;

				float tmpX2 = tmpx;

				bool modified = lower(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
		if (boundingObjectMax.x > terrains[i]->getBoundingBox().max.x)
		{
			if (terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x + (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMax.x - terrains[i]->getBoundingBox().max.x;

				float tmpX2 = (terrains[i]->getBoundingBox().min.x + distance) - scale;

				float tmpZ2 = tmpz;

				bool modified = lower(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
		if (boundingObjectMax.z > terrains[i]->getBoundingBox().max.z)
		{
			if (terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z + (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMax.z - terrains[i]->getBoundingBox().max.z;

				float tmpZ2 = (terrains[i]->getBoundingBox().min.z + distance) - scale;

				float tmpX2 = tmpx;

				bool modified = lower(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
	}

	bool modified = lower(_heightFieldSize, tmpx, tmpz, scale, terrains[nearest], _heightFields[nearest]);

	if (modified == true)
	{
		fields.push_back(nearest);
	}

	return fields;
}

std::vector<int> TerrainEditor::raiseTiles(float x, float z, float scale, std::vector<gameplay::Terrain*> terrains, int nearest, int _heightFieldSize, std::vector<HeightField*> _heightFields)
{
	float tmpx = x, tmpz = z;

	//bounding object used as if it was the selection ring
	Vector3 boundingObjectMin;
	Vector3 boundingObjectMax;

	boundingObjectMin.x = tmpx - scale;
	boundingObjectMin.z = tmpz - scale;

	boundingObjectMax.x = tmpx + scale;
	boundingObjectMax.z = tmpz + scale;

	std::vector<int> fields;

	for (size_t i = 0; i < terrains.size(); i++)
	{
		if (boundingObjectMin.x < terrains[i]->getBoundingBox().min.x)
		{
			if (terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x - (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMin.x - terrains[i]->getBoundingBox().min.x;

				float tmpX2 = (terrains[i]->getBoundingBox().max.x + distance) + scale;

				float tmpZ2 = tmpz;

				bool modified = raise(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
		if (boundingObjectMin.z < terrains[i]->getBoundingBox().min.z)
		{
			if (terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z - (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMin.z - terrains[i]->getBoundingBox().min.z;

				float tmpZ2 = (terrains[i]->getBoundingBox().max.z + distance) + scale;

				float tmpX2 = tmpx;

				bool modified = raise(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
		if (boundingObjectMax.x > terrains[i]->getBoundingBox().max.x)
		{
			if (terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x + (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMax.x - terrains[i]->getBoundingBox().max.x;

				float tmpX2 = (terrains[i]->getBoundingBox().min.x + distance) - scale;

				float tmpZ2 = tmpz;

				bool modified = raise(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}	
			}
		}
		if (boundingObjectMax.z > terrains[i]->getBoundingBox().max.z)
		{
			if (terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z + (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMax.z - terrains[i]->getBoundingBox().max.z;

				float tmpZ2 = (terrains[i]->getBoundingBox().min.z + distance) - scale;

				float tmpX2 = tmpx;

				bool modified = raise(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
	}

	bool modified = raise(_heightFieldSize, tmpx, tmpz, scale, terrains[nearest], _heightFields[nearest]);

	if (modified == true)
	{
		fields.push_back(nearest);
	}

	return fields;
}

std::vector<int> TerrainEditor::smoothTiles(float x, float z, float scale, std::vector<gameplay::Terrain*> terrains, int nearest, int _heightFieldSize, std::vector<HeightField*> _heightFields)
{
	float tmpx = x, tmpz = z;

	//bounding object used as if it was the selection ring
	Vector3 boundingObjectMin;
	Vector3 boundingObjectMax;

	boundingObjectMin.x = tmpx - scale;
	boundingObjectMin.z = tmpz - scale;

	boundingObjectMax.x = tmpx + scale;
	boundingObjectMax.z = tmpz + scale;

	std::vector<int> fields;
	
	for (size_t i = 0; i < terrains.size(); i++)
	{
		if (boundingObjectMin.x < terrains[i]->getBoundingBox().min.x)
		{
			if (terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x - (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMin.x - terrains[i]->getBoundingBox().min.x;

				float tmpX2 = (terrains[i]->getBoundingBox().max.x + distance) + scale;

				float tmpZ2 = tmpz;

				bool modified = smooth(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
		if (boundingObjectMin.z < terrains[i]->getBoundingBox().min.z)
		{
			if (terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z - (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMin.z - terrains[i]->getBoundingBox().min.z;

				float tmpZ2 = (terrains[i]->getBoundingBox().max.z + distance) + scale;

				float tmpX2 = tmpx;

				bool modified = smooth(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
		if (boundingObjectMax.x > terrains[i]->getBoundingBox().max.x)
		{
			if (terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x + (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMax.x - terrains[i]->getBoundingBox().max.x;

				float tmpX2 = (terrains[i]->getBoundingBox().min.x + distance) - scale;

				float tmpZ2 = tmpz;

				bool modified = smooth(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
		if (boundingObjectMax.z > terrains[i]->getBoundingBox().max.z)
		{
			if (terrains[i]->getNode()->getTranslationWorld().z ==
				terrains[nearest]->getNode()->getTranslationWorld().z + (terrains[i]->getBoundingBox().max.x * 2)
				&&
				terrains[i]->getNode()->getTranslationWorld().x ==
				terrains[nearest]->getNode()->getTranslationWorld().x)
			{
				//raw distance beyond the boundary of the terrain
				//basicly how much far is the selection deep into that terrain
				int distance = boundingObjectMax.z - terrains[i]->getBoundingBox().max.z;

				float tmpZ2 = (terrains[i]->getBoundingBox().min.z + distance) - scale;

				float tmpX2 = tmpx;

				bool modified = smooth(_heightFieldSize, tmpX2, tmpZ2, scale, terrains[i], _heightFields[i]);

				if (modified == true)
				{
					fields.push_back(i);
				}
			}
		}
	}

	bool modified = smooth(_heightFieldSize, tmpx, tmpz, scale, terrains[nearest], _heightFields[nearest]);

	if (modified == true)
	{
		fields.push_back(nearest);
	}

	return fields;
}