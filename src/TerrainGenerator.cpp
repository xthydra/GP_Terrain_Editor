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

/**
* return the heightfield height based on gameplay implementation
*
* @return float
**/
float returnHeight(int scaleY,
				   float x,
				   float z,
				   gameplay::HeightField * field, size_t _heightFieldSize)
{
	// Calculate the correct x, z position relative to the heightfield data.
	float cols = field->getColumnCount();
	float rows = field->getRowCount();

	GP_ASSERT(cols > 0);
	GP_ASSERT(rows > 0);

	Matrix _inverseWorldMatrix = Matrix::identity();

	// Apply local scale and invert
	_inverseWorldMatrix.scale(_heightFieldSize);
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
	height *= scaleY;

	return height;

}

void calculateNormal(
	float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3,
	Vector3* normal)
{
	Vector3 E(x1, y1, z1);
	Vector3 F(x2, y2, z2);
	Vector3 G(x3, y3, z3);

	Vector3 P, Q;
	Vector3::subtract(F, E, &P);
	Vector3::subtract(G, E, &Q);

	Vector3::cross(Q, P, normal);
}

std::vector<std::vector<std::vector<Vector3*> > > 
	TerrainGenerator::generateObjectsPosition(Vector3 worldScale,
											  int scaleY,
											  int random,
											  size_t _heightFieldSize,
											  std::vector<std::vector<gameplay::HeightField *> >_heightFields,
											  Node * object)
{
	Node * tempObj = object->clone();
	std::vector<std::vector<std::vector<Vector3*> > > objectPos;
	std::vector<std::vector<std::vector<BoundingSphere> > > bounds;
	bounds.resize(_heightFields.size());
	objectPos.resize(_heightFields.size());
	for (size_t i = 0; i < _heightFields.size(); i++)
	{
		bounds[i].resize(_heightFields[i].size());
		objectPos[i].resize(_heightFields[i].size());
		for (size_t j = 0; j < _heightFields[i].size(); j++)
		{
			size_t x, z, k;
			float worldx, worldz, worldy;

			float worldminx = -(_heightFieldSize * (_heightFieldSize - 1) * 0.5);
			float worldminz = -(_heightFieldSize * (_heightFieldSize - 1) * 0.5);
			float worldmaxx = (_heightFieldSize * (_heightFieldSize - 1) * 0.5);
			float worldmaxz = (_heightFieldSize * (_heightFieldSize - 1) * 0.5);

			for (x = 0; x < _heightFieldSize; x++)
			{
				for (z = 0; z < _heightFieldSize; z++)
				{
					int r = rand() % 100;
					if (random > r)
					{
						k = x + (z * _heightFieldSize);

						worldx = ((float)x / (float)_heightFieldSize) * (worldmaxx - worldminx) + worldminx;
						worldz = ((float)z / (float)_heightFieldSize) * (worldmaxz - worldminz) + worldminz;

						worldy = returnHeight(scaleY, worldx, worldz, _heightFields[i][j], _heightFieldSize);

						worldy *= worldScale.y;

						Vector3 truePos(worldx + ((worldmaxx * 2) * j), worldy, worldz + ((worldmaxx * 2) * i));
						tempObj->setTranslation(truePos);

						bool push = false;

						if (objectPos[i][j].size() > 0)
						{
							for (size_t g = 0; g < objectPos[i][j].size(); g++)
							{
								if (tempObj->getBoundingSphere().intersects(bounds[i][j][g]) == false)
								{
									push = true;
								}
							}
						}
						if (objectPos[i][j].size() == 0)
						{
							bounds[i][j].push_back(tempObj->getBoundingSphere());
							objectPos[i][j].push_back(new Vector3(tempObj->getTranslationWorld()));
						}
						if (push)
						{
							bounds[i][j].push_back(tempObj->getBoundingSphere());
							objectPos[i][j].push_back(new Vector3(tempObj->getTranslationWorld()));
						}
					}
				}
			}
		}
	}
	SAFE_RELEASE(tempObj);
	return objectPos;
}

