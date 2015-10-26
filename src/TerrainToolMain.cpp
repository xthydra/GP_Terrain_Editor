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

// Declare our game instance
TerrainToolMain game;

TerrainToolMain::TerrainToolMain()
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

void TerrainToolMain::initialize()
{
	// Create an empty scene.
	_scene = Scene::create();
	_scene->setAmbientColor(0.5,0.5,0.5);

	// Setup a fly cam.
	_camera.initialize(1.0f, 400000.0f, 45);
	_camera.rotate(0.0f, -MATH_DEG_TO_RAD(10));
	_scene->addNode(_camera.getRootNode());
	_scene->setActiveCamera(_camera.getCamera());

	// Create a light source.

//#define POINTLIGHT_TEST
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
#endif*/
	//creating and setting up the UI
	_mainForm = Form::create("res/main.form");

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

	slider = (Slider *)_mainForm->getControl("RenderSlider");
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

	control = _mainForm->getControl("GeneratenSaveButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GenerateBlendMapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GenerateHeightMapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GenerateNormalMapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GenerateRawHeightfieldsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GenerateObjectsPosButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("GenerateNoiseButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _mainForm->getControl("LoadButton");
	control->addListener(this, Control::Listener::CLICK);
	//=============

	//=============TERRAIN_EDITOR
	control = _mainForm->getControl("AlignButton");
	control->addListener(this, Control::Listener::CLICK);
	Button * button = (Button *)control;
	Vector4 normalColor = button->getSkinColor(Control::State::NORMAL);
	Vector4 activeColor = button->getSkinColor(Control::State::ACTIVE);

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
	_generateTerrainsForm = Form::create("res/generateTerrains.form");
	_generateTerrainsForm->setVisible(false);

	control = _generateTerrainsForm->getControl("CancelGenerateTerrainsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _generateTerrainsForm->getControl("ConfirmGenerateTerrainsButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============	
	_generateBlendmapsForm = Form::create("res/generateBlendmaps.form");
	_generateBlendmapsForm->setVisible(false);

	control = _generateBlendmapsForm->getControl("CancelGenerateBlendmapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _generateBlendmapsForm->getControl("ConfirmGenerateBlendmapsButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============
	_loadForm = Form::create("res/load.form");
	_loadForm->setVisible(false);

	control = _loadForm->getControl("CancelLoadButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _loadForm->getControl("ConfirmLoadButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============
	_noiseForm = Form::create("res/generateNoise.form");
	_noiseForm->setVisible(false);

	control = _noiseForm->getControl("CancelNoiseButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _noiseForm->getControl("ConfirmNoiseButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============GENERATE_N_SAVE
	_saveForm = Form::create("res/save.form");
	_saveForm->setVisible(false);

	control = _saveForm->getControl("PNGHeightmapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _saveForm->getControl("RAWHeightmapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _saveForm->getControl("BlendmapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _saveForm->getControl("NormalmapsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _saveForm->getControl("ObjectsButton");
	control->addListener(this, Control::Listener::CLICK);

	control = _saveForm->getControl("CancelSaveButton");
	control->addListener(this, Control::Listener::CLICK);
	//===============
	
//#define EXAMPLE
#ifdef EXAMPLE
	//Generate heightfields for the terrain pager
	STerrainParameters param;
	param.generatedBlendmaps = false;
	param.generatedNormalmaps = false;
	param.generatedHeightmaps = false;
	param.generatedObjects = false;
	param.tilesResolution = 2;			//TERRAIN_PARAMETER_1
	param.heightFieldResolution = 128;  //TERRAIN_PARAMETER_2
	//if you put LOD on 1 it will make terrain creation faster
	param.LodQuality = 1;				//TERRAIN_PARAMETER_3
	param.TextureScale = 40;			//TERRAIN_PARAMETER_4
	param.minHeight = 0;				//TERRAIN_PARAMETER_5
	param.maxHeight = 128;				//TERRAIN_PARAMETER_6
	param.skirtSize = 16;				//TERRAIN_PARAMETER_7
	param.patchSize = 32;				//TERRAIN_PARAMETER_8
	param.Scale = Vector3(param.heightFieldResolution, ((param.heightFieldResolution*param.tilesResolution)*0.10), param.heightFieldResolution);

	//int maxHeight = ((TerrPager->Param.tilesResolution * TerrPager->Param.tilesResolution) * TerrPager->Param.tilesResolution) *
	//(TerrPager->Param.heightFieldResolution * TerrPager->Param.tilesResolution);

	param.BoundingBox = (param.heightFieldResolution * param.heightFieldResolution) - param.heightFieldResolution;

	TerrainGenerator terrainGenerator;
	std::vector<std::vector<gameplay::HeightField*>> heightfields;
	heightfields = terrainGenerator.buildTerrainTiles(param.heightFieldResolution,
		param.tilesResolution,
		param.minHeight,
		param.maxHeight,
		0,
		1,
		1,
		true,
		true);

	param.heightFieldList = heightfields;
	//param.Scale = Vector3(param.heightFieldResolution, (param.heightFieldResolution /param.tilesResolution), param.heightFieldResolution);

	//if you want to apply a second scale apply it like that
	//E.G : param.Scale = param.Scale * 1.5; param.BoundingBox=param.BoundingBox* 1.5;
	//but dont expect the terrain editor tools to work because alot of functions are using heightfield resolution instead
	//of the scale parameter sent to the paging class

	param.Debug = false;

	param.distanceLoad = param.BoundingBox * .9;
	param.distanceUnload = param.BoundingBox * 1;
	param.distanceMaxRender = param.BoundingBox * 1;

	TerrPager = new TerrainPager(param, _scene);

#define CREATE_BLENDMAPS
#ifdef CREATE_BLENDMAPS
	TerrPager->Param.blendMaps = 
		terrainGenerator.createTiledTransparentBlendImages(TerrPager->Param.Scale.y,
														   0,
														   50,
														   0,
														   100,
														   TerrPager->Param.heightFieldResolution,
														   TerrPager->Param.heightFieldList);
	TerrPager->Param.BlendMapDIR = createTMPFolder();

	FilesSaver saver;
	Threads t;
	t.blendBool();
	saverThreads.push_back(t);

	threads.push_back(std::async(std::launch::async,
		&FilesSaver::saveBlendmaps,
		saver,
		TerrPager->Param.blendMaps,
		TerrPager->Param.BlendMapDIR,
		TerrPager->Param.heightFieldResolution));
#endif

#define CREATE_NORMALMAPS
#ifdef CREATE_NORMALMAPS
	TerrPager->Param.normalMaps = 
		terrainGenerator.createNormalmaps(param.Scale.y, 
										  TerrPager->Param.heightFieldResolution, 
										  TerrPager->Param.heightFieldList);

	TerrPager->Param.NormalMapDIR = createTMPFolder();

	Threads t2;
	t2.normalBool();
	saverThreads.push_back(t2);

	threads.push_back(std::async(std::launch::async,
		&FilesSaver::saveNormalmaps,
		saver,
		TerrPager->Param.normalMaps,
		TerrPager->Param.NormalMapDIR,
		TerrPager->Param.heightFieldResolution));
#endif
	_camera.setPosition(Vector3(0, ((param.heightFieldResolution*param.tilesResolution) * 25), 0));
	_camera.rotate(0, -90);

	_selectionRing = new SelectionRing(_scene);

#define CREATE_TREE
#ifdef CREATE_TREE
	Scene * Stree = Scene::load("res/tree.gpb");
	Node* leaves = Stree->findNode("leaves");
	Node* tree = Stree->findNode("trunk");
	tree->setScale(Vector3(50, 50, 50));
	leaves->setScale(Vector3(50, 50, 50));
	Model* tempt = dynamic_cast<Model*>(tree->getDrawable());
	tempt->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "POINT_LIGHT_COUNT 1");
#define TRANSPARENCY
#ifdef TRANSPARENCY
	Model* tempt2 = dynamic_cast<Model*>(leaves->getDrawable());
	tempt2->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "POINT_LIGHT_COUNT 1; TEXTURE_DISCARD_ALPHA 1");
#else
	Material* material2 = leaves->getModel()->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "DIRECTIONAL_LIGHT_COUNT 1");
#endif

#ifdef POINTLIGHT_TEST
	//tempt->getMaterial()->setParameterAutoBinding("u_normalMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
	tempt->getMaterial()->setParameterAutoBinding("u_worldViewMatrix", "WORLD_VIEW_MATRIX");
	tempt->getMaterial()->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
	tempt->getMaterial()->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
	//tempt2->getMaterial()->setParameterAutoBinding("u_normalMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
	tempt2->getMaterial()->setParameterAutoBinding("u_worldViewMatrix", "WORLD_VIEW_MATRIX");
	tempt2->getMaterial()->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
	tempt2->getMaterial()->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
#else
	tempt->getMaterial()->setParameterAutoBinding("u_worldViewMatrix", "WORLD_VIEW_MATRIX");
	tempt->getMaterial()->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
	tempt->getMaterial()->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
	tempt2->getMaterial()->setParameterAutoBinding("u_worldViewMatrix", "WORLD_VIEW_MATRIX");
	tempt2->getMaterial()->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
	tempt2->getMaterial()->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
#endif
	// Set the ambient color of the material.
	tempt->getMaterial()->getParameter("u_ambientColor")->setValue(Vector3(0.5f, 0.5f, 0.5f));
	tempt2->getMaterial()->getParameter("u_ambientColor")->setValue(Vector3(0.5f, 0.5f, 0.5f));

#ifdef POINTLIGHT_TEST
	tempt->getMaterial()->getParameter("u_pointLightColor[0]")->setValue(lightNode->getLight()->getColor());
	tempt->getMaterial()->getParameter("u_pointLightRangeInverse[0]")->setValue(lightNode->getLight()->getRangeInverse());
	tempt->getMaterial()->getParameter("u_pointLightPosition[0]")->bindValue(lightNode, &Node::getTranslationView);
	tempt2->getMaterial()->getParameter("u_pointLightColor[0]")->setValue(lightNode->getLight()->getColor());
	tempt2->getMaterial()->getParameter("u_pointLightRangeInverse[0]")->setValue(lightNode->getLight()->getRangeInverse());
	tempt2->getMaterial()->getParameter("u_pointLightPosition[0]")->bindValue(lightNode, &Node::getTranslationView);
#else
	// Bind the light's color and direction to the material.
	tempt->getMaterial()->getParameter("u_directionalLightColor[0]")->setValue(lightNode->getLight()->getColor());
	tempt->getMaterial()->getParameter("u_directionalLightDirection[0]")->bindValue(lightNode, &Node::getForwardVectorWorld);
	tempt2->getMaterial()->getParameter("u_directionalLightColor[0]")->setValue(lightNode->getLight()->getColor());
	tempt2->getMaterial()->getParameter("u_directionalLightDirection[0]")->bindValue(lightNode, &Node::getForwardVectorWorld);
#endif
	// Load the texture from file.
	Texture::Sampler* sampler = tempt->getMaterial()->getParameter("u_diffuseTexture")->setValue("res/bark.png", true);
	Texture::Sampler* sampler2 = tempt2->getMaterial()->getParameter("u_diffuseTexture")->setValue("res/leave.png", true);

	tempt->getMaterial()->getStateBlock()->setCullFace(true);
	tempt->getMaterial()->getStateBlock()->setDepthTest(true);

	tempt2->getMaterial()->getStateBlock()->setCullFace(true);
	tempt2->getMaterial()->getStateBlock()->setDepthTest(true);

	TerrPager->PagingCheck();

	Vector3 worldScale;//(1,1,1);
	TerrPager->PagingCheck();
	TerrPager->Param.loadedTerrains[0]->getNode()->getWorldMatrix().getScale(&worldScale);

	std::vector<std::vector<std::vector<Vector3*> > > objsPos = terrainGenerator.generateObjectsPosition(worldScale, param.Scale.y, 1, param.heightFieldResolution, param.heightFieldList, tree);
	std::vector<std::vector<std::vector<Node*> > > objs;
	std::vector<Model*> models;

	objs.resize(param.heightFieldList.size());	
	for (size_t i = 0; i < param.heightFieldList.size(); i++)
	{
		objs[i].resize(param.heightFieldList.size());
		for (size_t j = 0; j < param.heightFieldList[i].size(); j++)
		{
			for (size_t g = 0; g < objsPos[i][j].size(); g++)
			{
				Node * tmp1 = tree->clone();
				Node * tmp2 = leaves->clone();

				Node * tree2 = Node::create("tree");

				tree2->addChild(tmp2);
				tree2->addChild(tmp1);

				tree2->setTranslation(Vector3(
					objsPos[i][j][g]->x,
					objsPos[i][j][g]->y+30,
					objsPos[i][j][g]->z));

				_scene->addNode(tree2);

				objs[i][j].push_back(tree2);
			}
		}
	}
	TerrPager->Param.objects = objs;
	TerrPager->Param.objectsPosition = objsPos;
	TerrPager->Param.objectsFilename.push_back("tree.gpb");

	threads.push_back(std::async(std::launch::async,
		&FilesSaver::saveObjectsPos,
		saver,
		TerrPager->Param.objectsPosition,
		TerrPager->Param.objectsFilename[0]));

	Threads t3;
	t3.objectBool();
	saverThreads.push_back(t3);
#endif
	tree->getDrawable()->draw();
	leaves->getDrawable()->draw();
	TerrPager->Param.distanceMaxModelRender = tree->getBoundingSphere().radius * 5;
#endif
}

void TerrainToolMain::finalize()
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

void TerrainToolMain::controlEvent(Control* control, Control::Listener::EventType evt)
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
		_inputMode = TERRAIN;
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
	else if (strcmp(control->getId(), "GeneratenSaveButton") == 0)
	{		
		_mainForm->setVisible(false);
		_saveForm->setVisible(true);
	}
	else if (strcmp(control->getId(), "GenerateHeightMapsButton") == 0)
	{
		TerrainGenerator terrainGenerator;
		terrainGenerator.createHeightmaps(TerrPager->Param.Scale.y, TerrPager->Param.heightFieldResolution, TerrPager->Param.heightFieldList);
	}
	else if (strcmp(control->getId(), "GenerateBlendMapsButton") == 0)
	{
		_mainForm->setVisible(false);
		_generateBlendmapsForm->setVisible(true);
	}
	else if (strcmp(control->getId(), "GenerateNormalMapsButton") == 0)
	{
		TerrainGenerator terrainGenerator;
		terrainGenerator.createNormalmaps(TerrPager->Param.Scale.y, TerrPager->Param.heightFieldResolution, TerrPager->Param.heightFieldList);
	}
	else if (strcmp(control->getId(), "GenerateRawHeightfieldsButton") == 0)
	{
		TerrainGenerator terrainGenerator;
		terrainGenerator.createRawHeightfields(TerrPager->Param.Scale.y, TerrPager->Param.maxHeight, TerrPager->Param.BoundingBox, TerrPager->Param.heightFieldResolution, TerrPager->Param.heightFieldList);
	}
	else if (strcmp(control->getId(), "GenerateObjectsPosButton") == 0)
	{
		generateObjectsPosition();
		//FilesSaver saver;
		//std::thread(&FilesSaver::saveObjectsPos,saver, TerrPager->Param.objectsPosition, TerrPager->Param.objectsFilename[0]);
	}
	else if (strcmp(control->getId(), "GenerateNoiseButton") == 0)
	{
		_mainForm->setVisible(false);
		_noiseForm->setVisible(true);
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
	else if (strcmp(control->getId(), "LoadSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		if (TerrPager->Param.distanceUnload > (slider->getValue() * TerrPager->Param.BoundingBox))
		{
			TerrPager->Param.distanceLoad = (slider->getValue() * TerrPager->Param.BoundingBox);
		}
		else if (TerrPager->Param.distanceUnload < (slider->getValue() * TerrPager->Param.BoundingBox))
		{
			Control * control2 = _mainForm->getControl("UnloadSlider");
			Slider * slider2 = (Slider *)control2;
			slider2->setValue(slider->getValue() + 1);
			//setloadSlider value below unloadSlider
			TerrPager->Param.distanceUnload = (slider2->getValue() * TerrPager->Param.BoundingBox);
			TerrPager->Param.distanceLoad = (slider->getValue() * TerrPager->Param.BoundingBox);
		}
	}
	else if (strcmp(control->getId(), "RenderSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		TerrPager->Param.distanceMaxRender = (slider->getValue() * TerrPager->Param.BoundingBox);
		TerrPager->Param.distanceMaxModelRender = ((slider->getValue() * TerrPager->Param.BoundingBox) * 0.3);
	}
	else if (strcmp(control->getId(), "UnloadSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		if (TerrPager->Param.distanceLoad < (slider->getValue() * TerrPager->Param.BoundingBox))
		{
			TerrPager->Param.distanceUnload = (slider->getValue() * TerrPager->Param.BoundingBox);
		}
		else if (TerrPager->Param.distanceLoad >(slider->getValue() * TerrPager->Param.BoundingBox))
		{
			Control * control2 = _mainForm->getControl("LoadSlider");
			Slider * slider2 = (Slider *)control2;
			slider2->setValue(slider->getValue() - 1);
			//setloadSlider value below unloadSlider
			TerrPager->Param.distanceLoad = (slider2->getValue() * TerrPager->Param.BoundingBox);
			TerrPager->Param.distanceUnload = (slider->getValue() * TerrPager->Param.BoundingBox);
		}
	}
	//=============================

	//=======================EDITOR
	else if (strcmp(control->getId(), "SizeSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		_selectionScale = slider->getValue() * ((TerrPager->Param.heightFieldResolution / 8) * (TerrPager->Param.Scale.x / 8));
		int nearest = TerrPager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
		if (nearest != -1)
		{
			_selectionRing->setScale(_selectionScale, TerrPager->Param.loadedTerrains[nearest]);
		}
	}
	else if (strcmp(control->getId(), "AlignButton") == 0)
	{
		TerrainEditor TerrEdit;
		TerrEdit.aligningTerrainsVertexes(TerrPager->Param.heightFieldList,
			TerrPager->Param.heightFieldResolution);
		TerrPager->reloadTerrains();
	}
	else if (strcmp(control->getId(), "RaiseButton") == 0)
	{
		TerrainEditor TerrEdit;
		int nearest = TerrPager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
		if (nearest != -1)
		{
			int posX = _selectionRing->getPositionX();
			int posZ = _selectionRing->getPositionZ();

			//reposition the ray hit as if the terrain underneath the ray was at position 0
			int Xarray = TerrPager->Param.loadedTerrains[nearest]->getNode()->getTranslationWorld().x
				/ (TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.x * 2);
			posX -= (Xarray)*(TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.x * 2);

			int Zarray = TerrPager->Param.loadedTerrains[nearest]->getNode()->getTranslationWorld().z
				/ (TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.z * 2);
			posZ -= (Zarray)*(TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.z * 2);

			std::vector<int> heightfields =
				TerrEdit.raiseTiles(posX,
				posZ,
				_selectionRing->getScale(),
				TerrPager->Param.loadedTerrains,
				nearest,
				TerrPager->Param.heightFieldResolution,
				TerrPager->Param.loadedHeightfields);

			TerrPager->reload(heightfields);
		}
	}
	else if (strcmp(control->getId(), "LowerButton") == 0)
	{
		//control = _mainForm->getControl("LowerButton");
		//Button * button = (Button *)control;
		//button->setSkinColor(button->getSkinColor(Control::State::ACTIVE), gameplay::Control::State::NORMAL);

		TerrainEditor TerrEdit;
		int nearest = TerrPager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
		if (nearest != -1)
		{
			int posX = _selectionRing->getPositionX();
			int posZ = _selectionRing->getPositionZ();

			//reposition the ray hit as if the terrain underneath the ray was at position 0
			int Xarray = TerrPager->Param.loadedTerrains[nearest]->getNode()->getTranslationWorld().x
				/ (TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.x * 2);
			posX -= (Xarray)*(TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.x * 2);

			int Zarray = TerrPager->Param.loadedTerrains[nearest]->getNode()->getTranslationWorld().z
				/ (TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.z * 2);
			posZ -= (Zarray)*(TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.z * 2);

			std::vector<int> heightfields =
				TerrEdit.lowerTiles(posX,
				posZ,
				_selectionRing->getScale(),
				TerrPager->Param.loadedTerrains,
				nearest,
				TerrPager->Param.heightFieldResolution,
				TerrPager->Param.loadedHeightfields);

			TerrPager->reload(heightfields);
		}
	}
	else if (strcmp(control->getId(), "FlattenButton") == 0)
	{
		TerrainEditor TerrEdit;
		int nearest = TerrPager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
		if (nearest != -1)
		{
			int posX = _selectionRing->getPositionX();
			int posZ = _selectionRing->getPositionZ();

			//reposition the ray hit as if the terrain underneath the ray was at position 0
			int Xarray = TerrPager->Param.loadedTerrains[nearest]->getNode()->getTranslationWorld().x
				/ (TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.x * 2);
			posX -= (Xarray)*(TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.x * 2);

			int Zarray = TerrPager->Param.loadedTerrains[nearest]->getNode()->getTranslationWorld().z
				/ (TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.z * 2);
			posZ -= (Zarray)*(TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.z * 2);

			std::vector<int> heightfields =
				TerrEdit.flattenTiles(posX,
				posZ,
				_selectionRing->getScale(),
				TerrPager->Param.loadedTerrains,
				nearest,
				TerrPager->Param.heightFieldResolution,
				TerrPager->Param.loadedHeightfields);

			TerrPager->reload(heightfields);
		}
	}
	else if (strcmp(control->getId(), "SmoothButton") == 0)
	{
		TerrainEditor TerrEdit;
		int nearest = TerrPager->findTerrain(Vector2(_prevX, _prevY), Vector2(getWidth(), getHeight()));
		if (nearest != -1)
		{
			int posX = _selectionRing->getPositionX();
			int posZ = _selectionRing->getPositionZ();

			//reposition the ray hit as if the terrain underneath the ray was at position 0
			int Xarray = TerrPager->Param.loadedTerrains[nearest]->getNode()->getTranslationWorld().x
				/ (TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.x * 2);
			posX -= (Xarray)*(TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.x * 2);

			int Zarray = TerrPager->Param.loadedTerrains[nearest]->getNode()->getTranslationWorld().z
				/ (TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.z * 2);
			posZ -= (Zarray)*(TerrPager->Param.loadedTerrains[nearest]->getBoundingBox().max.z * 2);

			std::vector<int> heightfields =
				TerrEdit.smoothTiles(posX,
				posZ,
				_selectionRing->getScale(),
				TerrPager->Param.loadedTerrains,
				nearest,
				TerrPager->Param.heightFieldResolution,
				TerrPager->Param.loadedHeightfields);

			TerrPager->reload(heightfields);
		}
	}
	//=============================
	else if (strcmp(control->getId(), "CancelGenerateTerrainsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateTerrainsForm->setVisible(false);
		_mainForm->update(1);
		_mainForm->getControl("GeneratorToolBar")->setPosition(0, 50);
	}
	else if (strcmp(control->getId(), "ConfirmGenerateTerrainsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateTerrainsForm->setVisible(false);
		this->generateNewTerrain();
		_mainForm->update(1);
		_mainForm->getControl("GeneratorToolBar")->setPosition(0, 50);
	}
	//===========================
	else if (strcmp(control->getId(), "CancelGenerateBlendmapsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateBlendmapsForm->setVisible(false);
		_mainForm->update(1);
		_mainForm->getControl("GeneratorToolBar")->setPosition(0, 50);
	}
	else if (strcmp(control->getId(), "ConfirmGenerateBlendmapsButton") == 0)
	{
		_mainForm->setVisible(true);
		_generateBlendmapsForm->setVisible(false);
		generateNewBlendmaps();
		_mainForm->update(1);
		_mainForm->getControl("GeneratorToolBar")->setPosition(0, 50);
	}
	//===========================
	else if (strcmp(control->getId(), "CancelLoadButton") == 0)
	{
		_mainForm->setVisible(true);
		_loadForm->setVisible(false);
		_mainForm->update(1);
		_mainForm->getControl("GeneratorToolBar")->setPosition(0, 50);
	}
	else if (strcmp(control->getId(), "ConfirmLoadButton") == 0)	
	{
		_mainForm->setVisible(true);
		_loadForm->setVisible(false);
		load();
		_mainForm->update(1);
		_mainForm->getControl("GeneratorToolBar")->setPosition(0, 50);
	}
	//===========================
	else if (strcmp(control->getId(), "CancelSaveButton") == 0)
	{
		_mainForm->setVisible(true);
		_saveForm->setVisible(false);
		_mainForm->update(1);
		_mainForm->getControl("GeneratorToolBar")->setPosition(0, 50);
	}
	//===========================
	else if (strcmp(control->getId(), "CancelNoiseButton") == 0)
	{
		_mainForm->setVisible(true);
		_noiseForm->setVisible(false);
		_mainForm->update(1);
		_mainForm->getControl("GeneratorToolBar")->setPosition(0, 50);
	}
	else if (strcmp(control->getId(), "ConfirmNoiseButton") == 0)
	{
		_mainForm->setVisible(true);
		_noiseForm->setVisible(false);
		_mainForm->update(1);
		_mainForm->getControl("GeneratorToolBar")->setPosition(0, 50);
	}
}

void TerrainToolMain::load()
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
		TerrPager->removeObjects();
		FilesLoader load;
		TerrPager->Param.heightMapRAWList = load.loadRAWHeightmaps(folder);
		TerrPager->Param.HeightMapDIR = (char*)folder;
		TerrPager->Param.tilesResolution = load.tilesResolution;
		TerrPager->loadHeightfields();
		TerrPager->removeTerrains();
	}

	control = _loadForm->getControl("BlendmapRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		FilesLoader load;
		TerrPager->Param.blendMapList = load.loadBlendmaps(folder);
		TerrPager->Param.BlendMapDIR = (char*)folder;
		TerrPager->Param.tilesResolution = load.tilesResolution;
		TerrPager->reloadTerrains();
	}

	control = _loadForm->getControl("HeightmapRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		TerrPager->removeObjects();
		FilesLoader load;
		TerrPager->Param.heightMapPNGList = load.loadHeightmaps(folder);
		TerrPager->Param.HeightMapDIR = (char*)folder;
		TerrPager->Param.tilesResolution = load.tilesResolution;
		TerrPager->loadHeightfields();
		TerrPager->removeTerrains();
	}

	control = _loadForm->getControl("ObjectsPosRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{

	}

	control = _loadForm->getControl("NormalmapRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected())
	{
		FilesLoader load;
		TerrPager->Param.normalMapList = load.loadNormalmaps(folder);
		TerrPager->Param.NormalMapDIR = (char*)folder;
	}

}

void TerrainToolMain::generateNewBlendmaps()
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

	TerrPager->Param.blendMaps = terrainGenerator.createTiledTransparentBlendImages(TerrPager->Param.Scale.y,
		Intensity1,
		Intensity2,
		Opacity1,
		Opacity2,
		TerrPager->Param.heightFieldResolution,
		TerrPager->Param.heightFieldList);
	TerrPager->Param.BlendMapDIR = createTMPFolder();

	for (size_t i = 0; i < threads.size(); i++)
	{
		if (saverThreads[i].blendmap == true)
		{
			threads[i].~future();
			threads.erase(threads.begin() + i);
			saverThreads.erase(saverThreads.begin() + i);
		}
	}

	FilesSaver saver;
	Threads t;
	t.blendBool();
	saverThreads.push_back(t);
	TerrPager->Param.generatedBlendmaps = false;

	threads.push_back(std::async(std::launch::async,
		&FilesSaver::saveBlendmaps,
		saver,
		TerrPager->Param.blendMaps,
		TerrPager->Param.BlendMapDIR,
		TerrPager->Param.heightFieldResolution));

	TerrPager->reloadTerrains();
}

void TerrainToolMain::generateObjectsPosition()
{
	if (TerrPager)
	{
		if (TerrPager->Param.heightFieldList.size() > 0)
		{
			TerrPager->removeObjects();

			Scene * Stree = Scene::load("res/tree.gpb");
			Node* leaves = Stree->findNode("leaves");
			Node* tree = Stree->findNode("trunk");
			tree->setScale(Vector3(50, 50, 50));
			leaves->setScale(Vector3(50, 50, 50));
			Model* tempt = dynamic_cast<Model*>(tree->getDrawable());
			tempt->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "POINT_LIGHT_COUNT 1");
#define TRANSPARENCY
#ifdef TRANSPARENCY
			Model* tempt2 = dynamic_cast<Model*>(leaves->getDrawable());
			tempt2->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "POINT_LIGHT_COUNT 1; TEXTURE_DISCARD_ALPHA 1");
#else
			Material* material2 = leaves->getModel()->setMaterial("res/shaders/textured.vert", "res/shaders/textured.frag", "DIRECTIONAL_LIGHT_COUNT 1");
#endif

#ifdef POINTLIGHT_TEST
			//tempt->getMaterial()->setParameterAutoBinding("u_normalMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
			tempt->getMaterial()->setParameterAutoBinding("u_worldViewMatrix", "WORLD_VIEW_MATRIX");
			tempt->getMaterial()->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
			tempt->getMaterial()->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
			//tempt2->getMaterial()->setParameterAutoBinding("u_normalMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
			tempt2->getMaterial()->setParameterAutoBinding("u_worldViewMatrix", "WORLD_VIEW_MATRIX");
			tempt2->getMaterial()->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
			tempt2->getMaterial()->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
#else
			tempt->getMaterial()->setParameterAutoBinding("u_worldViewMatrix", "WORLD_VIEW_MATRIX");
			tempt->getMaterial()->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
			tempt->getMaterial()->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
			tempt2->getMaterial()->setParameterAutoBinding("u_worldViewMatrix", "WORLD_VIEW_MATRIX");
			tempt2->getMaterial()->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
			tempt2->getMaterial()->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
#endif
			// Set the ambient color of the material.
			tempt->getMaterial()->getParameter("u_ambientColor")->setValue(Vector3(0.5f, 0.5f, 0.5f));
			tempt2->getMaterial()->getParameter("u_ambientColor")->setValue(Vector3(0.5f, 0.5f, 0.5f));

			Node* lightNode = _scene->findNode("light");
#ifdef POINTLIGHT_TEST
			tempt->getMaterial()->getParameter("u_pointLightColor[0]")->setValue(lightNode->getLight()->getColor());
			tempt->getMaterial()->getParameter("u_pointLightRangeInverse[0]")->setValue(lightNode->getLight()->getRangeInverse());
			tempt->getMaterial()->getParameter("u_pointLightPosition[0]")->bindValue(lightNode, &Node::getTranslationView);
			tempt2->getMaterial()->getParameter("u_pointLightColor[0]")->setValue(lightNode->getLight()->getColor());
			tempt2->getMaterial()->getParameter("u_pointLightRangeInverse[0]")->setValue(lightNode->getLight()->getRangeInverse());
			tempt2->getMaterial()->getParameter("u_pointLightPosition[0]")->bindValue(lightNode, &Node::getTranslationView);
#else
			// Bind the light's color and direction to the material.
			tempt->getMaterial()->getParameter("u_directionalLightColor[0]")->setValue(lightNode->getLight()->getColor());
			tempt->getMaterial()->getParameter("u_directionalLightDirection[0]")->bindValue(lightNode, &Node::getForwardVectorWorld);
			tempt2->getMaterial()->getParameter("u_directionalLightColor[0]")->setValue(lightNode->getLight()->getColor());
			tempt2->getMaterial()->getParameter("u_directionalLightDirection[0]")->bindValue(lightNode, &Node::getForwardVectorWorld);
#endif
			// Load the texture from file.
			Texture::Sampler* sampler = tempt->getMaterial()->getParameter("u_diffuseTexture")->setValue("res/bark.png", true);
			Texture::Sampler* sampler2 = tempt2->getMaterial()->getParameter("u_diffuseTexture")->setValue("res/leave.png", true);

			tempt->getMaterial()->getStateBlock()->setCullFace(true);
			tempt->getMaterial()->getStateBlock()->setDepthTest(true);

			tempt2->getMaterial()->getStateBlock()->setCullFace(true);
			tempt2->getMaterial()->getStateBlock()->setDepthTest(true);

			TerrPager->PagingCheck();

			Vector3 worldScale;//(1,1,1);
			TerrPager->PagingCheck();
			TerrPager->Param.loadedTerrains[0]->getNode()->getWorldMatrix().getScale(&worldScale);

			TerrainGenerator terrainGenerator;
			std::vector<std::vector<std::vector<Vector3*> > > objsPos = terrainGenerator.generateObjectsPosition(worldScale, TerrPager->Param.Scale.y, 1, TerrPager->Param.heightFieldResolution, TerrPager->Param.heightFieldList, tree);
			std::vector<std::vector<std::vector<Node*> > > objs;
			std::vector<Model*> models;

			objs.resize(TerrPager->Param.heightFieldList.size());
			for (size_t i = 0; i < TerrPager->Param.heightFieldList.size(); i++)
			{
				objs[i].resize(TerrPager->Param.heightFieldList.size());
				for (size_t j = 0; j < TerrPager->Param.heightFieldList[i].size(); j++)
				{
					for (size_t g = 0; g < objsPos[i][j].size(); g++)
					{
						//if it would be the first node don't clone it and just position it
						Node * tmp1 = tree->clone();
						Node * tmp2 = leaves->clone();

						Node * tree2 = Node::create("tree");

						tree2->addChild(tmp2);
						tree2->addChild(tmp1);

						tree2->setTranslation(Vector3(
							objsPos[i][j][g]->x,
							objsPos[i][j][g]->y,
							objsPos[i][j][g]->z));

						_scene->addNode(tree2);

						objs[i][j].push_back(tree2);
					}
				}
			}
			TerrPager->Param.objects = objs;
			TerrPager->Param.objectsPosition = objsPos;
			TerrPager->Param.objectsFilename.push_back("tree.gpb");

			/*threads.push_back(std::async(std::launch::async,
				&FilesSaver::saveObjectsPos,
				saver,
				TerrPager->Param.objectsPosition,
				TerrPager->Param.objectsFilename[0]));

			Threads t3;
			t3.objectBool();
			saverThreads.push_back(t3);*/

			tree->getDrawable()->draw();
			leaves->getDrawable()->draw();
			TerrPager->Param.distanceMaxModelRender = tree->getBoundingSphere().radius * 5;
		}
	}
}

void TerrainToolMain::generateNewTerrain()
{
	if (TerrPager)
	{
		TerrPager->removeObjects();

		TerrPager->removeTerrains();
		for (size_t i = 0; i < TerrPager->zoneList.size(); i++)
		{
			for (size_t j = 0; j < TerrPager->zoneList[i].size(); j++)
			{
				if (TerrPager->zoneList[i][j]->isLoaded() == true)
				{
					TerrPager->removeTerrain(i, j);
				}
			}
		}

		TerrPager->Param.loadedTerrains.clear();
		TerrPager->Param.loadedHeightfields.clear();
	}

	if (!TerrPager)
	{
		STerrainParameters param;
		param.TextureScale = 40;
		param.generatedBlendmaps = false;
		param.generatedNormalmaps = false;
		param.generatedHeightmaps = false;
		param.generatedObjects = false;
		param.skirtSize = 16;
		param.Debug = false;
		TerrPager = new TerrainPager(param, _scene);
	}


	Control * control;
	Slider * slider;
	TextBox * textBox;
	RadioButton * radioButton;
	float xz = 0, y = 0;

	control = _generateTerrainsForm->getControl("DetailLevelsSlider");
	slider = (Slider *)control;
	TerrPager->Param.LodQuality = slider->getValue();

	if (TerrPager->Param.LodQuality == 0){ TerrPager->Param.LodQuality = 1; }

	control = _generateTerrainsForm->getControl("HeightFieldSizeSlider");
	slider = (Slider *)control;
	int heightfieldResolution = slider->getValue();
	TerrPager->Param.heightFieldResolution = heightfieldResolution;

	control = _generateTerrainsForm->getControl("MaxHeightSlider");
	slider = (Slider *)control;
	TerrPager->Param.maxHeight = (slider->getValue());

	control = _generateTerrainsForm->getControl("MinHeightSlider");
	slider = (Slider *)control;
	TerrPager->Param.minHeight = (slider->getValue());

	control = _generateTerrainsForm->getControl("PatchSizeSlider");
	slider = (Slider *)control;
	TerrPager->Param.patchSize = (slider->getValue());

	control = _generateTerrainsForm->getControl("ScaleYSlider");
	slider = (Slider *)control;
	float scaley = slider->getValue();

	/*control = _generateForm->getControl("ScaleXZSlider");
	slider = (Slider *) control;
	xz = slider->getValue();*/

	//if you want to apply a second scale apply it like that
	//E.G : param.Scale = param.Scale * 1.5; param.BoundingBox=param.BoundingBox* 1.5;
	//but dont expect the terrain editor tools to work because alot of functions are using heightfield resolution instead
	//of the scale parameter sent to the paging class

	control = _generateTerrainsForm->getControl("SeedTextBox");
	textBox = (TextBox *)control;
	int seed = (strtol(textBox->getText(), NULL, 10));

	control = _generateTerrainsForm->getControl("TilesResolutionBox");
	textBox = (TextBox *)control;
	TerrPager->Param.tilesResolution = (strtol(textBox->getText(), NULL, 10));

	control = _generateTerrainsForm->getControl("SimplexNoiseRadio");
	radioButton = (RadioButton *)control;
	int noise;
	if (radioButton->isSelected()) { noise = 0; }

	control = _generateTerrainsForm->getControl("DiamondNoiseRadio");
	radioButton = (RadioButton *)control;
	if (radioButton->isSelected()) { noise = 1; }

	//getting Noiser settings
	control = _noiseForm->getControl("AmplitudeSlider");
	slider = (Slider *)control;
	int amplitude = (slider->getValue());

	control = _noiseForm->getControl("GainSlider");
	slider = (Slider *)control;
	int gain = (slider->getValue());

	control = _noiseForm->getControl("AmplitudeNegativeRadio");
	radioButton = (RadioButton *)control;
	bool bAmp = true;
	if (radioButton->isSelected()) { bAmp = false; }

	control = _noiseForm->getControl("GainNegativeRadio");
	radioButton = (RadioButton *)control;
	bool bGain = true;
	if (radioButton->isSelected()) { bGain = false; }

	//TerrPager->Param.maxHeight = 128;
	//TerrPager->Param.maxHeight = TerrPager->Param.heightFieldResolution / 2;

	//creating heightfields
	TerrainGenerator terrainGenerator;

	TerrPager->Param.heightFieldList = terrainGenerator.buildTerrainTiles(TerrPager->Param.heightFieldResolution,
		TerrPager->Param.tilesResolution,
		TerrPager->Param.minHeight,
		TerrPager->Param.maxHeight,
		noise,
		gain,
		amplitude,
		bGain,
		bAmp);

	TerrPager->Param.Scale = Vector3(TerrPager->Param.heightFieldResolution, scaley, TerrPager->Param.heightFieldResolution);

	TerrPager->Param.BoundingBox = (TerrPager->Param.heightFieldResolution * TerrPager->Param.heightFieldResolution) - TerrPager->Param.heightFieldResolution;

	TerrPager->Param.distanceLoad = TerrPager->Param.BoundingBox * .9;
	TerrPager->Param.distanceUnload = TerrPager->Param.BoundingBox * 1;
	TerrPager->Param.distanceMaxRender = TerrPager->Param.BoundingBox * 1;

	TerrPager->computePositions();

	TerrainEditor terrEdit;
	terrEdit.aligningTerrainsVertexes(TerrPager->Param.heightFieldList, TerrPager->Param.heightFieldResolution);

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

	TerrPager->Param.blendMaps = terrainGenerator.createTiledTransparentBlendImages(TerrPager->Param.Scale.y,
		Intensity1,
		Intensity2,
		Opacity1,
		Opacity2,
		TerrPager->Param.heightFieldResolution,
		TerrPager->Param.heightFieldList);
	TerrPager->Param.BlendMapDIR = createTMPFolder();
	TerrPager->Param.generatedBlendmaps = false;

	for (size_t i = 0; i < threads.size(); i++)
	{
		if (saverThreads[i].blendmap == true)
		{
			threads[i].~future();
			threads.erase(threads.begin() + i);
			saverThreads.erase(saverThreads.begin() + i);
		}
	}

	FilesSaver saver;
	Threads t;
	t.blendBool();
	saverThreads.push_back(t);

	threads.push_back(std::async(std::launch::async,
		&FilesSaver::saveBlendmaps,
		saver,
		TerrPager->Param.blendMaps,
		TerrPager->Param.BlendMapDIR,
		TerrPager->Param.heightFieldResolution));

#define CREATE_NORMALMAPS
#ifdef CREATE_NORMALMAPS

	TerrPager->Param.normalMaps =
		terrainGenerator.createNormalmaps(TerrPager->Param.Scale.y,
		TerrPager->Param.heightFieldResolution,
		TerrPager->Param.heightFieldList);

	TerrPager->Param.NormalMapDIR = createTMPFolder();
	TerrPager->Param.generatedNormalmaps = false;

	Threads t2;
	t2.normalBool();
	saverThreads.push_back(t2);

	threads.push_back(std::async(std::launch::async,
		&FilesSaver::saveNormalmaps,
		saver,
		TerrPager->Param.normalMaps,
		TerrPager->Param.NormalMapDIR,
		TerrPager->Param.heightFieldResolution));
#endif

	TerrPager->Param.distanceLoad = (TerrPager->Param.BoundingBox * TerrPager->Param.maxHeight ) * 0.9;
	TerrPager->Param.distanceUnload = (TerrPager->Param.BoundingBox * TerrPager->Param.maxHeight) * 1;
	TerrPager->Param.distanceMaxRender = (TerrPager->Param.BoundingBox * TerrPager->Param.maxHeight) * 1;

	TerrPager->PagingCheck();
	_camera.setPosition(Vector3(0, TerrPager->zoneList[0][0]->getObjectInside()->_terrain->getHeight(0, 0), 0));

	TerrPager->Param.distanceLoad = TerrPager->Param.BoundingBox * .9;
	TerrPager->Param.distanceUnload = TerrPager->Param.BoundingBox * 1;
	TerrPager->Param.distanceMaxRender = TerrPager->Param.BoundingBox * 1;
}

void TerrainToolMain::moveCamera(float elapsedTime)
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

void TerrainToolMain::update(float elapsedTime)
{

	int iBlendmap, iNormalmap, iObjectpos, total;
	bool blendmap = false, normalmap = false, objectpos = false;
	total = threads.size();
	for (size_t i = 0; i < threads.size(); i++)
	{
		std::future_status status = threads[i].wait_for(std::chrono::milliseconds(0));
		if (status == std::future_status::ready)
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
				iObjectpos = i;
				objectpos = true;
				continue;
			}
		}
	}
	if (blendmap == true && total == saverThreads.size())
	{
		TerrPager->Param.blendMaps.clear();
		saverThreads.erase(saverThreads.begin() + iBlendmap);
		threads.erase(threads.begin() + iBlendmap);
		TerrPager->Param.generatedBlendmaps = true;
	}
	if (normalmap == true && total == saverThreads.size())
	{
		//TerrPager->Param.normalMaps.clear();
		saverThreads.erase(saverThreads.begin() + iNormalmap);
		threads.erase(threads.begin() + iNormalmap);
		TerrPager->Param.generatedNormalmaps = true;
	}
	if (objectpos == true && total == saverThreads.size()
		&& saverThreads.size() != 0 && threads.size() != 0)
	{
		saverThreads.erase(saverThreads.begin() + iObjectpos);
		threads.erase(threads.begin() + iObjectpos);
		TerrPager->Param.generatedObjects = true;
	}

	moveCamera(elapsedTime);

	if (_mainForm)
	{
		_mainForm->update(elapsedTime);
	}
	if (_generateTerrainsForm)
	{
		_generateTerrainsForm->update(elapsedTime);
	}
	if (_generateBlendmapsForm)
	{
		_generateBlendmapsForm->update(elapsedTime);
	}
	if (_loadForm)
	{
		_loadForm->update(elapsedTime);
	}
	if (_saveForm)
	{
		_saveForm->update(elapsedTime);
	}
	if (_noiseForm)
	{
		_noiseForm->update(elapsedTime);
	}
}

void TerrainToolMain::render(float elapsedTime)
{
	// Clear the color and depth buffers
	clear(CLEAR_COLOR_DEPTH, Vector4(0.0f, 0.5f, 1.0f, 1.0f), 1.0f, 0);

	if (TerrPager)
	{
		TerrPager->PagingCheck();

		TerrPager->render();
	}

	if (_selectionRing)
	{
		Node *ring = _selectionRing->_node->getFirstChild();
		while (ring)
		{
			/*Model* select = ring->getModel();
			if (select)
			{
				select->draw();
			}*/
			if (ring->getDrawable())
			{
				ring->getDrawable()->draw();
			}
			ring = ring->getNextSibling();
		}
	}
	/*Model * tree = _scene->getFirstNode()->getNextSibling()->getNextSibling()->getNextSibling()->getModel();
	tree->draw();*/

	//wtf is that? lol
	//_scene->getFirstNode()->getNextSibling()->getNextSibling()->getNextSibling()->getDrawable()->draw();

	// Visit all the nodes in the scene for drawing
	_scene->visit(this, &TerrainToolMain::drawScene);

	if (_mainForm->isVisible())
	{
		_mainForm->draw();
	}
	if (_generateTerrainsForm)
	{
		_generateTerrainsForm->draw();
	}
	if (_generateBlendmapsForm)
	{
		_generateBlendmapsForm->draw();
	}
	if (_loadForm)
	{
		_loadForm->draw();
	}
	if (_saveForm)
	{
		_saveForm->draw();
	}
	if (_saveForm)
	{
		_noiseForm->draw();
	}
}

bool TerrainToolMain::drawScene(Node* node)
{
	// If the node visited contains a model, draw it

	Model* model = dynamic_cast<Model*>(node->getDrawable());
	if (model)
	{
		float ActualDistance = _scene->getActiveCamera()->getNode()->getTranslationWorld().distance(node->getTranslationWorld());

		//if the range of view is too much small....it crash lol
		if (ActualDistance < TerrPager->Param.distanceMaxModelRender)
		{
			if (TerrPager->Param.generatedObjects = true)
			{
				//model->draw();
				node->getDrawable()->draw();
			}
		}
	}

	return true;
}

void TerrainToolMain::keyEvent(Keyboard::KeyEvent evt, int key)
{
	if (evt == Keyboard::KEY_PRESS)
	{
		switch (key)
		{
		case Keyboard::KEY_ESCAPE:
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

bool TerrainToolMain::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
	if (evt == Mouse::MOUSE_PRESS_RIGHT_BUTTON)
	{
		if (_inputMode == INPUT_MODE::TERRAIN)
		{
			Ray pickRay;
			_scene->getActiveCamera()->pickRay(Rectangle(0, 0, getWidth(), getHeight()), x, y, &pickRay);
			pickRay.setOrigin(_camera.getPosition());

			for (size_t i = 0; i < TerrPager->Param.loadedTerrains.size(); i++)
			{
				TerrainHitFilter hitFilter(TerrPager->Param.loadedTerrains[i]);
				PhysicsController::HitResult hitResult;
				if (Game::getInstance()->getPhysicsController()->rayTest(pickRay, 1000000, &hitResult, &hitFilter))
				{
					if (hitResult.object == TerrPager->Param.loadedTerrains[i]->getNode()->getCollisionObject())
					{
						_selectionRing->setPosition(hitResult.point.x, hitResult.point.z, TerrPager->Param.loadedTerrains[i]);
					}
				}
			}
		}
		return true;
	}
	if (evt == Mouse::MOUSE_PRESS_LEFT_BUTTON)
	{
		RMB = true;
		return true;
	}
	if (evt == Mouse::MOUSE_RELEASE_LEFT_BUTTON)
	{
		RMB = false;
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