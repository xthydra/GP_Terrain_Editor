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


TerrainToolAutoBindingResolver::TerrainToolAutoBindingResolver()
	: _light(NULL)
{

}

void TerrainToolAutoBindingResolver::setLight(Light* light)
{
	if (_light) {
		SAFE_RELEASE(_light);
	}
	light->addRef();
	_light = light;
}

TerrainToolAutoBindingResolver::~TerrainToolAutoBindingResolver()
{
	SAFE_RELEASE(_light);
}

bool TerrainToolAutoBindingResolver::resolveAutoBinding(const char* autoBinding, Node* node, MaterialParameter* parameter)
{
	//directional and spotlight
	if (strcmp(autoBinding, "LIGHT_DIRECTION_0") == 0)
	{
		parameter->bindValue(this, &TerrainToolAutoBindingResolver::getLightDirection0);
		return true;
	}
	//directional,spotlight and pointlight
	else if (strcmp(autoBinding, "LIGHT_COLOR_0") == 0)
	{
		parameter->bindValue(this, &TerrainToolAutoBindingResolver::getLightColor0);
		return true;
	}
	//point light,spot light
	else if (strcmp(autoBinding, "LIGHT_POSITION_0") == 0)
	{
		parameter->bindValue(this, &TerrainToolAutoBindingResolver::getLightPosition0);
		return true;
	}
	else if (strcmp(autoBinding, "LIGHT_RANGE_0") == 0)
	{
		parameter->bindValue(this, &TerrainToolAutoBindingResolver::getLightRange0);
		return true;
	}
	//point light,spot light
	else if (strcmp(autoBinding, "LIGHT_INVERSE_RANGE_0") == 0)
	{
		parameter->bindValue(this, &TerrainToolAutoBindingResolver::getLightInverseRange0);
		return true;
	}
	//spot light
	else if (strcmp(autoBinding, "LIGHT_INNER_ANGLE_COS_0") == 0)
	{
		parameter->bindValue(this, &TerrainToolAutoBindingResolver::getLightInnerAngleCos0);
		return true;
	}
	//spot light
	else if (strcmp(autoBinding, "LIGHT_OUTTER_ANGLE_COS_0") == 0)
	{
		parameter->bindValue(this, &TerrainToolAutoBindingResolver::getLightOutterAngleCos0);
		return true;
	}

	return false;
}


Vector3 TerrainToolAutoBindingResolver::getLightColor0() const
{
	if (_light) {
		return _light->getColor();
	}
	return Vector3::one();
}

Vector3 TerrainToolAutoBindingResolver::getLightDirection0() const
{
	if (_light)
	{
		return _light->getNode()->getForwardVectorView();
	}
	return Vector3::one();
}

Vector3 TerrainToolAutoBindingResolver::getLightPosition0() const
{
	if (_light)
	{
		return _light->getNode()->getTranslationView();
	}
	return Vector3::one();
	//return _directionalLight->getNode()->getTranslation();
}
float TerrainToolAutoBindingResolver::getLightInverseRange0() const
{
	if (_light)
	{
		return _light->getRangeInverse();
	}
	return 1;
}
float TerrainToolAutoBindingResolver::getLightRange0() const
{
	if (_light)
	{
		return _light->getRange();
	}
	return 1;
}
float TerrainToolAutoBindingResolver::getLightInnerAngleCos0() const
{
	if (_light)
	{
		return _light->getInnerAngleCos();
	}
	return 1;
}
float TerrainToolAutoBindingResolver::getLightOutterAngleCos0() const
{
	if (_light)
	{
		return _light->getOuterAngleCos();
	}
	return 1;
}