void TerrainGenerator::createRawHeightfields(int scaleY,
											 int maxHeight,
											 int boundingBox,
											 size_t heightfieldSize,
											 std::vector<std::vector<gameplay::HeightField *> > heightFields)
{
	// Generate a new tmp folder for the blend images.
#ifdef WIN32
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mktemp(tmpdir);
	_mkdir("res/tmp");
	_mkdir(tmpdir);
#else
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mkdtemp(tmpdir);
#endif
#define BIT16
#ifdef BIT16
	

	//i need vectors of each terrains max and min height instead of the difference between all terrain
	int terrainMinHeight=0;
	int terrainMaxHeight=0;
	std::vector<std::vector<std::vector<int> > > terrainMinMaxHeight;
	terrainMinMaxHeight.resize(heightFields.size());
	for (size_t i = 0; i < heightFields.size(); i++)
	{
		terrainMinMaxHeight[i].resize(heightFields[i].size());
		for (size_t j = 0; j < heightFields[i].size(); j++)
		{
			size_t x, z;

			float worldx, worldz, worldy;

			float worldminx = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldminz = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxx = (heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxz = (heightfieldSize * (heightfieldSize - 1) * 0.5);

			for (x = 0; x < heightfieldSize; x++)
			{
				for (z = 0; z < heightfieldSize; z++)
				{
					worldx = ((float)x / (float)heightfieldSize) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)heightfieldSize) * (worldmaxz - worldminz) + worldminz;

					worldy = returnHeight(scaleY, worldx, worldz, heightFields[i][j], heightfieldSize);

					// In the first pass we determine the min and max for the entire terrain.
					if (worldy < terrainMinHeight ) 
					{
						terrainMinHeight = worldy;
					}

					if (worldy > terrainMaxHeight ) 
					{
						terrainMaxHeight = worldy;
					}
				}
			}
			terrainMinMaxHeight[i][j].push_back(terrainMinHeight);
			terrainMinMaxHeight[i][j].push_back(terrainMaxHeight);
			terrainMaxHeight = 0;
			terrainMinHeight = 0;
		}
	}

	for (size_t i = 0; i < heightFields.size(); i++)
	{
		for (size_t j = 0; j < heightFields[i].size(); j++)
		{
			std::vector<__int16> raw;

			size_t x, z, k;
			float worldx, worldz, worldy;

			float worldminx = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldminz = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxx = (heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxz = (heightfieldSize * (heightfieldSize - 1) * 0.5);

			raw.resize(heightfieldSize * heightfieldSize);

			for (x = 0; x < heightfieldSize; x++)
			{
				for (z = 0; z < heightfieldSize; z++)
				{
#define TEST
#ifdef TEST
					k = x + (z * heightfieldSize);

					worldx = ((float)x / (float)heightfieldSize) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)heightfieldSize) * (worldmaxz - worldminz) + worldminz;

					worldy = returnHeight(scaleY, worldx, worldz, heightFields[i][j], heightfieldSize);

					bool modified = false;
					if (worldy > 0)
					{
						// i need to make sure that the negative value of minimal terrain height would be added to the position as a positive value
						worldy = (worldy - (terrainMinMaxHeight[i][j][0]));
						modified = true;
					}

					//the position need to be positive
					else if (worldy < 0 && modified == false)
					{
						//this doesnt work cuz it would still remain negative
						// and i need the value to become positive like
						// -3(worldy) + -4(minheight) = 1

						worldy = worldy + (-terrainMinMaxHeight[i][j][0]);
					}
					
					//i need pourcentage difference between worldy and maxheight
					double pourcentageofworldy = worldy / (terrainMinMaxHeight[i][j][1]- (terrainMinMaxHeight[i][j][0]));

					int scaledHeight = 65535 * pourcentageofworldy;

					if (scaledHeight > 65535)
					{
						scaledHeight = 65535;
					}
					if (scaledHeight < 0)
					{
						scaledHeight = 0;
					}

					raw[k] = scaledHeight;
#else
					k = x + (z * heightfieldSize);

					worldx = ((float)x / (float)heightfieldSize) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)heightfieldSize) * (worldmaxz - worldminz) + worldminz;

					worldy = returnHeight(scaleY, worldx, worldz, heightFields[i][j], heightfieldSize);

					//i need pourcentage difference between worldy and maxheight
					//to apply it to max __int16 value to scale it
					//pourcentageofworldy= worldy/maxheight
					//saveablevalue=65535 * pourcentageofworldy

					//it seem like sometime the world position Y is negative which output the terrain in a weird way
					// need a way to prevent the world position to be negative
					double pourcentageofworldy = worldy / (terrainmaxheight - terrainminheight);
					
					int scaledHeight = 65535 * pourcentageofworldy;

					if (scaledHeight > 65535)
					{
						scaledHeight = 65535;
					}
					if (scaledHeight < 0)
					{	
						scaledHeight = 0;
					}

					raw[k] = scaledHeight;
#endif
				}
			}

			//E.G //field-0-0.png
			std::string fieldName;

			fieldName += tmpdir;
			fieldName += "/";
			fieldName += "field-";
			fieldName += std::to_string(i);
			fieldName += "-";
			fieldName += std::to_string(j);
			fieldName += ".raw";

			int fd = open(fieldName.c_str(), O_CREAT | O_WRONLY | O_RAW);
			if (fd != -1)
			{
				//write(fd, raw.data(), raw.size()*2);
				write(fd, raw.data(), 32768);
				close(fd);
			}
		}
	}
