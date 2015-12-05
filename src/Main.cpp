﻿/*
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

char * createTMPFolder()
{
#ifdef WIN32
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mktemp(tmpdir);
	_mkdir("res/tmp");
	_mkdir(tmpdir);

	std::string dir;
	dir += tmpdir;
	dir += "/";
	char* cstr = new char[dir.length() + 1];
	strcpy(cstr, dir.c_str());
#else
	char tmpdir[] = "res/tmp/fileXXXXXX";
	mkdtemp(tmpdir);

	std::string dir;
	dir += tmpdir;
	dir += "/";
	char* cstr = new char[dir.length() + 1];
	strcpy(cstr, dir.c_str());
#endif
	return cstr;
}

bool toggleVisibility(gameplay::Control * control)
{
	if (control->isVisible())
	{
		return false;
	}
	else { return true; }
}

// Declare our game instance
Main game;

Main::Main()
	: _scene(NULL),
	_mainForm(NULL),
	_generateTerrainsForm(NULL),
	_loadForm(NULL),
	_moveForward(false),
	_moveBackward(false),
	_moveLeft(false),
	_moveRight(false),
	_prevX(0),
	_prevY(0),
	MOVE_SPEED(10.0f),
	_selectionScale(100.0f),
	_inputMode(NAVIGATION),
	_doAction(false)
{
}

void Main::initialize()
{
	
#ifdef _WIN64
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
#endif

	// Create an empty scene.
	_scene = Scene::create();
	_scene->setAmbientColor(0.5,0.5,0.5);

	// Setup a fly cam.
	_camera.initialize(1.0f, 60000.0f, 45);
	_camera.rotate(0.0f, -MATH_DEG_TO_RAD(10));
	_scene->addNode(_camera.getRootNode());
	_scene->setActiveCamera(_camera.getCamera());

	// Create a light source.

#ifdef POINTLIGHT_TEST
	_light = Light::createPoint(Vector3::one(), 800.0f);
	Node* lightNode = _scene->addNode("light");
	lightNode->setLight(_light);

	//setting up a light
	_binding = new TerrainToolAutoBindingResolver();
	_binding->setLight(_light);
#else
	_light = Light::createDirectional(Vector3(0.5,0.5,0.7));
	Node* lightNode = _scene->addNode("light");
	lightNode->setLight(_light);

	//setting up a light
	_binding = new TerrainToolAutoBindingResolver();
	_binding->setLight(_light);
#endif
	//creating and setting up the UI
	
	_mainForm = Form::create("res/forms/main.form");

	Control *control = _mainForm->getControl("EditorButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("NavigateButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GeneratorButton");
	control->addListener(this, Control::Listener::CLICK);

	/* Not implemented yet
	control = _mainForm->getControl("PaintButton");
	control->addListener(this, Control::Listener::CLICK);
	*/
	//=============NAVIGATOR
	Slider *slider = (Slider *)_mainForm->getControl("MoveSlider");
	slider->addListener(this, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)_mainForm->getControl("LODSlider");
	slider->addListener(this, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)_mainForm->getControl("TerrainsRenderSlider");
	slider->addListener(this, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)_mainForm->getControl("ObjectsRenderSlider");
	slider->addListener(this, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)_mainForm->getControl("LoadSlider");
	slider->addListener(this, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)_mainForm->getControl("UnloadSlider");
	slider->addListener(this, Control::Listener::VALUE_CHANGED);

	_mainForm->getControl("NavigateToolBar")->setVisible(true);
	//=============

	//=============GENERATOR
	control = _mainForm->getControl("GenerateTerrainsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GenerateBlendMapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GenerateNormalMapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GenerateObjectsPosButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("SaveButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("LoadButton");
	control->addListener(this, Control::Listener::CLICK);
	//=============

	//=============TERRAIN_EDITOR
	control = _mainForm->getControl("AlignButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("RaiseButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("LowerButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("FlattenButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("SmoothButton");
	control->addListener(this, Control::Listener::CLICK);

	slider = (Slider *)_mainForm->getControl("SizeSlider");
	slider->addListener(this, Control::Listener::VALUE_CHANGED);
	//===============
	_generateTerrainsForm = Form::create("res/forms/generateTerrains.form");
	_generateTerrainsForm->setVisible(false);

	control = _generateTerrainsForm->getControl("CancelGenerateTerrainsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _generateTerrainsForm->getControl("ConfirmGenerateTerrainsButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============	
	_generateBlendmapsForm = Form::create("res/forms/generateBlendmaps.form");
	_generateBlendmapsForm->setVisible(false);

	control = _generateBlendmapsForm->getControl("CancelGenerateBlendmapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _generateBlendmapsForm->getControl("ConfirmGenerateBlendmapsButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============
	_loadForm = Form::create("res/forms/load.form");
	_loadForm->setVisible(false);

	control = _loadForm->getControl("CancelLoadButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _loadForm->getControl("ConfirmLoadButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============
	_generateObjectsForm = Form::create("res/forms/generateObjects.form");
	_generateObjectsForm->setVisible(false);

	control = _generateObjectsForm->getControl("CancelGenerateObjectsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _generateObjectsForm->getControl("ConfirmGenerateObjectsButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============SAVE
	_saveForm = Form::create("res/forms/save.form");
	_saveForm->setVisible(false);

	control = _saveForm->getControl("RAWHeightmapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _saveForm->getControl("BlendmapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _saveForm->getControl("ObjectsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _saveForm->getControl("CancelSaveButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============

	control = _mainForm->getControl("RaiseButton");
	Button * button = (Button *)control;
	normalButton = button->getSkinColor(gameplay::Control::State::NORMAL);
	activeButton = button->getSkinColor(gameplay::Control::State::ACTIVE);
	
#ifdef EXAMPLE
	//Generate heightfields for the terrain pager
	PagerParameters parameters;
	parameters.generatedBlendmaps = false;
	parameters.generatedNormalmaps = false;
	parameters.generatedHeightmaps = false;
	parameters.generatedObjects = false;
	parameters.tilesResolution = 2;
	parameters.heightFieldResolution = 128;
	//if you put LOD on 1 it will make terrain creation faster
	parameters.lodQuality = 1;
	parameters.textureScale = 40;
	parameters.minHeight = 0;
	parameters.maxHeight = 128;
	parameters.skirtSize = 16;
	parameters.patchSize = 32;
#ifdef POINTLIGHT_TEST
	parameters.terrainMaterialPath = "res/materials/terrainPointlight.material";
#else
	parameters.terrainMaterialPath = "res/materials/terrainDirectionallight.material";
#endif

	parameters.scale = Vector3(parameters.heightFieldResolution, ((parameters.heightFieldResolution*parameters.tilesResolution)*0.10), parameters.heightFieldResolution);

	parameters.boundingBox = (parameters.heightFieldResolution * parameters.heightFieldResolution) - parameters.heightFieldResolution;

	TerrainGenerator terrainGenerator;
	std::vector<std::vector<gameplay::HeightField*>> heightfields;
	heightfields = terrainGenerator.buildTerrainTiles(parameters.heightFieldResolution,
		parameters.tilesResolution,
		parameters.minHeight,
		parameters.maxHeight,
		0,
		1,
		1,
		true,
		true);

	parameters.Debug = false;

	parameters.distanceTerrainLoad = parameters.boundingBox * .9;
	parameters.distanceTerrainUnload = parameters.boundingBox * 1;
	parameters.distanceTerrainMaxRender = parameters.boundingBox * 1;

	_pager = new Pager(parameters, _scene);

	_pager->heightFieldList = heightfields;

	_pager->computePositions();

#ifdef CREATE_BLENDMAPS
	_pager->blendMaps =
		terrainGenerator.createTiledTransparentBlendImages(_pager->parameters.scale.y,
														   _pager->parameters.scale.x,
														   0,
														   50,
														   0,
														   100,
														   _pager->parameters.heightFieldResolution,
														   _pager->heightFieldList);
	_pager->parameters.blendMapDIR = createTMPFolder();

	FilesSaver saver;
	Threads t;
	t.blendBool();
	saverThreads.push_back(t);

	threads.push_back(std::thread(
		&FilesSaver::saveBlendmaps,
		saver,
		_pager->blendMaps,
		_pager->parameters.blendMapDIR,
		_pager->parameters.heightFieldResolution));

#endif

#ifdef CREATE_NORMALMAPS
	_pager->normalMaps = 
		terrainGenerator.createNormalmaps(parameters.scale.y, 
										  _pager->parameters.scale.x,
										  _pager->parameters.heightFieldResolution, 
										  _pager->heightFieldList);

	_pager->parameters.normalMapDIR = createTMPFolder();

	Threads t2;
	t2.normalBool();
	saverThreads.push_back(t2);

	threads.push_back(std::thread(
		&FilesSaver::saveNormalmaps,
		saver,
		_pager->normalMaps,
		_pager->parameters.normalMapDIR,
		_pager->parameters.heightFieldResolution));

#endif
	_camera.setPosition(Vector3(0, ((parameters.heightFieldResolution*parameters.tilesResolution) * 25), 0));
	_camera.rotate(0, -90);

	_selectionRing = new SelectionRing(_scene);

#ifdef CREATE_TREES
#ifdef POINTLIGHT_TEST
	control = _generateObjectsForm->getControl("ObjectTextBox");
	TextBox * textBox = (TextBox *)control;
	textBox->setText("res/common/Tree_pointlight.scene");
	generateObjectsPosition();
#else
	generateObjectsPosition();
#endif
#endif
#endif

}

void Main::finalize()
{
	SAFE_RELEASE(_mainForm);
	SAFE_RELEASE(_generateTerrainsForm);
	SAFE_RELEASE(_generateBlendmapsForm);
	SAFE_RELEASE(_loadForm);
	SAFE_RELEASE(_light);
	SAFE_RELEASE(_selectionRing);
	SAFE_RELEASE(_scene);
	delete _binding;
}

void Main::controlEvent(Control* control, Control::Listener::EventType evt)
{
	//=====================MAIN_FORM
	if (strcmp(control->getId(), "NavigateButton") == 0)
	{
		_inputMode = NAVIGATION;
		//_mainForm->getControl("PaintToolbar")->setVisible(false);
		_mainForm->getControl("TerrainEditorbar")->setVisible(false);
		_mainForm->getControl("GeneratorToolBar")->setVisible(false);
		_mainForm->getControl("NavigateToolBar")->setVisible(true);
	}
	else if (strcmp(control->getId(), "EditorButton") == 0)
	{
		_inputMode = EDITING;
		_mainForm->getControl("NavigateToolBar")->setVisible(false);
		//_mainForm->getControl("PaintToolbar")->setVisible(false);
		_mainForm->getControl("GeneratorToolBar")->setVisible(false);
		_mainForm->getControl("TerrainEditorbar")->setVisible(true);
	}
	else if (strcmp(control->getId(), "GeneratorButton") == 0)
	{
		_inputMode = GENERATOR;
		_mainForm->getControl("NavigateToolBar")->setVisible(false);
		//_mainForm->getControl("PaintToolbar")->setVisible(toggleVisibility(_mainForm->getControl("PaintToolBar")));
		_mainForm->getControl("TerrainEditorbar")->setVisible(false);
		_mainForm->getControl("GeneratorToolBar")->setVisible(true);
	}
	else if (strcmp(control->getId(), "PaintButton") == 0)
	{
		_inputMode = PAINT;
		_mainForm->getControl("NavigateToolBar")->setVisible(false);
		_mainForm->getControl("TerrainEditorbar")->setVisible(false);
		_mainForm->getControl("GeneratorToolBar")->setVisible(false);
		//_mainForm->getControl("PaintToolbar")->setVisible(true);
	}
	//=============================

	//=====================GENERATOR
	else if (strcmp(control->getId(), "GenerateTerrainsButton") == 0)
	{
		_mainForm->setVisible(false);
		_generateTerrainsForm->setVisible(true);
	}
	else if (strcmp(control->getId(), "GenerateBlendMapsButton") == 0)
	{
		if (_pager)
		{
			if (_pager->heightFieldList.size() > 0)
			{
				_mainForm->setVisible(false);
				_generateBlendmapsForm->setVisible(true);
			}
		}
	}
	else if (strcmp(control->getId(), "GenerateNormalMapsButton") == 0)
	{
		if (_pager)
		{
			if (_pager->heightFieldList.size() > 0)
			{
				TerrainGenerator terrainGenerator;
				terrainGenerator.createNormalmaps(_pager->parameters.scale.y, _pager->parameters.scale.x, _pager->parameters.heightFieldResolution, _pager->heightFieldList);
			}
		}
	}
	else if (strcmp(control->getId(), "GenerateObjectsPosButton") == 0)
	{
		_mainForm->setVisible(false);
		_generateObjectsForm->setVisible(true);
	}
	else if (strcmp(control->getId(), "SaveButton") == 0)
	{
		_mainForm->setVisible(false);
		_saveForm->setVisible(true);
	}
	else if (strcmp(control->getId(), "LoadButton") == 0)
	{
		_loadForm->setVisible(true);
		_mainForm->setVisible(false);
	}

	//=============================

	//====================NAVIGATOR
	else if (strcmp(control->getId(), "MoveSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		MOVE_SPEED = slider->getValue();
	}
	else if (strcmp(control->getId(), "LODSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		_pager->parameters.lodQuality = slider->getValue();
	}
	else if (strcmp(control->getId(), "LoadSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		if (_pager->parameters.distanceTerrainUnload > (slider->getValue() * _pager->parameters.boundingBox))
		{
			_pager->parameters.distanceTerrainLoad = (slider->getValue() * _pager->parameters.boundingBox);
		}
		else if (_pager->parameters.distanceTerrainUnload < (slider->getValue() * _pager->parameters.boundingBox))
		{
			Control * control2 = _mainForm->getControl("UnloadSlider");
			Slider * slider2 = (Slider *)control2;
			slider2->setValue(slider->getValue() + 1);
			//setloadSlider value below unloadSlider
			_pager->parameters.distanceTerrainUnload = (slider2->getValue() * _pager->parameters.boundingBox);
			_pager->parameters.distanceTerrainLoad = (slider->getValue() * _pager->parameters.boundingBox);
		}
	}
	else if (strcmp(control->getId(), "ObjectsRenderSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		_pager->parameters.distanceMaxModelRender = ((slider->getValue() * _pager->parameters.boundingBox) * 0.2);
	}
	else if (strcmp(control->getId(), "TerrainsRenderSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		_pager->parameters.distanceTerrainMaxRender = (slider->getValue() * _pager->parameters.boundingBox);
	}
	else if (strcmp(control->getId(), "UnloadSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		if (_pager->parameters.distanceTerrainLoad < (slider->getValue() * _pager->parameters.boundingBox))
		{
			_pager->parameters.distanceTerrainUnload = (slider->getValue() * _pager->parameters.boundingBox);
		}
		else if (_pager->parameters.distanceTerrainLoad >(slider->getValue() * _pager->parameters.boundingBox))
		{
			Control * control2 = _mainForm->getControl("LoadSlider");
			Slider * slider2 = (Slider *)control2;
			slider2->setValue(slider->getValue() - 1);
			//setloadSlider value below unloadSlider
			_pager->parameters.distanceTerrainLoad = (slider2->getValue() * _pager->parameters.boundingBox);
			_pager->parameters.distanceTerrainUnload = (slider->getValue() * _pager->parameters.boundingBox);
		}
	}
	//=============================

	//=======================EDITOR
	else if (strcmp(control->getId(), "SizeSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		_selectionScale = slider->getValue() * ((_pager->parameters.heightFieldResolution / 8) * (_pager->parameters.scale.x / 8));
		int nearest = _pager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
		if (nearest != -1)
		{
			_selectionRing->setScale(_selectionScale, _pager->loadedTerrains[nearest]);
		}
	}
	/*
	TerrainEditor TerrEdit;
	std::vector<std::vector<unsigned char>> blend1, blend2;
	std::vector<Vector3> fieldsPos;

	for (size_t i = 0; i < _pager->loadedTerrains.size(); i++)
	{
		int posX = _pager->loadedTerrains[i]->getNode()->getTranslationWorld().x;
		int posZ = _pager->loadedTerrains[i]->getNode()->getTranslationWorld().z;

		int sPosX = posX / _pager->parameters.scale.x;
		int sPosZ = posZ / _pager->parameters.scale.x;

		posX = (posX / _pager->parameters.scale.x) / (_pager->parameters.heightFieldResolution - 1);
		posZ = (posZ / _pager->parameters.scale.x) / (_pager->parameters.heightFieldResolution - 1);

		blend1[i] = _pager->blendMaps[posX][posZ][0];
		blend2[i] = _pager->blendMaps[posX][posZ][1];
		fieldsPos.push_back(Vector3(sPosX, 0, sPosZ));
	}

	TerrEdit.paint(blend1,
		blend2,
		fieldsPos,
		0,
		Vector2(_selectionRing->getPosition().x, _selectionRing->getPosition().y),
		_pager->parameters.heightFieldResolution,
		_selectionRing->getScale()
		);*/

	else if (strcmp(control->getId(), "AlignButton") == 0)
	{
		TerrainEditor TerrEdit;
		TerrEdit.aligningTerrainsVertexes(_pager->heightFieldList);
		_pager->reloadTerrains();
	}
	else if (strcmp(control->getId(), "RaiseButton") == 0)
	{
		control = _mainForm->getControl("RaiseButton");
		Button * button = (Button *)control;
		button->setSkinColor(activeButton, gameplay::Control::State::NORMAL);
		control = _mainForm->getControl("LowerButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = _mainForm->getControl("FlattenButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
	}
	else if (strcmp(control->getId(), "LowerButton") == 0)
	{
		control = _mainForm->getControl("LowerButton");
		Button * button = (Button *)control;
		button->setSkinColor(activeButton, gameplay::Control::State::NORMAL);
		control = _mainForm->getControl("RaiseButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = _mainForm->getControl("FlattenButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
	}
	else if (strcmp(control->getId(), "FlattenButton") == 0)
	{
		control = _mainForm->getControl("FlattenButton");
		Button * button = (Button *)control;
		button->setSkinColor(activeButton, gameplay::Control::State::NORMAL);
		control = _mainForm->getControl("LowerButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = _mainForm->getControl("RaiseButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
	}
	else if (strcmp(control->getId(), "SmoothButton") == 0)
	{
		TerrainEditor TerrEdit;
		int nearest = _pager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
		if (nearest != -1)
		{
			Vector3 ringPos = _selectionRing->getPosition();

			std::vector<int> heightfields =
				TerrEdit.smooth(
					BoundingSphere(ringPos, _selectionRing->getScale()),
					_pager->parameters.scale.x,
					_pager->parameters.scale.y,
					_pager->loadedTerrains,
					_pager->loadedHeightfields);

			_pager->reload(heightfields);
		}
	}
	//=============================
	else if (strcmp(control->getId(), "CancelGenerateTerrainsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateTerrainsForm->setVisible(false);
	}
	else if (strcmp(control->getId(), "ConfirmGenerateTerrainsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateTerrainsForm->setVisible(false);
		this->generateNewTerrain();
	}
	//===========================
	else if (strcmp(control->getId(), "CancelGenerateBlendmapsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateBlendmapsForm->setVisible(false);
	}
	else if (strcmp(control->getId(), "ConfirmGenerateBlendmapsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateBlendmapsForm->setVisible(false);
		generateNewBlendmaps();
	}
	//===========================
	else if (strcmp(control->getId(), "CancelLoadButton") == 0)
	{
		_mainForm->setVisible(true);
		_loadForm->setVisible(false);
	}
	else if (strcmp(control->getId(), "ConfirmLoadButton") == 0)	
	{
		_mainForm->setVisible(true);
		_loadForm->setVisible(false);
		load();
	}
	//===========================
	else if (strcmp(control->getId(), "RAWHeightmapsButton") == 0)
	{
		FilesSaver saver;
		
		threads.push_back(std::thread(
			&FilesSaver::saveRAWHeightmaps,
			saver,
			_pager->heightFieldList));
	}
	else if (strcmp(control->getId(), "BlendmapsButton") == 0)
	{
		FilesSaver saver;
		Threads t;
		t.blendBool();
		saverThreads.push_back(t);

		threads.push_back(std::thread(
			&FilesSaver::saveBlendmaps,
			saver,
			_pager->blendMaps,
			_pager->parameters.blendMapDIR,
			_pager->parameters.heightFieldResolution));

		//TODO: i really dont know if i need to clear the folder containing the blendmaps before overwriting
		//cuz at somepoint i will probably allow the user to change textures files which will probably end up corrupting the way files are loaded
	}
	else if (strcmp(control->getId(), "ObjectsButton") == 0)
	{
		FilesSaver saver;

		threads.push_back(std::thread(
			&FilesSaver::saveObjectsPos,
			saver,
			_pager->objectsPosition,
			_pager->objectsFilename[0]));

		Threads t;
		t.objectBool();
		saverThreads.push_back(t);
	}
	else if (strcmp(control->getId(), "CancelSaveButton") == 0)
	{
		_mainForm->setVisible(true);
		_saveForm->setVisible(false);
	}
	//===========================
	else if (strcmp(control->getId(), "CancelGenerateObjectsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateObjectsForm->setVisible(false);
	}
	else if (strcmp(control->getId(), "ConfirmGenerateObjectsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateObjectsForm->setVisible(false);
		generateObjectsPosition();
	}
}

void Main::load()
{
	Control * control;
	TextBox * textBox;
	RadioButton * radioButton;

	control = _loadForm->getControl("FolderTextBox");
	textBox = (TextBox *)control;
	const char * folder = textBox->getText();

	control = _loadForm->getControl("RawHeightfieldRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		_pager->removeObjects();
		_pager->removeTerrains();
		FilesLoader load;
		_pager->heightFieldList = load.loadRAWHeightmaps(folder);
		_pager->parameters.heightMapDIR = (char*)folder;
		_pager->parameters.tilesResolution = load.tilesResolution;
	}

	control = _loadForm->getControl("BlendmapRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		FilesLoader load;
		_pager->blendMapList = load.loadBlendmaps(folder);
		_pager->parameters.blendMapDIR = (char*)folder;
		_pager->parameters.tilesResolution = load.tilesResolution;
		_pager->reloadTerrains();
	}

	control = _loadForm->getControl("ObjectsPosRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		_pager->removeObjects();
		FilesLoader load;
		_pager->objectsPosition = load.loadObjectsPos(folder);
		//TODO : std::string objectName=load.fileName;
		//TODO : _pager->createObjects(objectName,_pager->parameters.objectsPosition);
	}

	control = _loadForm->getControl("NormalmapRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		FilesLoader load;
		_pager->normalMapList = load.loadNormalmaps(folder);
		_pager->parameters.normalMapDIR = (char*)folder;
		_pager->reloadTerrains();
	}

}

void Main::generateNewBlendmaps()
{
	if (_pager)
	{
		Control * control;
		Slider * slider;

		control = _generateBlendmapsForm->getControl("Blendmap1-Intensity");
		slider = (Slider *)control;
		int Intensity1 = slider->getValue();

		control = _generateBlendmapsForm->getControl("Blendmap2-Intensity");
		slider = (Slider *)control;
		int Intensity2 = slider->getValue();

		control = _generateBlendmapsForm->getControl("Blendmap1-Opacity");
		slider = (Slider *)control;
		int Opacity1 = slider->getValue();

		control = _generateBlendmapsForm->getControl("Blendmap2-Opacity");
		slider = (Slider *)control;
		int Opacity2 = slider->getValue();

		TerrainGenerator terrainGenerator;

		_pager->blendMaps = terrainGenerator.createTiledTransparentBlendImages(_pager->parameters.scale.y,
			_pager->parameters.scale.x,
			Intensity1,
			Intensity2,
			Opacity1,
			Opacity2,
			_pager->parameters.heightFieldResolution,
			_pager->heightFieldList);
		_pager->parameters.blendMapDIR = createTMPFolder();

		for (size_t i = 0; i < threads.size(); i++)
		{
			if (saverThreads[i].blendmap == true)
			{
				threads[i].~thread();
				threads.erase(threads.begin() + i);
				saverThreads.erase(saverThreads.begin() + i);
			}
		}

		FilesSaver saver;
		Threads t;
		t.blendBool();
		saverThreads.push_back(t);
		_pager->parameters.generatedBlendmaps = false;

		threads.push_back(std::thread(
			&FilesSaver::saveBlendmaps,
			saver,
			_pager->blendMaps,
			_pager->parameters.blendMapDIR,
			_pager->parameters.heightFieldResolution));

		_pager->reloadTerrains();
	}
}

void Main::generateObjectsPosition()
{
	if (_pager)
	{
		if (_pager->heightFieldList.size() > 0)
		{
			_pager->removeObjects();

			Control * control;
			TextBox * textBox;
			std::string fileName, material;

			control = _generateObjectsForm->getControl("MaterialTextBox");
			textBox = (TextBox *)control;
			if (textBox)
			{
				material = textBox->getText();
			}

			control = _generateObjectsForm->getControl("ObjectTextBox");
			textBox = (TextBox *)control;
			if (textBox)
			{
				fileName = textBox->getText();
			}

			control = _generateObjectsForm->getControl("ChanceTextBox");
			textBox = (TextBox *)control;
			if (textBox)
			{
				int chanceFactor = (strtol(textBox->getText(), NULL, 10));
			}

			Scene * tempScene = Scene::load(fileName.c_str());

			Node * nodes = tempScene->getFirstNode();
			Node * node = Node::create(fileName.c_str());
			while (nodes)
			{
				if (nodes->getDrawable())
				{
					node->addChild(nodes);
				}
				nodes = tempScene->getNext();
			}

			Vector3 worldScale;//(1,1,1);
			_pager->pagingCheck();
			_pager->loadedTerrains[0]->getNode()->getWorldMatrix().getScale(&worldScale);

			TerrainGenerator terrainGenerator;
			std::vector<std::vector<std::vector<Vector3*> > > objsPos = terrainGenerator.generateObjectsPosition(worldScale, 
				_pager->parameters.scale.y, 
				_pager->parameters.scale.x,
				1, 
				_pager->parameters.heightFieldResolution, 
				_pager->heightFieldList, 
				node);

			std::vector<std::vector<std::vector<Node*> > > objs;
			std::vector<Model*> models;

			objs.resize(_pager->heightFieldList.size());
			for (size_t i = 0; i < _pager->heightFieldList.size(); i++)
			{
				objs[i].resize(_pager->heightFieldList.size());
				for (size_t j = 0; j < _pager->heightFieldList[i].size(); j++)
				{
					for (size_t g = 0; g < objsPos[i][j].size(); g++)
					{
						Node * tree2 = node->clone();

						tree2->setTranslation(Vector3(
							objsPos[i][j][g]->x,
							objsPos[i][j][g]->y,
							objsPos[i][j][g]->z));

						_scene->addNode(tree2);

						objs[i][j].push_back(tree2);
					}
				}
			}
			_pager->objects = objs;
			_pager->objectsPosition = objsPos;
			char * file = (char*)fileName.c_str();
			_pager->objectsFilename.push_back(file);

			FilesSaver saver;

			threads.push_back(std::thread(
				&FilesSaver::saveObjectsPos,
				saver,
				_pager->objectsPosition,
				file));

			Threads t3;
			t3.objectBool();
			saverThreads.push_back(t3);

			Node * childs = node->getFirstChild();

			while(childs)
			{
				if (childs->getDrawable())
				{
					childs->getDrawable()->draw();
				}
				childs=childs->getNextSibling();
			}
			_pager->parameters.distanceMaxModelRender = node->getBoundingSphere().radius * 5;
		}
	}
}

void Main::generateNewTerrain()
{
	if (_pager)
	{
		_pager->removeObjects();

		_pager->removeTerrains();
		for (size_t i = 0; i < _pager->zoneList.size(); i++)
		{
			for (size_t j = 0; j < _pager->zoneList[i].size(); j++)
			{
				if (_pager->zoneList[i][j]->isLoaded() == true)
				{
					_pager->removeTerrain(i, j);
				}
			}
		}

		_pager->loadedTerrains.clear();
		_pager->loadedHeightfields.clear();
	}

	if (!_pager)
	{
		PagerParameters parameters;
		parameters.textureScale = 40;
		parameters.generatedBlendmaps = false;
		parameters.generatedNormalmaps = false;
		parameters.generatedHeightmaps = false;
		parameters.generatedObjects = false;
		parameters.skirtSize = 16;
		parameters.Debug = false;
		_pager = new Pager(parameters, _scene);
	}


	Control * control;
	Slider * slider;
	TextBox * textBox;
	RadioButton * radioButton;
	float xz = 0, y = 0;

	control = _generateTerrainsForm->getControl("DetailLevelsSlider");
	slider = (Slider *)control;
	_pager->parameters.lodQuality = slider->getValue();

	if (_pager->parameters.lodQuality == 0){ _pager->parameters.lodQuality = 1; }

	control = _generateTerrainsForm->getControl("HeightFieldSizeSlider");
	slider = (Slider *)control;
	int heightfieldResolution = slider->getValue();
	_pager->parameters.heightFieldResolution = heightfieldResolution;

	control = _generateTerrainsForm->getControl("MaxHeightSlider");
	slider = (Slider *)control;
	_pager->parameters.maxHeight = (slider->getValue());

	control = _generateTerrainsForm->getControl("MinHeightSlider");
	slider = (Slider *)control;
	_pager->parameters.minHeight = (slider->getValue());

	control = _generateTerrainsForm->getControl("PatchSizeSlider");
	slider = (Slider *)control;
	_pager->parameters.patchSize = (slider->getValue());

	control = _generateTerrainsForm->getControl("ScaleYSlider");
	slider = (Slider *)control;
	float scaleY = slider->getValue();

	control = _generateTerrainsForm->getControl("ScaleXZSlider");
	slider = (Slider *) control;
	int scaleXZ = slider->getValue();

	control = _generateTerrainsForm->getControl("SeedTextBox");
	textBox = (TextBox *)control;
	int seed = (strtol(textBox->getText(), NULL, 10));

	control = _generateTerrainsForm->getControl("TilesResolutionBox");
	textBox = (TextBox *)control;
	_pager->parameters.tilesResolution = (strtol(textBox->getText(), NULL, 10));

	control = _generateTerrainsForm->getControl("TerrainMaterialBox");
	textBox = (TextBox *)control;
	if (textBox)
	{
		_pager->parameters.terrainMaterialPath = textBox->getText();
	}

	//getting Noiser settings
	control = _generateTerrainsForm->getControl("SimplexNoiseRadio");
	radioButton = (RadioButton *)control;
	int noise;
	if (radioButton->isSelected()) { noise = 0; }

	control = _generateTerrainsForm->getControl("DiamondNoiseRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected()) { noise = 1; }

	control = _generateTerrainsForm->getControl("AmplitudeSlider");
	slider = (Slider *)control;
	int amplitude = (slider->getValue());

	control = _generateTerrainsForm->getControl("GainSlider");
	slider = (Slider *)control;
	int gain = (slider->getValue());

	control = _generateTerrainsForm->getControl("AmplitudeNegativeRadio");
	radioButton = (RadioButton *)control;
	bool bAmp = true;
	if (radioButton->isSelected()) { bAmp = false; }

	control = _generateTerrainsForm->getControl("GainNegativeRadio");
	radioButton = (RadioButton *)control;
	bool bGain = true;
	if (radioButton->isSelected()) { bGain = false; }

	_pager->parameters.scale = Vector3(scaleXZ, scaleY, scaleXZ);

	//creating heightfields
	TerrainGenerator terrainGenerator;

	_pager->heightFieldList = terrainGenerator.buildTerrainTiles(_pager->parameters.heightFieldResolution,
		_pager->parameters.tilesResolution,
		_pager->parameters.minHeight,
		_pager->parameters.maxHeight,
		noise,
		gain,
		amplitude,
		bGain,
		bAmp);

	_pager->parameters.boundingBox = ((_pager->parameters.heightFieldResolution-1) * scaleXZ);

	_pager->parameters.distanceTerrainLoad = _pager->parameters.boundingBox * .9;
	_pager->parameters.distanceTerrainUnload = _pager->parameters.boundingBox * 1;
	_pager->parameters.distanceTerrainMaxRender = _pager->parameters.boundingBox * 1;

	_pager->computePositions();

	//aligning vertexes for aesthetic
	TerrainEditor terrEdit;
	terrEdit.aligningTerrainsVertexes(_pager->heightFieldList);

	//configuring blend maps generation
	control = _generateBlendmapsForm->getControl("Blendmap1-Intensity");
	slider = (Slider *)control;
	int Intensity1 = slider->getValue();

	control = _generateBlendmapsForm->getControl("Blendmap2-Intensity");
	slider = (Slider *)control;
	int Intensity2 = slider->getValue();

	control = _generateBlendmapsForm->getControl("Blendmap1-Opacity");
	slider = (Slider *)control;
	int Opacity1 = slider->getValue();

	control = _generateBlendmapsForm->getControl("Blendmap2-Opacity");
	slider = (Slider *)control;
	int Opacity2 = slider->getValue();

	//generating blendmaps
	_pager->blendMaps = terrainGenerator.createTiledTransparentBlendImages(_pager->parameters.scale.y,
		_pager->parameters.scale.x,
		Intensity1,
		Intensity2,
		Opacity1,
		Opacity2,
		_pager->parameters.heightFieldResolution,
		_pager->heightFieldList);
	_pager->parameters.blendMapDIR = createTMPFolder();
	_pager->parameters.generatedBlendmaps = false;

	//configuring a thread to save blendmaps
	for (size_t i = 0; i < threads.size(); i++)
	{
		if (saverThreads[i].blendmap == true)
		{
			threads[i].~thread();
			threads.erase(threads.begin() + i);
			saverThreads.erase(saverThreads.begin() + i);
		}
	}

	FilesSaver saver;
	Threads t;
	t.blendBool();
	saverThreads.push_back(t);

	//create a thread to save blendmaps into PNG images format
	threads.push_back(std::thread(&FilesSaver::saveBlendmaps,
		saver,
		_pager->blendMaps,
		_pager->parameters.blendMapDIR,
		_pager->parameters.heightFieldResolution));

#define CREATE_NORMALMAPS
#ifdef CREATE_NORMALMAPS

	_pager->normalMaps =
		terrainGenerator.createNormalmaps(_pager->parameters.scale.y,
		_pager->parameters.scale.x,
		_pager->parameters.heightFieldResolution,
		_pager->heightFieldList);

	_pager->parameters.normalMapDIR = createTMPFolder();
	_pager->parameters.generatedNormalmaps = false;

	Threads t2;
	t2.normalBool();
	saverThreads.push_back(t2);

	threads.push_back(std::thread(&FilesSaver::saveNormalmaps,
		saver,
		_pager->normalMaps,
		_pager->parameters.normalMapDIR,
		_pager->parameters.heightFieldResolution));

#endif

	_pager->parameters.distanceTerrainLoad = (_pager->parameters.boundingBox * _pager->parameters.maxHeight ) * 0.9;
	_pager->parameters.distanceTerrainUnload = (_pager->parameters.boundingBox * _pager->parameters.maxHeight) * 1;
	_pager->parameters.distanceTerrainMaxRender = (_pager->parameters.boundingBox * _pager->parameters.maxHeight) * 1;

	_pager->pagingCheck();
	_camera.setPosition(Vector3(0, _pager->zoneList[0][0]->getObjectInside()->terrain->getHeight(0, 0), 0));

	_pager->parameters.distanceTerrainLoad = _pager->parameters.boundingBox * .9;
	_pager->parameters.distanceTerrainUnload = _pager->parameters.boundingBox * 1;
	_pager->parameters.distanceTerrainMaxRender = _pager->parameters.boundingBox * 1;
}

void Main::moveCamera(float elapsedTime)
{
	if (_moveForward)
	{
		_camera.moveForward(elapsedTime * MOVE_SPEED);
#ifdef POINTLIGHT_TEST
		_light->getNode()->setTranslation(_camera.getPosition());
#endif
	}
	if (_moveBackward)
	{
		_camera.moveBackward(elapsedTime * MOVE_SPEED);
#ifdef POINTLIGHT_TEST
		_light->getNode()->setTranslation(_camera.getPosition());
#endif
	}
	if (_moveLeft)
	{
		_camera.moveLeft(elapsedTime * MOVE_SPEED);
#ifdef POINTLIGHT_TEST
		_light->getNode()->setTranslation(_camera.getPosition());
#endif
	}
	if (_moveRight)
	{
		_camera.moveRight(elapsedTime * MOVE_SPEED);
#ifdef POINTLIGHT_TEST
		_light->getNode()->setTranslation(_camera.getPosition());
#endif
	}
}

void Main::update(float elapsedTime)
{

	int iBlendmap, iNormalmap, iObjectpos, total;
	bool blendmap = false, normalmap = false, objectpos = false;

	total = threads.size();

	for (size_t i = 0; i < threads.size(); i++)
	{
		if (threads[i].joinable())
		{
			if (saverThreads[i].blendmap == true)
			{
				blendmap = true;
				iBlendmap = i;
				continue;
			}
			if (saverThreads[i].normalmap == true)
			{
				normalmap = true;
				iNormalmap = i;
				continue;
			}
			if (saverThreads[i].objectPos == true)
			{
				objectpos = true;
				iObjectpos = i;
				continue;
			}
		}
	}
	if (blendmap == true && total == saverThreads.size())
	{
		_pager->blendMaps.clear();
		saverThreads.erase(saverThreads.begin() + iBlendmap);
		threads[iBlendmap].join();
		threads.erase(threads.begin() + iBlendmap);
		_pager->parameters.generatedBlendmaps = true;
	}
	if (normalmap == true && total == saverThreads.size())
	{
		_pager->normalMaps.clear();
		saverThreads.erase(saverThreads.begin() + iNormalmap);
		threads[iNormalmap].join();
		threads.erase(threads.begin() + iNormalmap);
		_pager->parameters.generatedNormalmaps = true;
	}
	if (objectpos == true && total == saverThreads.size()
		&& saverThreads.size() != 0 && threads.size() != 0)
	{
		saverThreads.erase(saverThreads.begin() + iObjectpos);
		threads[iObjectpos].join();
		threads.erase(threads.begin() + iObjectpos);
		_pager->parameters.generatedObjects = true;
	}

	if (_inputMode == INPUT_MODE::EDITING && RMB == true)
	{
		Control * control = _mainForm->getControl("LowerButton");
		Button * button = (Button *)control;
		if (button->getSkinColor(Control::State::NORMAL) == activeButton)
		{
			TerrainEditor TerrEdit;
			int nearest = _pager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
			if (nearest != -1)
			{
				Vector3 ringPos = _selectionRing->getPosition();

				std::vector<int> heightfields =
					TerrEdit.lower(
						BoundingSphere(ringPos, _selectionRing->getScale()),
						_pager->parameters.scale.x,
						_pager->parameters.scale.y,
						_pager->loadedTerrains,
						_pager->loadedHeightfields);

				_pager->reload(heightfields);
			}
		}
		control = _mainForm->getControl("FlattenButton");
		button = (Button *)control;
		if (button->getSkinColor(Control::State::NORMAL) == activeButton)
		{
			TerrainEditor TerrEdit;
			int nearest = _pager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
			if (nearest != -1)
			{
				Vector3 ringPos = _selectionRing->getPosition();

				std::vector<int> heightfields =
					TerrEdit.flatten(
						BoundingSphere(ringPos, _selectionRing->getScale()),
						_pager->parameters.scale.x,
						_pager->parameters.scale.y,
						_pager->loadedTerrains,
						_pager->loadedHeightfields);

				_pager->reload(heightfields);
			}
		}
		control = _mainForm->getControl("RaiseButton");
		button = (Button *)control;
		if (button->getSkinColor(Control::State::NORMAL) == activeButton)
		{
			TerrainEditor TerrEdit;
			int nearest = _pager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
			if (nearest != -1)
			{
				Vector3 ringPos = _selectionRing->getPosition();

				std::vector<int> heightfields =
					TerrEdit.raise(
						BoundingSphere(ringPos, _selectionRing->getScale()),
						_pager->parameters.scale.x,
						_pager->parameters.scale.y,
						_pager->loadedTerrains,
						_pager->loadedHeightfields);

				_pager->reload(heightfields);
			}
		}
	}

	moveCamera(elapsedTime);

	_mainForm->update(elapsedTime);
	_generateTerrainsForm->update(elapsedTime);
	_generateBlendmapsForm->update(elapsedTime);
	_generateObjectsForm->update(elapsedTime);
	_loadForm->update(elapsedTime);
	_saveForm->update(elapsedTime);
}

void Main::render(float elapsedTime)
{
	// Clear the color and depth buffers
	clear(CLEAR_COLOR_DEPTH, Vector4(0.0f, 0.5f, 1.0f, 1.0f), 1.0f, 0);

	if (_pager)
	{
		_pager->pagingCheck();

		_pager->render();
	}

	if (_selectionRing)
	{
		Node *ring = _selectionRing->_node->getFirstChild();
		while (ring)
		{
			if (ring->getDrawable())
			{
				ring->getDrawable()->draw();
			}
			ring = ring->getNextSibling();
		}
	}

	// Visit all the nodes in the scene for drawing
	//_scene->visit(this, &Main::drawScene);

	if (_mainForm->isVisible())
	{
		_mainForm->draw();
	}
	if (_generateTerrainsForm->isVisible())
	{
		_generateTerrainsForm->draw();
	}
	if (_generateBlendmapsForm->isVisible())
	{
		_generateBlendmapsForm->draw();
	}
	if (_loadForm->isVisible())
	{
		_loadForm->draw();
	}
	if (_saveForm->isVisible())
	{
		_saveForm->draw();
	}
	if (_generateObjectsForm->isVisible())
	{
		_generateObjectsForm->draw();
	}
}

bool Main::drawScene(Node* node)
{
	// If the node visited contains a model, draw it

	Model* model = dynamic_cast<Model*>(node->getDrawable());
	if (model)
	{
		float ActualDistance = _scene->getActiveCamera()->getNode()->getTranslationWorld().distance(node->getTranslationWorld());

		//if the range of view is too much small....it crash lol
		if (ActualDistance < _pager->parameters.distanceMaxModelRender)
		{
			if (_pager->parameters.generatedObjects = true)
			{
				//model->draw();
				node->getDrawable()->draw();
			}
		}
	}
	return true;
}

void Main::keyEvent(Keyboard::KeyEvent evt, int key)
{
	if (evt == Keyboard::KEY_PRESS)
	{
		switch (key)
		{
		case Keyboard::KEY_ESCAPE:
			clear(CLEAR_COLOR_DEPTH, Vector4(0,0,0,0), 0, 0);
			game.clearSchedule();
			exit();
			break;
		case Keyboard::KEY_W:
		case Keyboard::KEY_UP_ARROW:
			if (_inputMode == NAVIGATION)
			{
				_moveForward = true;
			}
			break;
		case Keyboard::KEY_S:
		case Keyboard::KEY_DOWN_ARROW:
			if (_inputMode == NAVIGATION)
			{
				_moveBackward = true;
			}
			break;
		case Keyboard::KEY_A:
		case Keyboard::KEY_LEFT_ARROW:
			if (_inputMode == NAVIGATION)
			{
				_moveLeft = true;
			}
			break;
		case Keyboard::KEY_D:
		case Keyboard::KEY_RIGHT_ARROW:
			if (_inputMode == NAVIGATION)
			{
				_moveRight = true;
			}
			break;
		case Keyboard::KEY_TAB:
			if (_inputMode == NAVIGATION)
			{
				Control * control=_mainForm->getControl("NavigateButton");
				RadioButton * button = (RadioButton*)control;
				button->setSelected(false);
				control = _mainForm->getControl("EditorButton");
				button = (RadioButton*)control;
				button->setSelected(true);

				_inputMode = EDITING;
				_mainForm->getControl("NavigateToolBar")->setVisible(false);
				_mainForm->getControl("GeneratorToolBar")->setVisible(false);
				_mainForm->getControl("TerrainEditorbar")->setVisible(true);
			}
			else if (_inputMode == EDITING)
			{
				Control * control = _mainForm->getControl("NavigateButton");
				RadioButton * button = (RadioButton*)control;
				button->setSelected(true);
				control = _mainForm->getControl("EditorButton");
				button = (RadioButton*)control;
				button->setSelected(false);

				_inputMode = NAVIGATION;
				_mainForm->getControl("TerrainEditorbar")->setVisible(false);
				_mainForm->getControl("GeneratorToolBar")->setVisible(false);
				_mainForm->getControl("NavigateToolBar")->setVisible(true);
			}
			break;
		}

	}
	else if (evt == Keyboard::KEY_RELEASE)
	{
		switch (key)
		{
		case Keyboard::KEY_W:
		case Keyboard::KEY_UP_ARROW:
			if (_inputMode == NAVIGATION)
			{
				_moveForward = false;
			}
			break;
		case Keyboard::KEY_S:
		case Keyboard::KEY_DOWN_ARROW:
			if (_inputMode == NAVIGATION)
			{
				_moveBackward = false;
			}
			break;
		case Keyboard::KEY_A:
		case Keyboard::KEY_LEFT_ARROW:
			if (_inputMode == NAVIGATION)
			{
				_moveLeft = false;
			}
			break;
		case Keyboard::KEY_D:
		case Keyboard::KEY_RIGHT_ARROW:
			if (_inputMode == NAVIGATION)
			{
				_moveRight = false;
			}
			break;
		}
	}
}

bool Main::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
	if (evt == Mouse::MOUSE_PRESS_RIGHT_BUTTON)
	{
		RMB = true;
		return true;
	}
	if (evt == Mouse::MOUSE_RELEASE_RIGHT_BUTTON)
	{
		RMB = false;
		return true;
	}
	if (evt == Mouse::MOUSE_PRESS_LEFT_BUTTON)
	{
		if (_inputMode == INPUT_MODE::EDITING)
		{
			Ray pickRay;
			_scene->getActiveCamera()->pickRay(gameplay::Rectangle(0, 0, getWidth(), getHeight()), x, y, &pickRay);
			pickRay.setOrigin(_camera.getPosition());

			for (size_t i = 0; i < _pager->loadedTerrains.size(); i++)
			{
				TerrainHitFilter hitFilter(_pager->loadedTerrains[i]);
				PhysicsController::HitResult hitResult;
				if (Game::getInstance()->getPhysicsController()->rayTest(pickRay, 1000000, &hitResult, &hitFilter))
				{
					if (hitResult.object == _pager->loadedTerrains[i]->getNode()->getCollisionObject())
					{
						_selectionRing->setPosition(hitResult.point.x, hitResult.point.z, _pager->loadedTerrains[i]);
					}
				}
			}
		}
		LMB = true;
		return true;
	}
	if (evt == Mouse::MOUSE_RELEASE_LEFT_BUTTON)
	{
		LMB = false;
		return true;
	}
	if (evt == Mouse::MOUSE_MOVE)
	{
		int deltaX = x - _prevX;
		int deltaY = y - _prevY;
		if (_inputMode == NAVIGATION && RMB == true)
		{
			float pitch = -MATH_DEG_TO_RAD(deltaY * 0.5f);
			float yaw = MATH_DEG_TO_RAD(deltaX * 0.5f);
			_camera.rotate(yaw, pitch);
		}
		_prevX = x;
		_prevY = y;
		return true;
	}
	return false;
}