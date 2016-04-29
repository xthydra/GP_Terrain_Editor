/*
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

// Declare our game instance
Main game;

Main::Main()
	: _scene(NULL),
	_moveForward(false),
	_moveBackward(false),
	_moveLeft(false),
	_moveRight(false),
	_prevX(0),
	_prevY(0),
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
	_camera = new FirstPersonCamera();
	_camera->initialize(1.0f, 60000.0f, 45);
	_camera->rotate(0.0f, -MATH_DEG_TO_RAD(10));
	_camera->setMoveSpeed(10);
	_scene->addNode(_camera->getRootNode());
	_scene->setActiveCamera(_camera->getCamera());

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
	
#ifdef EXAMPLE
	//Generate heightfields for the terrain pager
	PagerParameters parameters;
	parameters.generatedBlendmaps = false;
	parameters.generatedNormalmaps = false;
	parameters.generatedHeightmaps = false;
	parameters.generatedObjects = false;
	parameters.zoneResolution = 2;
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

	parameters.scale = Vector3(parameters.heightFieldResolution, ((parameters.heightFieldResolution*parameters.zoneResolution)*0.10), parameters.heightFieldResolution);

	parameters.boundingBox = (parameters.heightFieldResolution * parameters.heightFieldResolution) - parameters.heightFieldResolution;

	TerrainGenerator terrainGenerator;
	std::vector<std::vector<gameplay::HeightField*>> heightfields;
	heightfields = terrainGenerator.buildTerrainTiles(parameters.heightFieldResolution,
		parameters.zoneResolution,
		parameters.minHeight,
		parameters.maxHeight,
		0,
		1,
		1,
		true,
		true);

	parameters.debug = false;

	parameters.distanceTerrainLoad = parameters.boundingBox * .9;
	parameters.distanceTerrainUnload = parameters.boundingBox * 1;
	parameters.distanceTerrainMaxRender = parameters.boundingBox * 1;

	_pager = new Pager(parameters, _scene);

	_pager->computePositions();

	//putting heightfields inside the zone list buffer
	for (size_t i = 0; i < heightfields.size(); i++)
	{
		for (size_t f = 0; f < heightfields[i].size(); f++)
		{
			_pager->zoneList[i][f]->heightField = heightfields[i][f];
		}
	}

	//creating and setting up the UI
	interface = new UI(_scene, this, _camera, _pager);

#ifdef CREATE_BLENDMAPS
	std::vector<std::vector<std::vector<std::vector<unsigned char>>>> blendMaps =
		terrainGenerator.createTiledTransparentBlendImages(_pager->parameters.scale.y,
														   _pager->parameters.scale.x,
														   0,
														   50,
														   0,
														   100,
														   _pager->parameters.heightFieldResolution,
														   heightfields);
	_pager->parameters.tmpFolder = createTMPFolder();

	//putting blendmaps and assigning textures path in the zone list buffer
	for (size_t i = 0; i < blendMaps.size(); i++)
	{
		for (size_t f = 0; f < blendMaps[i].size(); f++)
		{
			_pager->zoneList[i][f]->blendMaps = blendMaps[i][f];

			_pager->zoneList[i][f]->texturesLocation.push_back(std::string("res/common/terrain/grass.dds"));
			_pager->zoneList[i][f]->texturesLocation.push_back(std::string("res/common/terrain/dirt.dds"));
			_pager->zoneList[i][f]->texturesLocation.push_back(std::string("res/common/terrain/rock.dds"));
		}
	}

	FilesSaver saver;
	Threads t;
	t.blendBool();
	saverThreads.push_back(t);

	threads.push_back(std::thread(
		&FilesSaver::saveBlendmaps,
		saver,
		blendMaps,
		_pager->parameters.tmpFolder,
		_pager->parameters.heightFieldResolution));

#endif

#ifdef CREATE_NORMALMAPS
	std::vector<std::vector<std::vector<unsigned char>>> normalMaps =
		terrainGenerator.createNormalmaps(parameters.scale.y, 
										  _pager->parameters.scale.x,
										  _pager->parameters.heightFieldResolution, 
										  heightfields);

	//putting blendmaps and assigning textures path in the zone list buffer
	for (size_t i = 0; i < normalMaps.size(); i++)
	{
		for (size_t f = 0; f < normalMaps[i].size(); f++)
		{
			_pager->zoneList[i][f]->normalMap = normalMaps[i][f];
		}
	}

	Threads t2;
	t2.normalBool();
	saverThreads.push_back(t2);

	threads.push_back(std::thread(
		&FilesSaver::saveNormalmaps,
		saver,
		normalMaps,
		_pager->parameters.tmpFolder,
		_pager->parameters.heightFieldResolution));

#endif
	_camera->setPosition(Vector3(0, ((parameters.heightFieldResolution*parameters.zoneResolution) * 25), 0));
	_camera->rotate(0, -90);

	_selectionRing = new SelectionRing(_scene);

	interface->selectionRing = _selectionRing;

#ifdef CREATE_TREES
#ifdef POINTLIGHT_TEST
	Control * control =	interface->generateObjectsForm->getControl("ObjectTextBox");
	TextBox * textBox = (TextBox *)control;
	textBox->setText("res/common/Tree_pointlight.scene");
	generateObjectsPosition();
#else
	generateObjectsPosition();
#endif
#endif
#elif
	//creating and setting up the UI
	interface = new UI(_scene, this, _camera, _pager);
#endif

}

void Main::finalize()
{
	SAFE_RELEASE(_light);	
	SAFE_RELEASE(_selectionRing);
	SAFE_RELEASE(_scene);
	delete _binding;
}

void Main::controlEvent(Control* control, Control::Listener::EventType evt)
{
	//most of the user interface events will happen in that function
	interface->controlEvents(control, evt);
	_inputMode = (INPUT_MODE)interface->inputMode;

	//the other part of the UI events will be called here
	if (strcmp(control->getId(), "ConfirmGenerateTerrainsButton") == 0)
	{
		generateNewTerrain();
	}
	else if (strcmp(control->getId(), "ConfirmGenerateBlendmapsButton") == 0)
	{
		generateNewBlendmaps();
	}
	else if (strcmp(control->getId(), "ConfirmLoadButton") == 0)
	{
		load();
	}
	else if (strcmp(control->getId(), "RAWHeightmapsButton") == 0)
	{
		std::vector<std::vector<HeightField*>> heightFields;
		heightFields.resize(_pager->zoneList.size());
		for (size_t i = 0; i < _pager->zoneList.size(); i++)
		{
			for (size_t f = 0; f < _pager->zoneList.size(); f++)
			{
				heightFields[i].push_back(_pager->zoneList[i][f]->heightField);
			}
		}

		FilesSaver saver;
		Threads t;
		t.heightBool();
		saverThreads.push_back(t);

		threads.push_back(std::thread(
			&FilesSaver::saveRAWHeightmaps,
			saver,
			heightFields,
			_pager->parameters.tmpFolder));
	}
	else if (strcmp(control->getId(), "BlendmapsButton") == 0)
	{
		FilesSaver saver;
		Threads t;
		t.blendBool();
		saverThreads.push_back(t);

		std::vector < std::vector < std::vector<std::vector<unsigned char>>>> blendMaps;
		blendMaps.resize(_pager->zoneList.size());
		for (size_t i = 0; i < _pager->zoneList.size(); i++)
		{
			blendMaps[i].resize(_pager->zoneList[i].size());
			for (size_t f = 0; f < _pager->zoneList.size(); f++)
			{
				blendMaps[i][f].push_back(_pager->zoneList[i][f]->blendMaps[0]);
				blendMaps[i][f].push_back(_pager->zoneList[i][f]->blendMaps[1]);
			}
		}

		threads.push_back(std::thread(
			&FilesSaver::saveBlendmaps,
			saver,
			blendMaps,
			_pager->parameters.tmpFolder,
			_pager->parameters.heightFieldResolution));

		//TODO: i really dont know if i need to clear the folder containing the blendmaps before overwriting
		//cuz at somepoint i will probably allow the user to change textures files which will probably end up corrupting the way files are loaded
	}
	else if (strcmp(control->getId(), "ObjectsButton") == 0)
	{
		FilesSaver saver;

		//TODO: the index im sending to the function that save objects positions
		//doesn't adapt to conditional requests

		threads.push_back(std::thread(
			&FilesSaver::saveObjectsPos,
			saver,
			_pager->objectsPosition,
			1,
			_pager->parameters.tmpFolder));

		Threads t;
		t.objectBool();
		saverThreads.push_back(t);
	}
	else if (strcmp(control->getId(), "ConfirmGenerateObjectsButton") == 0)
	{
		generateObjectsPosition();
	}
}

void Main::load()
{
	Control * control;
	TextBox * textBox;
	RadioButton * radioButton;

	control = interface->loadForm->getControl("FolderTextBox");
	textBox = (TextBox *)control;
	const char * folder = textBox->getText();

	control = interface->loadForm->getControl("RawHeightfieldRadio");
	radioButton = (RadioButton *)control;
	std::vector<std::vector<HeightField*>> heightfields;
	if (radioButton->isSelected())
	{
		_pager->removeObjects();
		_pager->removeTerrains();
		FilesLoader load;
		heightfields = load.loadRAWHeightmaps(folder);
		_pager->parameters.tmpFolder = (char*)folder;
		_pager->parameters.zoneResolution = load.tilesResolution;
	}

	for (size_t i = 0; i < heightfields.size(); i++)
	{
		for (size_t f = 0; f < heightfields[i].size() ; f++)
		{
			_pager->zoneList[i][f]->heightField = heightfields[i][f];
		}
	}

	control = interface->loadForm->getControl("BlendmapRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		FilesLoader load;
		//TODO : i should use the load function to check against the folder and the tiles resolution and return a bool
		//about if the folder have the same tiles resolution as the currently generated terrains
		load.loadBlendmaps(folder);
		_pager->parameters.tmpFolder = (char*)folder;
		//_pager->parameters.tilesResolution = load.tilesResolution;
		_pager->reloadTerrains();
	}

	control = interface->loadForm->getControl("ObjectsPosRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		//TODO : i should use the load function to check against the folder and the tiles resolution and return a bool
		//about if the folder have the same tiles resolution as the currently generated terrains
		_pager->removeObjects();
		FilesLoader load;
		_pager->objectsPosition = load.loadObjectsPos(folder);
		//TODO : std::string objectName=load.fileName;
		//TODO : _pager->createObjects(objectName,_pager->parameters.objectsPosition);
	}

	control = interface->loadForm->getControl("NormalmapRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		//TODO : i should use the load function to check against the folder and the tiles resolution and return a bool
		//about if the folder have the same tiles resolution as the currently generated terrains
		FilesLoader load;
		load.loadNormalmaps(folder);
		_pager->parameters.tmpFolder = (char*)folder;
		_pager->reloadTerrains();
	}

}

void Main::generateNewBlendmaps()
{
	if (_pager)
	{
		Control * control;
		Slider * slider;

		control = interface->generateBlendmapsForm->getControl("Blendmap1-Intensity");
		slider = (Slider *)control;
		int Intensity1 = slider->getValue();

		control = interface->generateBlendmapsForm->getControl("Blendmap2-Intensity");
		slider = (Slider *)control;
		int Intensity2 = slider->getValue();

		control = interface->generateBlendmapsForm->getControl("Blendmap1-Opacity");
		slider = (Slider *)control;
		int Opacity1 = slider->getValue();

		control = interface->generateBlendmapsForm->getControl("Blendmap2-Opacity");
		slider = (Slider *)control;
		int Opacity2 = slider->getValue();

		TerrainGenerator terrainGenerator;

		std::vector<std::vector<HeightField*>> heightfields;
		heightfields.resize(_pager->zoneList.size());
		for (size_t i = 0; i < _pager->zoneList.size(); i++)
		{
			for (size_t f = 0; f < _pager->zoneList[i].size(); f++)
			{
				heightfields[i].push_back(_pager->zoneList[i][f]->heightField);
			}
		}

		std::vector<std::vector<std::vector<std::vector<unsigned char>>>> blendMaps =
			terrainGenerator.createTiledTransparentBlendImages(
			_pager->parameters.scale.y,
			_pager->parameters.scale.x,
			Intensity1,
			Intensity2,
			Opacity1,
			Opacity2,
			_pager->parameters.heightFieldResolution,
			heightfields);

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
			blendMaps,
			_pager->parameters.tmpFolder,
			_pager->parameters.heightFieldResolution));

		_pager->reloadTerrains();
	}
}

void Main::generateObjectsPosition()
{
	if (_pager)
	{
		if (_pager->zoneList.size() > 0)
		{
			_pager->removeObjects();

			Control * control;
			TextBox * textBox;
			std::string fileName, material;

			control = interface->generateObjectsForm->getControl("MaterialTextBox");
			textBox = (TextBox *)control;
			if (textBox)
			{
				material = textBox->getText();
			}

			control = interface->generateObjectsForm->getControl("ObjectTextBox");
			textBox = (TextBox *)control;
			if (textBox)
			{
				fileName = textBox->getText();
			}

			control = interface->generateObjectsForm->getControl("ChanceTextBox");
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

			std::vector<std::vector<HeightField*>> heightfields;
			heightfields.resize(_pager->zoneList.size());
			for (size_t i = 0; i < _pager->zoneList.size(); i++)
			{
				for (size_t f = 0; f < _pager->zoneList[i].size(); f++)
				{
					heightfields[i].push_back(_pager->zoneList[i][f]->heightField);
				}
			}

			TerrainGenerator terrainGenerator;
			std::vector<std::vector<std::vector<Vector3*> > > objsPos = terrainGenerator.generateObjectsPosition(worldScale, 
				_pager->parameters.scale.y, 
				_pager->parameters.scale.x,
				2, 
				_pager->parameters.heightFieldResolution, 
				heightfields, 
				node);

			std::vector<std::vector<std::vector<Node*> > > objs;
			std::vector<Model*> models;

			objs.resize(_pager->zoneList.size());
			for (size_t i = 0; i < _pager->zoneList.size(); i++)
			{
				objs[i].resize(_pager->zoneList.size());
				for (size_t j = 0; j < _pager->zoneList[i].size(); j++)
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
			//char * file = (char*)fileName.c_str();
			//_pager->objectsFilename.push_back((char*)fileName.c_str());

			FilesSaver saver;

			//TODO: the index im sending to the function that save objects positions
			//doesn't adapt to conditional requests

			threads.push_back(std::thread(
				&FilesSaver::saveObjectsPos,
				saver,
				_pager->objectsPosition,
				1,
				_pager->parameters.tmpFolder));

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
		parameters.debug = false;
		_pager = new Pager(parameters, _scene);
	}

	Control * control;
	Slider * slider;
	TextBox * textBox;
	RadioButton * radioButton;
	float xz = 0, y = 0;

	control = interface->generateTerrainsForm->getControl("DetailLevelsSlider");
	slider = (Slider *)control;
	_pager->parameters.lodQuality = slider->getValue();

	if (_pager->parameters.lodQuality == 0){ _pager->parameters.lodQuality = 1; }

	control = interface->generateTerrainsForm->getControl("HeightFieldSizeSlider");
	slider = (Slider *)control;
	int heightfieldResolution = slider->getValue();
	_pager->parameters.heightFieldResolution = heightfieldResolution;

	control = interface->generateTerrainsForm->getControl("MaxHeightSlider");
	slider = (Slider *)control;
	_pager->parameters.maxHeight = (slider->getValue());

	control = interface->generateTerrainsForm->getControl("MinHeightSlider");
	slider = (Slider *)control;
	_pager->parameters.minHeight = (slider->getValue());

	control = interface->generateTerrainsForm->getControl("PatchSizeSlider");
	slider = (Slider *)control;
	_pager->parameters.patchSize = (slider->getValue());

	control = interface->generateTerrainsForm->getControl("ScaleYSlider");
	slider = (Slider *)control;
	float scaleY = slider->getValue();

	control = interface->generateTerrainsForm->getControl("ScaleXZSlider");
	slider = (Slider *) control;
	int scaleXZ = slider->getValue();

	control = interface->generateTerrainsForm->getControl("SeedTextBox");
	textBox = (TextBox *)control;
	int seed = (strtol(textBox->getText(), NULL, 10));

	control = interface->generateTerrainsForm->getControl("TilesResolutionBox");
	textBox = (TextBox *)control;
	_pager->parameters.zoneResolution = (strtol(textBox->getText(), NULL, 10));

	control = interface->generateTerrainsForm->getControl("TerrainMaterialBox");
	textBox = (TextBox *)control;
	if (textBox)
	{
		_pager->parameters.terrainMaterialPath = textBox->getText();
	}

	//getting Noiser settings
	control = interface->generateTerrainsForm->getControl("SimplexNoiseRadio");
	radioButton = (RadioButton *)control;
	int noise;
	if (radioButton->isSelected()) { noise = 0; }

	control = interface->generateTerrainsForm->getControl("DiamondNoiseRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected()) { noise = 1; }

	control = interface->generateTerrainsForm->getControl("AmplitudeSlider");
	slider = (Slider *)control;
	int amplitude = (slider->getValue());

	control = interface->generateTerrainsForm->getControl("GainSlider");
	slider = (Slider *)control;
	int gain = (slider->getValue());

	control = interface->generateTerrainsForm->getControl("AmplitudeNegativeRadio");
	radioButton = (RadioButton *)control;
	bool bAmp = true;
	if (radioButton->isSelected()) { bAmp = false; }

	control = interface->generateTerrainsForm->getControl("GainNegativeRadio");
	radioButton = (RadioButton *)control;
	bool bGain = true;
	if (radioButton->isSelected()) { bGain = false; }

	_pager->parameters.scale = Vector3(scaleXZ, scaleY, scaleXZ);

	//creating heightfields
	TerrainGenerator terrainGenerator;

	std::vector<std::vector<HeightField*>> heightfields = terrainGenerator.buildTerrainTiles(_pager->parameters.heightFieldResolution,
		_pager->parameters.zoneResolution,
		_pager->parameters.minHeight,
		_pager->parameters.maxHeight,
		noise,
		gain,
		amplitude,
		bGain,
		bAmp);

	_pager->parameters.boundingBox = ((_pager->parameters.heightFieldResolution - 1) * scaleXZ);

	_pager->parameters.distanceTerrainLoad = _pager->parameters.boundingBox * .9;
	_pager->parameters.distanceTerrainUnload = _pager->parameters.boundingBox * 1;
	_pager->parameters.distanceTerrainMaxRender = _pager->parameters.boundingBox * 1;

	_pager->computePositions();

	for (size_t i = 0; i < heightfields.size(); i++)
	{
		for (size_t f = 0; f < heightfields[i].size(); f++)
		{
			_pager->zoneList[i][f]->heightField=heightfields[i][f];
		}
	}

	//aligning vertexes for aesthetic
	TerrainEditor terrEdit;
	terrEdit.aligningTerrainsVertexes(heightfields);

	//configuring blend maps generation
	control = interface->generateBlendmapsForm->getControl("Blendmap1-Intensity");
	slider = (Slider *)control;
	int Intensity1 = slider->getValue();

	control = interface->generateBlendmapsForm->getControl("Blendmap2-Intensity");
	slider = (Slider *)control;
	int Intensity2 = slider->getValue();

	control = interface->generateBlendmapsForm->getControl("Blendmap1-Opacity");
	slider = (Slider *)control;
	int Opacity1 = slider->getValue();

	control = interface->generateBlendmapsForm->getControl("Blendmap2-Opacity");
	slider = (Slider *)control;
	int Opacity2 = slider->getValue();

	//generating blendmaps
	std::vector<std::vector<std::vector<std::vector<unsigned char>>>> blendmaps = terrainGenerator.createTiledTransparentBlendImages(_pager->parameters.scale.y,
		_pager->parameters.scale.x,
		Intensity1,
		Intensity2,
		Opacity1,
		Opacity2,
		_pager->parameters.heightFieldResolution,
		heightfields);

	_pager->parameters.generatedBlendmaps = false;

	for (size_t i = 0; i < _pager->parameters.zoneResolution; i++)
	{
		for (size_t j = 0; j < _pager->parameters.zoneResolution; j++)
		{
			_pager->zoneList[i][j]->texturesLocation.push_back(std::string("res/common/terrain/grass.dds"));
			_pager->zoneList[i][j]->texturesLocation.push_back(std::string("res/common/terrain/dirt.dds"));
			_pager->zoneList[i][j]->texturesLocation.push_back(std::string("res/common/terrain/rock.dds"));
		}
	}

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
		blendmaps,
		_pager->parameters.tmpFolder,
		_pager->parameters.heightFieldResolution));

#define CREATE_NORMALMAPS
#ifdef CREATE_NORMALMAPS
	std::vector<std::vector<std::vector<unsigned char>>> normalMaps =
		terrainGenerator.createNormalmaps(_pager->parameters.scale.y,
		_pager->parameters.scale.x,
		_pager->parameters.heightFieldResolution,
		heightfields);

	_pager->parameters.generatedNormalmaps = false;

	Threads t2;
	t2.normalBool();
	saverThreads.push_back(t2);

	threads.push_back(std::thread(&FilesSaver::saveNormalmaps,
		saver,
		normalMaps,
		_pager->parameters.tmpFolder,
		_pager->parameters.heightFieldResolution));

#endif
	_pager->parameters.distanceTerrainLoad = _pager->parameters.boundingBox * 1.3;
	_pager->parameters.distanceTerrainUnload = _pager->parameters.boundingBox * 1.4;
	_pager->parameters.distanceTerrainMaxRender = _pager->parameters.boundingBox * 1.4;

	_camera->setPosition(Vector3(0, 0, 0));
	_pager->pagingCheck();
	_camera->setPosition(Vector3(0, _pager->zoneList[0][0]->getObjectInside()->terrain->getHeight(0, 0), 0));
}

void Main::moveCamera(float elapsedTime)
{
	if (_moveForward)
	{
		_camera->moveForward(elapsedTime);
#ifdef POINTLIGHT_TEST
		_light->getNode()->setTranslation(_camera->getPosition());
#endif
	}
	if (_moveBackward)
	{
		_camera->moveBackward(elapsedTime);
#ifdef POINTLIGHT_TEST
		_light->getNode()->setTranslation(_camera->getPosition());
#endif
	}
	if (_moveLeft)
	{
		_camera->moveLeft(elapsedTime);
#ifdef POINTLIGHT_TEST
		_light->getNode()->setTranslation(_camera->getPosition());
#endif
	}
	if (_moveRight)
	{
		_camera->moveRight(elapsedTime);
#ifdef POINTLIGHT_TEST
		_light->getNode()->setTranslation(_camera->getPosition());
#endif
	}
}

void Main::update(float elapsedTime)
{
	//references to prevent segmentation fault
	int iBlendmap, iNormalmap, iObjectpos, iHeightmap, total;
	bool blendmap = false, normalmap = false, objectpos = false, heightmap=false;

	total = threads.size();

	//check if a thread is finished the function
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
			if (saverThreads[i].heightmap == true)
			{
				heightmap = true;
				iHeightmap = i;
				continue;
			}
		}
	}

	//if a thread finished then delete references, merge the thread with the main thread and send the confirmation to the Pager class
	if (heightmap == true && total == saverThreads.size())
	{
		saverThreads.erase(saverThreads.begin() + iHeightmap);
		threads[iHeightmap].join();
		threads.erase(threads.begin() + iHeightmap);
	}
	if (blendmap == true && total == saverThreads.size())
	{
		saverThreads.erase(saverThreads.begin() + iBlendmap);
		threads[iBlendmap].join();
		threads.erase(threads.begin() + iBlendmap);
		_pager->parameters.generatedBlendmaps = true;
	}
	if (normalmap == true && total == saverThreads.size())
	{
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

	//check against user interface and right click mouse event
	if (_inputMode == INPUT_MODE::EDITING && RMB == true)
	{
		//if the paint tool bar is visible
		Control * control = interface->mainForm->getControl("PaintToolbar");
		Container * container = (Container*)control;
		bool paintIsVisible = container->isVisible();

		if (paintIsVisible == false)
		{
			control = interface->mainForm->getControl("LowerButton");
			Button * button = (Button *)control;
			if (button->getSkinColor(Control::State::NORMAL) == interface->activeButton)
			{
				//get the nearest terrain according to the camera focus in 3D world space
				TerrainEditor TerrEdit;
				int nearest = _pager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
				if (nearest != -1)
				{
					//get the selection ring position
					Vector3 ringPos = _selectionRing->getPosition();

					//query the heightfields from the editing function
					std::vector<int> heightfields =
						TerrEdit.lower(
							BoundingSphere(ringPos, _selectionRing->getScale()),
							_pager->parameters.scale.x,
							_pager->parameters.scale.y,
							_pager->loadedTerrains,
							_pager->loadedHeightfields);	

					//reload the terrains with new heightfields
					_pager->reload(heightfields);
				}
			}
			//and so on
			control = interface->mainForm->getControl("FlattenButton");
			button = (Button *)control;
			if (button->getSkinColor(Control::State::NORMAL) == interface->activeButton)
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
					//_pager->reloadTerrains();
				}
			}
			control = interface->mainForm->getControl("RaiseButton");
			button = (Button *)control;
			if (button->getSkinColor(Control::State::NORMAL) == interface->activeButton)
			{
				TerrainEditor TerrEdit;
				int nearest = _pager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
				if (nearest != -1)
				{
					Vector3 ringPos = _selectionRing->getPosition();

					std::vector<int> modified =
						TerrEdit.raise(
							BoundingSphere(ringPos, _selectionRing->getScale()),
							_pager->parameters.scale.x,
							_pager->parameters.scale.y,
							_pager->loadedTerrains,
							_pager->loadedHeightfields);

					_pager->reload(modified);
				}
			}
			control = interface->mainForm->getControl("SmoothButton");
			button = (Button *)control;
			if (button->getSkinColor(Control::State::NORMAL) == interface->activeButton)
			{
				TerrainEditor TerrEdit;
				int nearest = _pager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
				if (nearest != -1)
				{
					Vector3 ringPos = _selectionRing->getPosition();

					std::vector<int> modified =
						TerrEdit.raise(
							BoundingSphere(ringPos, _selectionRing->getScale()),
							_pager->parameters.scale.x,
							_pager->parameters.scale.y,
							_pager->loadedTerrains,
							_pager->loadedHeightfields);

					_pager->reload(modified);
				}
			}
		}
		if (paintIsVisible == true)
		{
			bool paint = false,draw=false,erase=false;
			control = interface->mainForm->getControl("DrawButton");
			RadioButton * button = (RadioButton *)control;
			if (button->isSelected() == true)
			{
				paint = true;
				draw = true;
			}
			control = interface->mainForm->getControl("EraseButton");
			button = (RadioButton *)control;
			if (button->isSelected() == true)
			{
				paint = true;
				erase = true;
			}
			if (paint == true)
			{
				TerrainEditor TerrEdit;
				std::vector<std::vector<unsigned char>> blend1, blend2;
				std::vector<Vector3> fieldsPos;

				for (size_t i = 0; i < _pager->loadedTerrains.size(); i++)
				{
					//trying to find a loaded terrain index by using world space position
					//to find the blendmaps inside the zonelist vector
					int posX = _pager->loadedTerrains[i]->getNode()->getTranslationWorld().x;
					int posZ = _pager->loadedTerrains[i]->getNode()->getTranslationWorld().z;

					int offsetX = 0, offsetZ = 0;
					if (posX > 0) { offsetX = 1; }
					if (posZ > 0) { offsetZ = 1; }
					int sPosX = ((posX / _pager->parameters.scale.x) + offsetX);
					int sPosZ = ((posZ / _pager->parameters.scale.x) + offsetZ);

					posX = (posX / _pager->parameters.scale.x) / (_pager->parameters.heightFieldResolution - 1);
					posZ = (posZ / _pager->parameters.scale.x) / (_pager->parameters.heightFieldResolution - 1);

					//blend1.push_back(_pager->blendMaps[posX][posZ][0]);
					//blend2.push_back(_pager->blendMaps[posX][posZ][1]);
					//blend1.push_back(_pager->blendMaps[posZ][posX][0]);
					//blend2.push_back(_pager->blendMaps[posZ][posX][1]);
					blend1.push_back(_pager->zoneList[posZ][posX]->blendMaps[0]);
					blend2.push_back(_pager->zoneList[posZ][posX]->blendMaps[1]);
					fieldsPos.push_back(Vector3(sPosX, 0, sPosZ));
				}

				//trying to know what to paint
				int selectedTexture = 0;

				control = interface->mainForm->getControl("Texture0");
				RadioButton * button = (RadioButton *)control;
				if (button->isSelected())
				{
					selectedTexture = 0;
				}
				control = interface->mainForm->getControl("Texture1");
				button = (RadioButton *)control;
				if (button->isSelected())
				{
					selectedTexture = 1;
				}
				control = interface->mainForm->getControl("Texture2");
				button = (RadioButton *)control;
				if (button->isSelected())
				{
					selectedTexture = 2;
				}

				bool drawOrErase;
				if (draw == true) { drawOrErase = true; }
				if (erase == true) { drawOrErase = false; }

				//then it paint
				std::vector<std::vector<Vector3>> modified = TerrEdit.paint(blend1,
					blend2,
					fieldsPos,
					selectedTexture,
					Vector2(_selectionRing->getPosition().x / _pager->parameters.scale.x, _selectionRing->getPosition().z / _pager->parameters.scale.x),
					_pager->parameters.heightFieldResolution,
					_selectionRing->getScale() / _pager->parameters.scale.x,
					drawOrErase);

				//then it reload terrains, but there's possibly a weird unoptimized function here or above
				//im not sure where i screwed up yet
				for (size_t i = 0; i < modified[0].size(); i++)
				{
					int z = (modified[0][i].x / _pager->parameters.heightFieldResolution);
					int x = (modified[0][i].z / _pager->parameters.heightFieldResolution);

					_pager->zoneList[x][z]->blendMaps[0] = TerrEdit.blend1[i];
					FilesSaver saver;
					saver.saveBlendmap(
						_pager->zoneList[x][z]->blendMaps[0],
						1,
						_pager->parameters.tmpFolder,
						x,
						z,
						_pager->parameters.heightFieldResolution
						);
				}
				for (size_t i = 0; i < modified[1].size(); i++)
				{
					int z = (modified[1][i].x / _pager->parameters.heightFieldResolution);
					int x = (modified[1][i].z / _pager->parameters.heightFieldResolution);

					_pager->zoneList[x][z]->blendMaps[1] = TerrEdit.blend2[i];
					FilesSaver saver;
					saver.saveBlendmap(
						_pager->zoneList[x][z]->blendMaps[1],
						2,
						_pager->parameters.tmpFolder,
						x,
						z,
						_pager->parameters.heightFieldResolution
						);
				}
				//TODO: i should only reload modified terrain
				//check every modified blendmaps against loaded terrains positions to know which of those to reload
				/*std::vector<int> toReload;
				for (size_t i = 0; i < modified.size(); i++)
				{
					for (size_t g = 0; g < modified[i].size(); g++)
					{
						if (toReload.size() != 0)
						{
							for (size_t f = 0; f < toReload.size(); f++)
							{

							}
						}
						else
						{
							for (size_t j = 0; j < _pager->loadedTerrains.size(); j++)
							{
								int z = (modified[1][i].x / _pager->parameters.heightFieldResolution);
								int x = (modified[1][i].z / _pager->parameters.heightFieldResolution);


								toReload
							}
						}
					}
				}*/
				_pager->reloadTerrains();
				//_pager->reload(modified[0]);
				//_pager->reload(modified[1]);
			}
		}
	}

	moveCamera(elapsedTime);

	interface->updateForms(elapsedTime);
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

	interface->renderForms();
}