#endif
//#define BIT8
#ifdef BIT8
	int terrainminheight=0;
	int terrainmaxheight=0;
	for (size_t i = 0; i < heightFields.size(); i++)
	{
		for (size_t j = 0; j < heightFields[i].size(); j++)
		{
			size_t x, z;

			float worldx, worldz, worldy;

			float worldminx = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldminz = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxx = (heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxz = (heightfieldSize * (heightfieldSize - 1) * 0.5);

			for (x = 0; x < heightfieldSize; x++)
			{
				for (z = 0; z < heightfieldSize; z++)
				{
					worldx = ((float)x / (float)heightfieldSize) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)heightfieldSize) * (worldmaxz - worldminz) + worldminz;

					worldy = returnHeight(scaleY, worldx, worldz, heightFields[i][j], heightfieldSize);

					// In the first pass we determine the min and max for the entire terrain.
					if (worldy < terrainminheight || (x == 0 && z == 0))
					{
					terrainminheight = worldy;
					}

					if (worldy > terrainmaxheight || (x == 0 && z == 0))
					{
						terrainmaxheight = worldy;
					}
				}
			}
		}
	}

	for (size_t i = 0; i < heightFields.size(); i++)
	{
		for (size_t j = 0; j < heightFields[i].size(); j++)
		{
			std::vector<__int8> raw;

			size_t x, z, k;
			float worldx, worldz, worldy;

			float worldminx = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldminz = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxx = (heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxz = (heightfieldSize * (heightfieldSize - 1) * 0.5);

			raw.resize(heightfieldSize * heightfieldSize);

			for (x = 0; x < heightfieldSize; x++)
			{
				for (z = 0; z < heightfieldSize; z++)
				{
					k = x + (z * heightfieldSize);

					worldx = ((float)x / (float)heightfieldSize) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)heightfieldSize) * (worldmaxz - worldminz) + worldminz;

					worldy = returnHeight(scaleY, worldx, worldz, heightFields[i][j], heightfieldSize);

					double pourcentageofworldy = worldy / (terrainmaxheight - terrainminheight);
					int scaledHeight = 254 * pourcentageofworldy;

					if (scaledHeight > 254)
					{
						scaledHeight = 254;
					}
					if (scaledHeight < 1)
					{
						scaledHeight = 1;
					}

					raw[k] = scaledHeight;
				}
			}

			//E.G //field-0-0.png
			std::string fieldName;

			fieldName += tmpdir;
			fieldName += "/";
			fieldName += "field-";
			fieldName += std::to_string(i);
			fieldName += "-";
			fieldName += std::to_string(j);
			fieldName += ".raw";

			int fd = open(fieldName.c_str(), O_CREAT | O_WRONLY | O_RAW);
			if (fd != -1)
			{
				write(fd, raw.data(), 16384);
				close(fd);
			}
		}
	}
#endif
#ifdef OLD
	for (size_t i = 0; i < heightFields.size(); i++)
	{
		for (size_t j = 0; j < heightFields[i].size(); j++)
		{
			std::vector<char> raw;

			size_t x, z, k;
			float worldx, worldz, worldy;

			float worldminx = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldminz = -(heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxx = (heightfieldSize * (heightfieldSize - 1) * 0.5);
			float worldmaxz = (heightfieldSize * (heightfieldSize - 1) * 0.5);

			raw.resize(heightfieldSize * heightfieldSize);

			for (x = 0; x < heightfieldSize; x++)
			{
				for (z = 0; z < heightfieldSize; z++)
				{
					k = x + (z * heightfieldSize);

					worldx = ((float)x / (float)heightfieldSize) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)heightfieldSize) * (worldmaxz - worldminz) + worldminz;

					worldy = returnHeight(scaleY, worldx, worldz, heightFields[i][j], heightfieldSize);

					worldy /= scaleY;

					if (worldy > 255)
					{
						worldy = 255;
					}
					if (worldy < 1)
					{
						worldy = 1;
					}

					raw[k] = worldy;
				}
			}

			//E.G //field-0-0.png
			std::string fieldName;

			fieldName += tmpdir;
			fieldName += "/";
			fieldName += "field-";
			fieldName += std::to_string(i);
			fieldName += "-";
			fieldName += std::to_string(j);
			fieldName += ".raw";

			std::ofstream rawField;
			rawField.open(fieldName.c_str(), std::ios::binary);

			rawField.write(raw.data(), raw.size());
			rawField.close();
		}
	}
#endif
}

