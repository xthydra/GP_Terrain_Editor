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

SelectionRing::SelectionRing(Scene* scene)
	:_ringCount(16)
{
	// Store a reference to the scene node.
	_scene = scene;
	_scene->addRef();

	// Load a 3d model for a single point around the ring.
	Bundle* bundle;
	bundle = Bundle::create("res/selection.gpb");
	Node *one = bundle->loadNode("SelectionModel");

	// Attempt to set a material for the model - this is not working and I need to work out why.
	//one->getModel()->setMaterial("res/demo.material#colored", 0);
	Model* ring = dynamic_cast<Model*>(one->getDrawable());
	ring->setMaterial("res/materials/demo.material#colored", 0);

	// We dont need this anymore.
	SAFE_RELEASE(bundle);

	// Save a reference to the model node.
	_node = _scene->addNode("SelectionRing");

	// Duplicate the node at even points around the selection ring.
	int i = 0;
	for (i = 0; i < _ringCount; i++) {
		float x = sin((float)i / _ringCount * MATH_PIX2);
		float y = cos((float)i / _ringCount * MATH_PIX2);
		Node *clone = one->clone();
		clone->setTranslationX(x);
		clone->setTranslationX(y);
		_node->addChild(clone);
		SAFE_RELEASE(clone);
	}

	_scene->addNode(_node);
}
Vector3 SelectionRing::getPosition()
{
	return _position;
}

float SelectionRing::getScale()
{
	return _scale;
}

SelectionRing::~SelectionRing()
{
	SAFE_RELEASE(_node);
	SAFE_RELEASE(_scene);
}

void SelectionRing::setPosition(float x, float z, Terrain *terrain)
{
	_position.x = x;
	_position.z = z;

	this->setRingNodeHeights(terrain);
}

void SelectionRing::setRingNodeHeights(Terrain *terrain)
{
	Node *ring = _node->getFirstChild();
	int i = 0;

	float tmpx = _position.x;
	float tmpz = _position.z;

	//reposition the ray hit as if the terrain underneath the ray was at position 0
	int Xarray = terrain->getNode()->getTranslationWorld().x
		/ (terrain->getBoundingBox().max.x * 2);
	tmpx -= (Xarray)*(terrain->getBoundingBox().max.x * 2);

	int Zarray = terrain->getNode()->getTranslationWorld().z
		/ (terrain->getBoundingBox().max.z * 2);
	tmpz -= (Zarray)*(terrain->getBoundingBox().max.z * 2);

	std::vector<float> cumulativeHeight;

	// Adjust the height of each node in the ring so it sits just above the terrain.
	while (ring) 
	{
		float offsetx = sin((float)i / _ringCount * MATH_PIX2) * _scale;
		float offsetz = cos((float)i / _ringCount * MATH_PIX2) * _scale;

		// adjust the height seperately for each node in the ring.
		float height = terrain->getHeight(offsetx + tmpx, offsetz + tmpz) + (_scale / 8.0f);

		cumulativeHeight.push_back(height);

		ring->setScale(_scale / 8.0f);
		ring->setTranslation(offsetx + _position.x, height, offsetz + _position.z);
		ring = ring->getNextSibling();
		i++;
	}
	//get the cumulative height of all the rings
	float cumulativeHeight2=0;
	for (size_t i = 0; i <cumulativeHeight.size(); i++)
	{
		cumulativeHeight2 += cumulativeHeight[i];
	}
	_position.y = cumulativeHeight2 / _ringCount;
}

void SelectionRing::setScale(float scale, Terrain *terrain)
{
	_scale = scale;

	this->setRingNodeHeights(terrain);
}


