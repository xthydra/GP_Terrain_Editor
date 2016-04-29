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

#include "Main.h"

/**
* The current class is used to parse events related to the user interface and
* is partially used by the generator(generator.cpp) structure to define parameters needed to create heightfields,normal maps and blendmaps.
**/
class UI
{
	Scene * _scene;

public:

	enum INPUT_MODE
	{
		NAVIGATION,
		EDITING,
		PAINT,
		GENERATOR
	};

	/**
	* A ui form.
	**/
	Form* mainForm;

	/**
	* A ui form.
	**/
	Form* sizeForm;

	/**
	* UI form for generating objects.
	**/
	Form* generateObjectsForm;

	/**
	* UI form for generating terrains.
	**/
	Form* generateTerrainsForm;

	/**
	* UI form for loading files.
	**/
	Form* loadForm;

	/**
	* UI form for saving files.
	**/
	Form* saveForm;

	/**
	* UI form for modiying blendmaps intensity.
	**/
	Form* generateBlendmapsForm;

	/**
	* Used in editing tools to toggle tools
	**/
	Vector4 activeButton, normalButton;

	SelectionRing * selectionRing;

	Pager * pager;

	FirstPersonCamera * camera;

	INPUT_MODE inputMode;

	UI(Scene *, Control::Listener *, FirstPersonCamera *, Pager *);

	void updateForms(float elapsedTime);

	void renderForms();

	void controlEvents(Control* control, Control::Listener::EventType evt);
};