void TerrainGenerator::createHeightmaps(int scaleY,
										size_t _heightFieldSize,
										std::vector<std::vector<gameplay::HeightField *> >_heightFields)
{
	// Generate a new tmp folder for the blend images.
#ifdef WIN32
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mktemp(tmpdir);
	_mkdir("res/tmp");
	_mkdir(tmpdir);
#else
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mkdtemp(tmpdir);
#endif
	size_t heightmapResolution = _heightFieldSize;
	for (size_t i = 0; i < _heightFields.size(); i++)
	{
		for (size_t j = 0; j < _heightFields[i].size(); j++)
		{
			std::vector<unsigned char> blend1;
			size_t x, z, k;
			float worldx, worldz, worldy;

			float worldminx = -(_heightFieldSize * (_heightFieldSize - 1) * 0.5);
			float worldminz = -(_heightFieldSize * (_heightFieldSize - 1) * 0.5);
			float worldmaxx = (_heightFieldSize * (_heightFieldSize - 1) * 0.5);
			float worldmaxz = (_heightFieldSize * (_heightFieldSize - 1) * 0.5);

			blend1.resize(heightmapResolution * heightmapResolution * 4);

			for (x = 0; x < heightmapResolution; x++)
			{
				for (z = 0; z < heightmapResolution; z++)
				{
					k = 4 * x + (z * heightmapResolution * 4);

					worldx = ((float)x / (float)heightmapResolution) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)heightmapResolution) * (worldmaxz - worldminz) + worldminz;

					worldy = returnHeight(scaleY, worldx, worldz, _heightFields[i][j], _heightFieldSize);

					//worldy /= _heightFieldSize;
					worldy /= scaleY;

					blend1[k] = worldy;
					blend1[k + 1] = worldy;
					blend1[k + 2] = worldy;
					blend1[k + 3] = worldy;
				}
			}

			//E.G //field-0-0.png
			std::string fieldName;

			fieldName += tmpdir;
			fieldName += "/";
			fieldName += "field-";
			fieldName += std::to_string(i);
			fieldName += "-";
			fieldName += std::to_string(j);
			fieldName += ".png";

			// Generate the png.
			lodepng::encode(fieldName.c_str(), blend1, heightmapResolution, heightmapResolution);
		}
	}
}

