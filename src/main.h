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

//xcopy ..\GamePlay\res\shaders res\shaders\* / s / y / d
//xcopy ..\GamePlay\bin\windows\OpenAL.dll $(Configuration) / s / y / d

//if you see error C2504 you need to put the files with error in it underneath the class undefined

#include "gameplay.h"

#define EXAMPLE
#ifdef EXAMPLE
#define CREATE_NORMALMAPS
#define CREATE_BLENDMAPS
#define CREATE_TREES
#define POINTLIGHT_TEST
#endif


#pragma once
#ifdef WIN32
#define _USE_MATH_DEFINES
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <time.h>
#include <cstdlib>
#include <io.h>
#include <direct.h>
#include <share.h>
#else
#include <unistd.h>
#endif
#include <thread>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <iostream>	
#include <fcntl.h>

#include "Terrain.h"
#include "SelectionRing.h"
#include "LodePNG.h"
#include "FirstPersonCamera.h"
#include "INoiseAlgorithm.h"
#include "DiamondSquareNoise.h"
#include "SimplexNoise.h"
#include "AutoBindingResolver.h"
#include "Generator.h"
#include "FilesLoader.h"
#include "FilesSaver.h"
#include "Pager.h"
#include "TerrainEditor.h"
#include "UInterface.h"

/**
* Used to confirm a thread termination.
**/
struct Threads
{
	bool blendmap,
		normalmap,
		objectPos;

	void blendBool() { blendmap = true, normalmap = false, objectPos = false; }
	void normalBool() { blendmap = false, normalmap = true, objectPos = false; }
	void objectBool() { blendmap = false, normalmap = false, objectPos = true; }
};

using namespace gameplay;

/**
* Main game class.
*/
class Main : public Game, Control::Listener
{
	/**
	* @see Game::keyEvent
	*/
	void keyEvent(Keyboard::KeyEvent evt, int key);

	/**
	* @see Game::mouseEvent
	*/
	bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta);

	/**
	* @see Game::initialize
	*/
	void initialize();

	/**
	* @see Game::finalize
	*/
	void finalize();

	/**
	* @see Game::update
	*/
	void update(float elapsedTime);

	/**
	* @see Game::render
	*/
	void render(float elapsedTime);

	/**
	* Handler for UI form event
	*/
	void controlEvent(Control* control, EventType evt);

	/**
	* Draws the scene each frame.
	*/
	bool drawScene(Node* node);

	/**
	* If the camera is moving, move it relative to the amount of time that has passed.
	*
	* @param elapsedTime ...
	* @return void
	**/
	void moveCamera(float elapsedTime);

	/**
	* Build a new terrain based on the parameters in the terrain generation form.
	*
	* @return void
	**/
	void generateNewTerrain();

	/*
	used to load files using the UI
	*/
	void load();

	/*
	randomly generate objects positions
	*/
	void generateObjectsPosition();

	/*
	threads used to save files
	*/
	std::vector<std::thread> threads;
	
	/*
	used to confirm the state of a thread
	*/
	std::vector<Threads> saverThreads;

	/**
	* Generate blendmaps
	*
	* @return void
	**/
	void generateNewBlendmaps();

	/**
	* Used by the ui to switch input states.
	**/
	enum INPUT_MODE
	{
		NAVIGATION, 
		EDITING,
		PAINT,
		GENERATOR
	};

	/**
	* The Pager will take care of loading,unloading and rendering various objects based on distance
	**/
	Pager * _pager;

	/**
	* The root scene node.
	**/
	Scene* _scene;

	/**
	* The root of the selection node.
	**/
	Node* _selection;

	/**
	* The selection ring instance.
	**/
	SelectionRing *_selectionRing;

	/**
	* Used to pass params to the terrain shader.
	**/
	TerrainToolAutoBindingResolver* _binding;

	/**
	* This is the scene camera.
	**/
	FirstPersonCamera * _camera;

	/**
	* The scene light.
	**/
	Light* _light;

	UI * interface;
	
	/**
	* boolean flags to say if we are moving.
	**/
	bool _moveForward, _moveBackward, _moveLeft, _moveRight;

	/**
	* Used to cancel an action.
	**/
	bool _doAction;

	/**
	* Store the last cursor position.
	**/
	float _prevX, _prevY;

	/**
	* Used to know if the button is hold
	**/
	bool LMB, RMB;

	/**
	* The current input mode.
	**/
	INPUT_MODE _inputMode;

public:

	/**
	* Constructor.
	*/
	Main();
};

/**
* Used to quickly filter ray collisions to collisions with the terrain (for picking).
**/

struct TerrainHitFilter : public PhysicsController::HitFilter {

	/**
	* Constructor -- stores a reference to the terrain.
	*
	* @param terrain ...
	**/
	TerrainHitFilter(Terrain* terrain)
	{
		terrainObject = terrain->getNode()->getCollisionObject();
	}

	/**
	* Only return true if the hit is on the terrain.
	*
	* @param object The result of the ray collision test.
	* @return bool
	**/
	bool filter(PhysicsCollisionObject* object)
	{
		// Filter out all objects but the terrain
		return (object != terrainObject);
	}

	/**
	* Stored so it can be used in the hit detection.
	**/
	PhysicsCollisionObject* terrainObject;
};

