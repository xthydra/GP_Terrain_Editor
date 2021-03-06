﻿/*
	GP_Terrain_Editor - GamePlay3D Unoffical Third Party Terrain Editor
	Copyright (C) 2016 Anthony Belisle <xt.hydra@gmail.com>

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

#include "Main.h"

void TerrainEditor::aligningTerrainsVertexes(std::vector<std::vector<HeightField*> > heightFields)
{
	size_t heightFieldSize = heightFields[0][0]->getColumnCount();

	for (size_t i = 0; i < heightFields.size(); i++)
	{
		for (size_t j = 0; j < heightFields.size(); j++)
		{
			for (size_t g = 0; g < heightFieldSize; g++)
			{
				if (j != (heightFields.size() - 1))
				{
					float pHeight = (heightFields[i][j]->getHeight(heightFieldSize, g));//primary height 1
					float pHeight2 = (heightFields[i][j + 1]->getHeight(0, g));//primary height 2

					float sHeight = (heightFields[i][j]->getHeight(heightFieldSize-1, g));//secondary height 1
					float sHeight2 = (heightFields[i][j + 1]->getHeight(1, g));//secondary height 2

					//TODO : something is wrong with the way it's smoothing out
					//as if it favor a greater number when smoothing and when you press align again it keep moving
					//the terrains extremities shouldnt move at the second time align is called if the terrains didnt change at least not as much as it's doing right now

					//float tmpHeight = ((pHeight + pHeight2) + (sHeight + sHeight2)) / 4;
					//float tmpHeight = ((((pHeight + pHeight2) * 0.5) + ((sHeight + sHeight2) * 0.5)) * 0.5);
					//float tmpHeight = ((((pHeight + pHeight2) * 0.5) + sHeight + sHeight2) / 3);
					float tmpHeight = ((sHeight + sHeight2)* 0.5);
					//double tmpHeight2 = std::floor(tmpHeight * 100000000.) / 100000000.;

					//aligning vertexes
					float * fieldArray = heightFields[i][j]->getArray();
					fieldArray[(heightFieldSize * g) + heightFieldSize - 1] = tmpHeight;//X

					fieldArray = heightFields[i][j + 1]->getArray();
					fieldArray[heightFieldSize * g] = tmpHeight;//-X
				}
				if (i != (heightFields.size() - 1))
				{
					float pHeight = heightFields[i][j]->getHeight(g, heightFieldSize);//primary height 1
					float pHeight2 = heightFields[i + 1][j]->getHeight(g, 0);//primary height 2

					float sHeight = heightFields[i][j]->getHeight(g, heightFieldSize-1);//secondary height 1
					float sHeight2 = heightFields[i + 1][j]->getHeight(g, 1);//secondary height 2

					//float tmpHeight = ((pHeight + pHeight2) + (sHeight + sHeight2)) / 4;
					//float tmpHeight = ((((pHeight + pHeight2) * 0.5) + ((sHeight + sHeight2) * 0.5)) * 0.5);
					//float tmpHeight = ((((pHeight + pHeight2) * 0.5) + sHeight + sHeight2) / 3);
					float tmpHeight = ((sHeight + sHeight2)* 0.5);
					//double tmpHeight2 = std::floor(tmpHeight * 100000000.) / 100000000.;

					//aligning vertexes
					float * fieldArray = heightFields[i][j]->getArray();
					fieldArray[(heightFieldSize * (heightFieldSize - 1)) + g] = tmpHeight; //Z

					fieldArray = heightFields[i + 1][j]->getArray();
					fieldArray[g] = tmpHeight;//-Z
				}
			}
		}
	}
}

std::vector<int> TerrainEditor::flatten(BoundingSphere selectionRing,
										int scaleXZ,
										int scaleY,
										std::vector<gameplay::Terrain*> terrains,
										std::vector<HeightField*> heightFields)
{
	std::vector<int> fields;

	size_t heightFieldSize = heightFields[0]->getRowCount();

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
			(terrainPos.x - boxSize),
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
			for (size_t f = 0; f < vertexesModified[i].size(); f++)
			{
				heights[vertexesModified[i][f]] += (cumulativeHeight - heights[vertexesModified[i][f]]) * 0.03;
			}
		}
	}
	return fields;
}

std::vector<int> TerrainEditor::lower(BoundingSphere selectionRing,
									  int scaleXZ,
									  int scaleY,
									  std::vector<gameplay::Terrain*> terrains,
									  std::vector<HeightField*> heightFields)
{
	std::vector<int> fields;

	size_t heightFieldSize = heightFields[0]->getRowCount();

	for (size_t i = 0; i < terrains.size(); i++)
	{
		Vector3 terrainPos = terrains[i]->getNode()->getTranslationWorld();
		//terrain bounding box world space
		int boxSize = terrains[i]->getBoundingBox().max.x;
		BoundingBox worldBox(
			(terrainPos.x - boxSize),
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

					float brushStr = 1.5;//1.0 - 1.5

					//float str = dist / selectionRing.radius;
					double str = (((selectionRing.radius*brushStr) - dist) / selectionRing.radius);

					if (dist <= selectionRing.radius)
					{
						//heights[x + (z * heightFieldSize)] -= (selectionRing.radius - dist) * 0.01;
						heights[x + (z * heightFieldSize)] -= str;
					}
				}
			}
		}
	}
	return fields;
}
std::vector<int> TerrainEditor::raise(BoundingSphere selectionRing,
									  int scaleXZ,
									  int scaleY,
									  std::vector<gameplay::Terrain*> terrains,
									  std::vector<HeightField*> heightFields)
{
	std::vector<int> fields;

	size_t heightFieldSize = heightFields[0]->getRowCount();

	for (size_t i = 0; i < terrains.size(); i++)
	{
		Vector3 terrainPos=terrains[i]->getNode()->getTranslationWorld();
		//terrain bounding box world space
		int boxSize=terrains[i]->getBoundingBox().max.x;
		BoundingBox worldBox(
			(terrainPos.x - boxSize),
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

std::vector<int> TerrainEditor::smooth(BoundingSphere selectionRing,
									   int scaleXZ,
									   int scaleY,
									   std::vector<gameplay::Terrain*> terrains,
									   std::vector<HeightField*> heightFields)
{
	//TODO : isnt meant to be used right now
	std::vector<int> fields;

	size_t heightFieldSize = heightFields[0]->getRowCount();

	std::vector<std::vector<int>> vertexesModified;
	std::vector<std::vector<int>> vertexesModifiedHeight;

	vertexesModified.resize(heightFields.size());
	vertexesModifiedHeight.resize(heightFields.size());

	for (size_t i = 0; i < terrains.size(); i++)
	{
		Vector3 terrainPos = terrains[i]->getNode()->getTranslationWorld();
		//terrain bounding box in world space
		int boxSize = terrains[i]->getBoundingBox().max.x;
		BoundingBox worldBox(
			(terrainPos.x - boxSize),
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

			bool beyondX=false, belowX=false, beyondZ=false, belowZ=false;
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
						//x+1
						Vector3 vertexePosition2(((x + 1)*scaleXZ) + (terrainPos.x - boxSize),
							heights[(x + 1) + (z * heightFieldSize)] * scaleY,
							(z*scaleXZ) + (terrainPos.z - boxSize));
						if (vertexePosition2.x > (terrainPos.x + boxSize))
						{
							beyondX = true;
						}

						//z+1
						Vector3 vertexePosition3((x*scaleXZ) + (terrainPos.x - boxSize),
							heights[x + ((z + 1) * heightFieldSize)] * scaleY,
							((z + 1)*scaleXZ) + (terrainPos.z - boxSize));

						if (vertexePosition3.z > (terrainPos.z + boxSize))
						{
							beyondZ = true;
						}

						//x-1
						Vector3 vertexePosition4(((x - 1)*scaleXZ) + (terrainPos.x - boxSize),
							heights[(x - 1) + (z * heightFieldSize)] * scaleY,
							(z*scaleXZ) + (terrainPos.z - boxSize));

						if (vertexePosition4.x < (terrainPos.x - boxSize))
						{
							belowX = true;
						}

						//z-1
						Vector3 vertexePosition5((x*scaleXZ) + (terrainPos.x - boxSize),
							heights[x + ((z - 1) * heightFieldSize)] * scaleY,
							((z - 1)*scaleXZ) + (terrainPos.z - boxSize));

						if (vertexePosition2.z < (terrainPos.z - boxSize))
						{
							belowZ = true;
						}

						//z+1,x+1
						Vector3 vertexePosition6(((x + 1)*scaleXZ) + (terrainPos.x - boxSize),
							heights[(x + 1) + ((z + 1) * heightFieldSize)] * scaleY,
							((z + 1)*scaleXZ) + (terrainPos.z - boxSize));

						if (vertexePosition3.z > (terrainPos.z + boxSize))
						{
							beyondZ = true;
						}
						else if (vertexePosition2.x > (terrainPos.x + boxSize))
						{
							beyondX = true;
						}

						//z-1,x-1
						Vector3 vertexePosition7(((x - 1)*scaleXZ) + (terrainPos.x - boxSize),
							heights[(x - 1) + ((z - 1) * heightFieldSize)] * scaleY,
							((z - 1)*scaleXZ) + (terrainPos.z - boxSize));

						if (vertexePosition3.z < (terrainPos.z - boxSize))
						{
							belowZ = true;
						}
						else if (vertexePosition2.x < (terrainPos.x - boxSize))
						{
							belowX = true;
						}

						//z-1,x+1
						Vector3 vertexePosition8(((x + 1)*scaleXZ) + (terrainPos.x - boxSize),
							heights[(x + 1) + ((z - 1) * heightFieldSize)] * scaleY,
							((z - 1)*scaleXZ) + (terrainPos.z - boxSize));

						if (vertexePosition3.z < (terrainPos.z - boxSize))
						{
							belowZ = true;
						}
						else if (vertexePosition2.x > (terrainPos.x + boxSize))
						{
							beyondX = true;
						}

						//z+1,x-1
						Vector3 vertexePosition9(((x - 1)*scaleXZ) + (terrainPos.x - boxSize),
							heights[(x - 1) + ((z + 1) * heightFieldSize)] * scaleY,
							((z + 1)*scaleXZ) + (terrainPos.z - boxSize));

						if (vertexePosition3.z > (terrainPos.z + boxSize))
						{
							beyondZ = true;
						}
						else if (vertexePosition2.x < (terrainPos.x - boxSize))
						{
							belowX = true;
						}

						vertexesModified[i].push_back(x + (z * heightFieldSize));
						vertexesModifiedHeight[i].push_back(heights[x + (z * heightFieldSize)]);
					}
				}
			}
			if (belowX)
			{
				printf("below X pos");
			}
			else if (beyondX)
			{
				printf("beyond X pos");
			}
			else if (belowZ)
			{
				printf("below X pos");
			}
			else if (beyondZ)
			{
				printf("beyond Z pos");
			}
		}
	}
	return fields;
}

std::vector<std::vector<Vector3>> TerrainEditor::paint(std::vector<std::vector<unsigned char>> blendmap1,
													   std::vector<std::vector<unsigned char>> blendmap2,
													   std::vector<Vector3> fieldsPos,
													   int selectedTex,
													   Vector2 pos,
													   int blendmapRes,
													   int radius,
													   bool draw)
{
	std::vector<std::vector<Vector3>> modified;
	modified.resize(2);
	for (size_t i = 0; i < blendmap1.size(); i++)
	{
		bool bBlend1 = false, bBlend2 = false;
		for (size_t x = 0; x < blendmapRes; x++)
		{
			for (size_t z = 0; z < blendmapRes; z++)
			{
				Vector2 pixel = Vector2(((fieldsPos[i].x - (blendmapRes*0.5)) + x), 
										((fieldsPos[i].z - (blendmapRes*0.5)) + z));

				float dist = pos.distance(pixel);

				if(dist < radius)
				{
					int k = 4 * x + (z * blendmapRes * 4);

					double strNeg = (dist / radius);
					double strPlus = ((radius - dist) / radius);
					float brushStr = 1;//minimum 1// maximum ~1.5
					strPlus += brushStr;

					if (selectedTex == 0)
					{
						if (draw)
						{
							bBlend1 = true;
							bBlend2 = true;

							//blendmap1[--]
							//blendmap2[--]

							blendmap1[i][k] *= strNeg;
							blendmap1[i][k+1] *= strNeg;
							blendmap1[i][k+2] *= strNeg;
							blendmap1[i][k + 3] = ((blendmap1[i][k] + blendmap1[i][k + 1] + blendmap1[i][k + 2]) / 3);

							blendmap2[i][k] *= strNeg;
							blendmap2[i][k + 1] *= strNeg;
							blendmap2[i][k + 2] *= strNeg;
							blendmap2[i][k + 3] = ((blendmap2[i][k] + blendmap2[i][k + 1] + blendmap2[i][k + 2]) / 3);
						}
						// you cannot remove the texture one because it's not a blendmap
						//tho you can replace it by using blendmaps but it doesn't follow the logical expression behind "removing a texture"
						//i would have to choose between 2 blendmaps to "replace" instead of "remove", very counter intuitive
					}
					if (selectedTex == 1)
					{
						if (draw)
						{
							bBlend1 = true;
							bBlend2 = true;

							//blendmap1[++]
							//blendmap2[--]

							int height = (((255 - (blendmap1[i][k] + 1)) * 0.5) * strPlus);
							if (height + blendmap1[i][k] > 255) { blendmap1[i][k] = 255; }
							else { blendmap1[i][k] += height; }

							height = (((255 - (blendmap1[i][k + 1] + 1)) * 0.5) * strPlus);
							if (height + blendmap1[i][k + 1] > 255) { blendmap1[i][k + 1] = 255; }
							else { blendmap1[i][k + 1] += height; }

							height = (((255 - (blendmap1[i][k + 2] + 1)) * 0.5) * strPlus);
							if (height + blendmap1[i][k + 2] > 255) { blendmap1[i][k + 2] = 255; }
							else { blendmap1[i][k + 2] += height; }

							blendmap1[i][k + 3] = ((blendmap1[i][k] + blendmap1[i][k + 1] + blendmap1[i][k + 2]) / 3);

							blendmap2[i][k] *= strNeg;
							blendmap2[i][k + 1] *= strNeg;
							blendmap2[i][k + 2] *= strNeg;
							blendmap2[i][k + 3] = ((blendmap2[i][k] + blendmap2[i][k + 1] + blendmap2[i][k + 2]) / 3);
						}
						else
						{
							bBlend1 = true;

							//blendmap1[--]

							blendmap1[i][k] *= strNeg;
							blendmap1[i][k + 1] *= strNeg;
							blendmap1[i][k + 2] *= strNeg;

							blendmap1[i][k + 3] = ((blendmap1[i][k] + blendmap1[i][k + 1] + blendmap1[i][k + 2]) / 3);
						}
					}
					if (selectedTex == 2)
					{
						if (draw)
						{
							bBlend1 = true;
							bBlend2 = true;

							//blendmap1[--]
							//blendmap2[++]

							int height = (((255 - (blendmap2[i][k] + 1)) * 0.5) * strPlus);
							if (height + blendmap2[i][k] > 255) { blendmap2[i][k] = 255; }
							else { blendmap2[i][k] += height; }

							height = (((255 - (blendmap2[i][k + 1] + 1)) * 0.5) * strPlus);
							if (height + blendmap2[i][k + 1] > 255) { blendmap2[i][k + 1] = 255; }
							else { blendmap2[i][k + 1] += height; }

							height = (((255 - (blendmap2[i][k + 2] + 1)) * 0.5) * strPlus);
							if (height + blendmap2[i][k + 2] > 255) { blendmap2[i][k + 2] = 255; }
							else { blendmap2[i][k + 2] += height; }

							blendmap2[i][k + 3] = ((blendmap2[i][k] + blendmap2[i][k + 1] + blendmap2[i][k + 2]) / 3);

							blendmap1[i][k] *= strNeg;
							blendmap1[i][k + 1] *= strNeg;
							blendmap1[i][k + 2] *= strNeg;
							blendmap1[i][k + 3] = ((blendmap1[i][k] + blendmap1[i][k + 1] + blendmap1[i][k + 2]) / 3);
						}
						else
						{
							bBlend2 = true;

							//blendmap2[--]

							blendmap2[i][k] *= strNeg;
							blendmap2[i][k + 1] *= strNeg;
							blendmap2[i][k + 2] *= strNeg;
							blendmap2[i][k + 3] = ((blendmap2[i][k] + blendmap2[i][k + 1] + blendmap2[i][k + 2]) / 3);
						}
					}
				}
			}
		}
		//used to specify which terrains to reload if a modification have occured
		if (bBlend1 == true)
		{
			modified[0].push_back(fieldsPos[i]);
			blend1.push_back(blendmap1[i]);
		}
		if (bBlend2 == true)
		{
			modified[1].push_back(fieldsPos[i]);
			blend2.push_back(blendmap2[i]);
		}
	}
	return modified;
}