std::vector<std::vector<std::vector<unsigned char> > >
	TerrainGenerator::createNormalmaps(int scaleY,
									   size_t _heightFieldSize,
									   std::vector<std::vector<gameplay::HeightField *> >_heightFields)
{
	size_t normalMapResolution = _heightFieldSize;

	float worldminx = -(_heightFieldSize * (_heightFieldSize - 1) * 0.5);
	float worldminz = -(_heightFieldSize * (_heightFieldSize - 1) * 0.5);
	float worldmaxx = (_heightFieldSize * (_heightFieldSize - 1) * 0.5);
	float worldmaxz = (_heightFieldSize * (_heightFieldSize - 1) * 0.5);

	Vector2 scale((_heightFieldSize * (_heightFieldSize - 1) * 0.5) / (normalMapResolution), (_heightFieldSize * (_heightFieldSize - 1) * 0.5) / (normalMapResolution));

	struct Face
	{
		Vector3 normal1;
		Vector3 normal2;
	};

	std::vector<std::vector<std::vector<unsigned char> > > normalmaps;

	normalmaps.resize(_heightFields.size());
	for (size_t i = 0; i < _heightFields.size(); i++)
	{
		normalmaps[i].resize(_heightFields[i].size());
		for (size_t j = 0; j < _heightFields[i].size(); j++)
		{
			//std::vector<unsigned char> blend1;
			size_t x, z, k;
			float worldx, worldz;

			normalmaps[i][j].resize(normalMapResolution * normalMapResolution * 4);

			Face* faceNormals = new Face[(normalMapResolution)* (normalMapResolution)];

			for (x = 0; x < normalMapResolution; x++)
			{
				for (z = 0; z < normalMapResolution; z++)
				{
					worldx = ((float)x / (float)normalMapResolution) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)normalMapResolution) * (worldmaxz - worldminz) + worldminz;

					float topLeftHeight = (returnHeight(scaleY, worldx, worldz, _heightFields[i][j], _heightFieldSize));
					float bottomLeftHeight = (returnHeight(scaleY, worldx, worldz + _heightFieldSize, _heightFields[i][j], _heightFieldSize));
					float bottomRightHeight = (returnHeight(scaleY, worldx + _heightFieldSize, worldz + _heightFieldSize, _heightFields[i][j], _heightFieldSize));
					float topRightHeight = (returnHeight(scaleY, worldx + _heightFieldSize, worldz, _heightFields[i][j], _heightFieldSize));

					// Triangle 1
					calculateNormal(
						(float)x*scale.x, bottomLeftHeight, (float)(z + 1)*scale.y,
						(float)x*scale.x, topLeftHeight, (float)z*scale.y,
						(float)(x + 1)*scale.x, topRightHeight, (float)z*scale.y,
						&faceNormals[z*(normalMapResolution)+x].normal1);

					// Triangle 2
					calculateNormal(
						(float)x*scale.x, bottomLeftHeight, (float)(z + 1)*scale.y,
						(float)(x + 1)*scale.x, topRightHeight, (float)z*scale.y,
						(float)(x + 1)*scale.x, bottomRightHeight, (float)(z + 1)*scale.y,
						&faceNormals[z*(normalMapResolution)+x].normal2);
				}
			}

			for (x = 0; x < normalMapResolution; x++)
			{
				for (z = 0; z < normalMapResolution; z++)
				{
					// Smooth normals by taking an average for each vertex
					Vector3 normal;

					// Reset normal sum
					normal.set(0, 0, 0);

					if (x > 0)
					{
						if (z > 0)
						{
							// Top left
							normal.add(faceNormals[(z - 1)*(normalMapResolution)+(x - 1)].normal2);
						}

						if (z < (normalMapResolution))
						{
							// Bottom left
							normal.add(faceNormals[z*(normalMapResolution)+(x - 1)].normal1);
							normal.add(faceNormals[z*(normalMapResolution)+(x - 1)].normal2);
						}
					}

					if (x < (normalMapResolution - 1))
					{
						if (z > 0)
						{
							// Top right
							normal.add(faceNormals[(z - 1)*(normalMapResolution)+x].normal1);
							normal.add(faceNormals[(z - 1)*(normalMapResolution)+x].normal2);
						}

						if (z < (normalMapResolution))
						{
							// Bottom right
							normal.add(faceNormals[z*(normalMapResolution)+x].normal1);
						}
					}

					// We don't have to worry about weighting the normals by
					// the surface area of the triangles since a heightmap
					// guarantees that all triangles have the same surface area.
					normal.normalize();

					k = 4 * x + (z * normalMapResolution * 4);

					normalmaps[i][j][k] = (float)((normal.x + 1.0f) * 0.5f * 255.0f);
					normalmaps[i][j][k + 1] = (float)((normal.y + 1.0f) * 0.5f * 255.0f);
					normalmaps[i][j][k + 2] = (float)((normal.z + 1.0f) * 0.5f * 255.0f);
					normalmaps[i][j][k + 3] = ((float)((normal.x + 1.0f) * 0.5f * 255.0f) + (float)((normal.y + 1.0f) * 0.5f * 255.0f) + (float)((normal.z + 1.0f) * 0.5f * 255.0f)) / 3;

				}
			}
		}
	}
	return normalmaps;
#ifdef OLD_NORMAL
#ifdef WIN32
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mktemp(tmpdir);
	_mkdir("res/tmp");
	_mkdir(tmpdir);