bool Main::drawScene(Node* node)
{
	return false;
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
				Control * control= interface->mainForm->getControl("NavigateButton");
				RadioButton * button = (RadioButton*)control;
				button->setSelected(false);
				control = interface->mainForm->getControl("EditorButton");
				button = (RadioButton*)control;
				button->setSelected(true);

				_inputMode = EDITING;
				interface->inputMode = (UI::INPUT_MODE)EDITING;
				interface->mainForm->getControl("NavigateToolBar")->setVisible(false);
				interface->mainForm->getControl("GeneratorToolBar")->setVisible(false);
				interface->mainForm->getControl("TerrainEditorbar")->setVisible(true);
			}
			else if (_inputMode == EDITING)
			{
				Control * control = interface->mainForm->getControl("NavigateButton");
				RadioButton * button = (RadioButton*)control;
				button->setSelected(true);
				control = interface->mainForm->getControl("EditorButton");
				button = (RadioButton*)control;
				button->setSelected(false);

				_inputMode = NAVIGATION;
				interface->inputMode = (UI::INPUT_MODE)NAVIGATION;
				interface->mainForm->getControl("TerrainEditorbar")->setVisible(false);
				interface->mainForm->getControl("GeneratorToolBar")->setVisible(false);
				interface->mainForm->getControl("NavigateToolBar")->setVisible(true);
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
			pickRay.setOrigin(_camera->getPosition());

			for (size_t i = 0; i < _pager->loadedTerrains.size(); i++)
			{
				TerrainHitFilter hitFilter(_pager->loadedTerrains[i]);
				PhysicsController::HitResult hitResult;
				if (Game::getInstance()->getPhysicsController()->rayTest(pickRay, 1000000, &hitResult, &hitFilter))
				{
					if (hitResult.object == _pager->loadedTerrains[i]->getNode()->getCollisionObject())
					{
						_selectionRing->setPosition(hitResult.point.x, hitResult.point.z, _pager->loadedTerrains[i]);


						//changing textures path in the UI according to the nearest terrain from the selection ring
						int indexX = _pager->loadedTerrains[i]->getNode()->getTranslationWorld().x / _pager->parameters.boundingBox;
						int indexZ = _pager->loadedTerrains[i]->getNode()->getTranslationWorld().z / _pager->parameters.boundingBox;

						Control * control = interface->mainForm->getControl("Texture0Pos");
						TextBox * textBox = (TextBox *)control;
						textBox->setText(_pager->zoneList[indexX][indexZ]->texturesLocation[0].c_str());

						control = interface->mainForm->getControl("Texture1Pos");
						textBox = (TextBox *)control;
						textBox->setText(_pager->zoneList[indexX][indexZ]->texturesLocation[1].c_str());

						control = interface->mainForm->getControl("Texture2Pos");
						textBox = (TextBox *)control;
						textBox->setText(_pager->zoneList[indexX][indexZ]->texturesLocation[2].c_str());
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
			_camera->rotate(yaw, pitch);
		}
		_prevX = x;
		_prevY = y;
		return true;
	}
	return false;
}