#else
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mkdtemp(tmpdir);
#endif
	size_t normalMapResolution = _heightFieldSize;

	float worldminx = -(_heightFieldSize * (_heightFieldSize - 1) * 0.5);
	float worldminz = -(_heightFieldSize * (_heightFieldSize - 1) * 0.5);
	float worldmaxx = (_heightFieldSize * (_heightFieldSize - 1) * 0.5);
	float worldmaxz = (_heightFieldSize * (_heightFieldSize - 1) * 0.5);

	Vector2 scale((_heightFieldSize * (_heightFieldSize - 1) * 0.5) / (normalMapResolution), (_heightFieldSize * (_heightFieldSize - 1) * 0.5) / (normalMapResolution));

	struct Face
	{
		Vector3 normal1;
		Vector3 normal2;
	};

	for (size_t i = 0; i < _heightFields.size(); i++)
	{
		for (size_t j = 0; j < _heightFields[i].size(); j++)
		{
			std::vector<unsigned char> blend1;
			size_t x, z, k;
			float worldx, worldz;

			blend1.resize(normalMapResolution * normalMapResolution * 4);

			Face* faceNormals = new Face[(normalMapResolution)* (normalMapResolution)];

			for (x = 0; x < normalMapResolution; x++)
			{
				for (z = 0; z < normalMapResolution; z++)
				{
					worldx = ((float)x / (float)normalMapResolution) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)normalMapResolution) * (worldmaxz - worldminz) + worldminz;

					float topLeftHeight = (returnHeight(scaleY, worldx, worldz, _heightFields[i][j], _heightFieldSize));
					float bottomLeftHeight = (returnHeight(scaleY, worldx, worldz + _heightFieldSize, _heightFields[i][j], _heightFieldSize));
					float bottomRightHeight = (returnHeight(scaleY, worldx + _heightFieldSize, worldz + _heightFieldSize, _heightFields[i][j], _heightFieldSize));
					float topRightHeight = (returnHeight(scaleY, worldx + _heightFieldSize, worldz, _heightFields[i][j], _heightFieldSize));

					// Triangle 1
					calculateNormal(
						(float)x*scale.x, bottomLeftHeight, (float)(z + 1)*scale.y,
						(float)x*scale.x, topLeftHeight, (float)z*scale.y,
						(float)(x + 1)*scale.x, topRightHeight, (float)z*scale.y,
						&faceNormals[z*(normalMapResolution)+x].normal1);

					// Triangle 2
					calculateNormal(
						(float)x*scale.x, bottomLeftHeight, (float)(z + 1)*scale.y,
						(float)(x + 1)*scale.x, topRightHeight, (float)z*scale.y,
						(float)(x + 1)*scale.x, bottomRightHeight, (float)(z + 1)*scale.y,
						&faceNormals[z*(normalMapResolution)+x].normal2);
				}
			}

			for (x = 0; x < normalMapResolution; x++)
			{
				for (z = 0; z < normalMapResolution; z++)
				{
					// Smooth normals by taking an average for each vertex
					Vector3 normal;

					// Reset normal sum
					normal.set(0, 0, 0);

					if (x > 0)
					{
						if (z > 0)
						{
							// Top left
							normal.add(faceNormals[(z - 1)*(normalMapResolution)+(x - 1)].normal2);
						}

						if (z < (normalMapResolution))
						{
							// Bottom left
							normal.add(faceNormals[z*(normalMapResolution)+(x - 1)].normal1);
							normal.add(faceNormals[z*(normalMapResolution)+(x - 1)].normal2);
						}
					}

					if (x < (normalMapResolution - 1))
					{
						if (z > 0)
						{
							// Top right
							normal.add(faceNormals[(z - 1)*(normalMapResolution)+x].normal1);
							normal.add(faceNormals[(z - 1)*(normalMapResolution)+x].normal2);
						}

						if (z < (normalMapResolution))
						{
							// Bottom right
							normal.add(faceNormals[z*(normalMapResolution)+x].normal1);
						}
					}

					// We don't have to worry about weighting the normals by
					// the surface area of the triangles since a heightmap
					// guarantees that all triangles have the same surface area.
					normal.normalize();

					k = 4 * x + (z * normalMapResolution * 4);

					blend1[k] = (float)((normal.x + 1.0f) * 0.5f * 255.0f);
					blend1[k + 1] = (float)((normal.y + 1.0f) * 0.5f * 255.0f);
					blend1[k + 2] = (float)((normal.z + 1.0f) * 0.5f * 255.0f);
					blend1[k + 3] = ((float)((normal.x + 1.0f) * 0.5f * 255.0f) + (float)((normal.y + 1.0f) * 0.5f * 255.0f) + (float)((normal.z + 1.0f) * 0.5f * 255.0f)) / 3;

				}
			}

			//E.G //field-0-0.png
			std::string fieldName;

			fieldName += tmpdir;
			fieldName += "/";
			fieldName += "normalMap-";
			fieldName += std::to_string(i);
			fieldName += "-";
			fieldName += std::to_string(j);
			fieldName += ".png";

			// Generate the png.
			lodepng::encode(fieldName.c_str(), blend1, normalMapResolution, normalMapResolution);
		}
	}
#endif
}

std::vector<std::vector<std::vector<std::vector<unsigned char> > > >
	TerrainGenerator::createTiledTransparentBlendImages(int scaleY,
														float intensity1,
														float intensity2,
														float opacity1,
														float opacity2,
														size_t _heightFieldSize,
														std::vector<std::vector<gameplay::HeightField *> >_heightFields)
{
	size_t _blendResolution = _heightFieldSize;
	std::vector<
		std::vector<
			std::vector<
				std::vector<unsigned char> > > > blendmaps;

	float terrainminheight = 0.0f, terrainmaxheight = 0.0f;
	float worldminx = -((_heightFieldSize * (_heightFieldSize - 1)) * 0.5);
	float worldminz = -((_heightFieldSize * (_heightFieldSize - 1)) * 0.5);
	float worldmaxx = ((_heightFieldSize * (_heightFieldSize - 1)) * 0.5);
	float worldmaxz = ((_heightFieldSize * (_heightFieldSize - 1)) * 0.5);

	blendmaps.resize(_heightFields.size());
	for (size_t i = 0; i < _heightFields.size(); i++)
	{
		blendmaps[i].resize(_heightFields.size());
		for (size_t j = 0; j < _heightFields[i].size(); j++)
		{
			size_t x, z, k;
			float worldx, worldz, worldy, intensity;

			blendmaps[i][j].resize(2);
			blendmaps[i][j][0].resize((_blendResolution * _blendResolution) * 4);
			blendmaps[i][j][1].resize((_blendResolution * _blendResolution) * 4);

			for (x = 0; x < _blendResolution; x++)
			{
				for (z = 0; z < _blendResolution; z++)
				{
					k = 4 * x + (z * _blendResolution * 4);

					worldx = ((float)x / (float)_blendResolution) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)_blendResolution) * (worldmaxz - worldminz) + worldminz;

					worldy = returnHeight(scaleY, worldx, worldz, _heightFields[i][j], _heightFieldSize);

					// In the first pass we determine the min and max for all the tiles.
					if (worldy < terrainminheight || (j == 0 && i == 0)) {
						terrainminheight = worldy;
					}

					if (worldy > terrainmaxheight || (j == 0 && i == 0)) {
						terrainmaxheight = worldy;
					}
//#define TEST
#ifdef TEST
					// And work out the value for the purely slope based map.
					intensity = abs(returnHeight(scaleY, worldx - intensity2, worldz, _heightFields[i][j], _heightFieldSize) - worldy);
					intensity += abs(returnHeight(scaleY, worldx + intensity2, worldz, _heightFields[i][j], _heightFieldSize) - worldy);
					intensity += abs(returnHeight(scaleY, worldx, worldz - intensity2, _heightFields[i][j], _heightFieldSize) - worldy);
					intensity += abs(returnHeight(scaleY, worldx, worldz + intensity2, _heightFields[i][j], _heightFieldSize) - worldy);

					intensity /= 400.0f;
#endif
#define DEFAULT
#ifdef DEFAULT
					// And work out the value for the purely slope based map.
					intensity = abs(returnHeight(scaleY, worldx - 100, worldz, _heightFields[i][j], _heightFieldSize) - worldy);
					intensity += abs(returnHeight(scaleY, worldx + 100, worldz, _heightFields[i][j], _heightFieldSize) - worldy);
					intensity += abs(returnHeight(scaleY, worldx, worldz - 100, _heightFields[i][j], _heightFieldSize) - worldy);
					intensity += abs(returnHeight(scaleY, worldx, worldz + 100, _heightFields[i][j], _heightFieldSize) - worldy);

					intensity /= 400.0f;
#endif

					if (intensity > 1.0f) {
						intensity = 1.0f;
					}
					
#define OPACITY
#ifdef OPACITY
					intensity = (intensity / 100) * opacity2;
					intensity *= 254;
#else
					intensity *= 254;
#endif

					if (intensity > 254) {
						intensity = 254;
					}
					if (intensity < 1) {
						intensity = 1;
					}

					blendmaps[i][j][1][k] = intensity;
					blendmaps[i][j][1][k + 1] = intensity;
					blendmaps[i][j][1][k + 2] = intensity;
					blendmaps[i][j][1][k + 3] = intensity;
				}
			}
		}
	}
	for (size_t i = 0; i < _heightFields.size(); i++)
	{
		for (size_t j = 0; j < _heightFields[i].size(); j++)
		{
			size_t x, z, k, k1, k2, k3, k4;
			float worldx, worldz, worldy, intensity;

			// Layer 1 is determined purely by the height.
			for (x = 0; x < _blendResolution; x++) {
				for (z = 0; z < _blendResolution; z++) {
					k = 4 * x + (z * _blendResolution * 4);

					// Purely height based.
					worldx = ((float)x / (float)_blendResolution) * (worldmaxx - worldminx) + worldminx;
					worldz = ((float)z / (float)_blendResolution) * (worldmaxz - worldminz) + worldminz;

					worldy = returnHeight(scaleY, worldx, worldz, _heightFields[i][j], _heightFieldSize);

//#define OPACITY2
#ifdef OPACITY2
					worldy -= terrainminheight;
					worldy /= (float)(terrainmaxheight - terrainminheight);
					worldy *= 0.75;//25% less opacity
					intensity = 254 * worldy;
					//so the dirt blendmap intensity become 75% instead of 100%
					//so theres still dirt but it's more transparent

					/*if (intensity > 254) {
						intensity = 254;
					}
					if (intensity < 1) {
						intensity = 1;
					}*/
#endif
//#define OPACITY3
#ifdef OPACITY3
					worldy -= terrainminheight;
					worldy /= (float)(terrainmaxheight - ((terrainmaxheight / 100) * opacity1));//40% more transparent dirt
					intensity = 254 * worldy;

					if (intensity > 254)
					{
						intensity = 254;
					}
					if (intensity < 1)
					{
						intensity = 1;
					}
#endif
//#define SHIT2
#ifdef SHIT2
					worldy -= ((terrainmaxheight / 100) * intensity1);
					worldy /= (float)(terrainmaxheight);// - ((terrainmaxheight / 100) * intensity1));//40% less dirt
					intensity = 254 * worldy;

					if (intensity > 254)
					{
						intensity = 254;
					}
					if (intensity < 1)
					{
						intensity = 1;
					}
#endif

#define TEST3
#ifdef TEST3
					worldy -= terrainminheight;
					worldy -= ((terrainmaxheight / 100) * intensity1);
					worldy /= (float)(terrainmaxheight - ((terrainmaxheight / 100) * opacity1));
					intensity = 254 * worldy;

					if (intensity > 254)
					{
						intensity = 254;
					}
					if (intensity < 1)
					{
						intensity = 1;
					}
#endif

					blendmaps[i][j][0][k] = intensity;
					blendmaps[i][j][0][k + 1] = intensity;
					blendmaps[i][j][0][k + 2] = intensity;
					blendmaps[i][j][0][k + 3] = intensity;

					// Average the slope blends (a bit smoother).
					k1 = 4 * ((x - 1) % _blendResolution) + (z * _blendResolution * 4);
					k2 = 4 * ((x + 1) % _blendResolution) + (z * _blendResolution * 4);
					k3 = 4 * (x)+(((z - 1) % _blendResolution) * _blendResolution * 4);
					k4 = 4 * (x)+(((z + 1) % _blendResolution) * _blendResolution * 4);
					blendmaps[i][j][1][k] = (blendmaps[i][j][1][k] + blendmaps[i][j][1][k1] + blendmaps[i][j][1][k2] + blendmaps[i][j][1][k3] + blendmaps[i][j][1][k4]) / 5.0f;
					blendmaps[i][j][1][k + 1] = blendmaps[i][j][1][k + 2] = blendmaps[i][j][1][k + 3] = blendmaps[i][j][1][k];
				}
			}
		}
	}
	return blendmaps;
}

std::vector<std::vector<gameplay::HeightField*> > 
	TerrainGenerator::buildTerrainTiles(size_t heightmapResolution,
										size_t tilesResolution,
										int minHeight,
										int maxHeight,
										int type,
										int gain,
										int amplitude,
										bool bGain,
										bool bAmplitude)
{
	std::vector<std::vector<gameplay::HeightField*> > heightfields;

	//create empty heightmaps
	heightfields.resize(tilesResolution);
	for (size_t i = 0; i < tilesResolution; i++)
	{
		heightfields[i].resize(tilesResolution);
		for (size_t j = 0; j < tilesResolution; j++)
		{
			heightfields[i][j] = HeightField::create(heightmapResolution, heightmapResolution);
		}
	}

	INoiseAlgorithm * noise = NULL;
	if (type == 1)
	{
		noise = new DiamondSquareNoise();
	}
	else if (type == 0)
	{
		noise = new SimplexNoise(tilesResolution,amplitude,gain,bAmplitude,bGain);
	}

	//make a huge buffer
	noise->init(heightmapResolution*tilesResolution, heightmapResolution*tilesResolution, minHeight, maxHeight, 0);

	float *usedHeights;

	//this is basicly an interlaced iteration to push out into heightfields
	size_t i = 0, j = 0, k = 0, g = 0;
	for (i = 0; i < tilesResolution; i++)
	{
		for (j = 0; j < heightmapResolution; j++)
		{
			for (k = 0; k < tilesResolution; k++)
			{
				size_t y = i;
				//int y = (((i*j) / heightmapResolution));
				size_t x = (((g*k) / heightmapResolution));
				usedHeights = heightfields[x][y]->getArray();
				for (g = 0; g < heightmapResolution; g++)
				{
					size_t vertexe = (j*heightmapResolution) + g;
					//usedHeights2[vertexe] = noise->noise(((i*heightmapResolution) + j), ((k*heightmapResolution) + g));// kinda work
					//usedHeights2[vertexe] = noise->noise(((k*heightmapResolution) + j), ((i*heightmapResolution) + g));
					//usedHeights2[vertexe] = noise->noise(((k*heightmapResolution) + g), ((i*heightmapResolution) + j));
					usedHeights[vertexe] = noise->noise(((i*heightmapResolution) + g), ((k*heightmapResolution) + j));
				}
			}
		}
	}

	delete noise;

	TerrainEditor terrEdit;
	terrEdit.aligningTerrainsVertexes(heightfields, heightmapResolution);

	return heightfields;
}