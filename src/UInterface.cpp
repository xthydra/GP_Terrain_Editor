#include "Main.h"

UI::UI(Scene * scene, Control::Listener * listener, FirstPersonCamera * camera_, Pager * pager_)
{
	_scene = scene;
	camera = camera_;
	pager = pager_;

	inputMode = NAVIGATION;

	mainForm = Form::create("res/forms/main.form");

	Control *control = mainForm->getControl("EditorButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("NavigateButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("GeneratorButton");
	control->addListener(listener, Control::Listener::CLICK);

	//=============NAVIGATOR
	Slider *slider = (Slider *)mainForm->getControl("MoveSlider");
	slider->addListener(listener, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)mainForm->getControl("LODSlider");
	slider->addListener(listener, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)mainForm->getControl("TerrainsRenderSlider");
	slider->addListener(listener, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)mainForm->getControl("ObjectsRenderSlider");
	slider->addListener(listener, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)mainForm->getControl("LoadSlider");
	slider->addListener(listener, Control::Listener::VALUE_CHANGED);

	slider = (Slider *)mainForm->getControl("UnloadSlider");
	slider->addListener(listener, Control::Listener::VALUE_CHANGED);

	mainForm->getControl("NavigateToolBar")->setVisible(true);
	//=============

	//=============GENERATOR
	control = mainForm->getControl("GenerateTerrainsButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("GenerateBlendMapsButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("GenerateNormalMapsButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("GenerateObjectsPosButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("SaveButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("LoadButton");
	control->addListener(listener, Control::Listener::CLICK);
	//=============

	//=============TERRAIN_EDITOR
	slider = (Slider *)mainForm->getControl("SizeSlider");
	slider->addListener(listener, Control::Listener::VALUE_CHANGED);

	control = mainForm->getControl("AlignButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("RaiseButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("LowerButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("FlattenButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("SmoothButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("PaintButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = mainForm->getControl("SetTexture");
	control->addListener(listener, Control::Listener::CLICK);

	//===============
	generateTerrainsForm = Form::create("res/forms/generateTerrains.form");
	generateTerrainsForm->setVisible(false);

	control = generateTerrainsForm->getControl("CancelGenerateTerrainsButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = generateTerrainsForm->getControl("ConfirmGenerateTerrainsButton");
	control->addListener(listener, Control::Listener::CLICK);
	//===============	
	generateBlendmapsForm = Form::create("res/forms/generateBlendmaps.form");
	generateBlendmapsForm->setVisible(false);

	control = generateBlendmapsForm->getControl("CancelGenerateBlendmapsButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = generateBlendmapsForm->getControl("ConfirmGenerateBlendmapsButton");
	control->addListener(listener, Control::Listener::CLICK);
	//===============
	loadForm = Form::create("res/forms/load.form");
	loadForm->setVisible(false);

	control = loadForm->getControl("CancelLoadButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = loadForm->getControl("ConfirmLoadButton");
	control->addListener(listener, Control::Listener::CLICK);
	//===============
	generateObjectsForm = Form::create("res/forms/generateObjects.form");
	generateObjectsForm->setVisible(false);

	control = generateObjectsForm->getControl("CancelGenerateObjectsButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = generateObjectsForm->getControl("ConfirmGenerateObjectsButton");
	control->addListener(listener, Control::Listener::CLICK);
	//===============SAVE
	saveForm = Form::create("res/forms/save.form");
	saveForm->setVisible(false);

	control = saveForm->getControl("RAWHeightmapsButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = saveForm->getControl("BlendmapsButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = saveForm->getControl("ObjectsButton");
	control->addListener(listener, Control::Listener::CLICK);

	control = saveForm->getControl("CancelSaveButton");
	control->addListener(listener, Control::Listener::CLICK);
	//===============

	control = mainForm->getControl("RaiseButton");
	Button * button = (Button *)control;
	normalButton = button->getSkinColor(gameplay::Control::State::NORMAL);
	activeButton = button->getSkinColor(gameplay::Control::State::ACTIVE);
}

void UI::controlEvents(Control* control, Control::Listener::EventType evt)
{
	//=====================MAIN_FORM
	if (strcmp(control->getId(), "NavigateButton") == 0)
	{
		inputMode = NAVIGATION;
		mainForm->getControl("TerrainEditorbar")->setVisible(false);
		mainForm->getControl("GeneratorToolBar")->setVisible(false);
		mainForm->getControl("NavigateToolBar")->setVisible(true);
	}
	else if (strcmp(control->getId(), "EditorButton") == 0)
	{
		inputMode = EDITING;
		mainForm->getControl("NavigateToolBar")->setVisible(false);
		mainForm->getControl("GeneratorToolBar")->setVisible(false);
		mainForm->getControl("TerrainEditorbar")->setVisible(true);
	}
	else if (strcmp(control->getId(), "GeneratorButton") == 0)
	{
		inputMode = GENERATOR;
		mainForm->getControl("NavigateToolBar")->setVisible(false);
		mainForm->getControl("TerrainEditorbar")->setVisible(false);
		mainForm->getControl("GeneratorToolBar")->setVisible(true);
	}
	//=============================

	//=====================GENERATOR
	else if (strcmp(control->getId(), "GenerateTerrainsButton") == 0)
	{
		mainForm->setVisible(false);
		generateTerrainsForm->setVisible(true);
	}
	else if (strcmp(control->getId(), "GenerateBlendMapsButton") == 0)
	{
		if (pager)
		{
			if (pager->zoneList.size() > 0)
			{
				mainForm->setVisible(false);
				generateBlendmapsForm->setVisible(true);
			}
		}
	}
	else if (strcmp(control->getId(), "GenerateNormalMapsButton") == 0)
	{
		if (pager)
		{
			if (pager->zoneList.size() > 0)
			{
				std::vector<std::vector<HeightField*>> fields;
				fields.resize(pager->zoneList.size());
				for (size_t i = 0; i < pager->zoneList.size(); i++)
				{
					for (size_t g = 0; g < pager->zoneList[i].size(); g++)
					{
						fields[i].push_back(pager->zoneList[i][g]->heightField);
					}
				}
				TerrainGenerator terrainGenerator;
				terrainGenerator.createNormalmaps(pager->parameters.scale.y, pager->parameters.scale.x, pager->parameters.heightFieldResolution, fields);
			}
		}
	}
	else if (strcmp(control->getId(), "GenerateObjectsPosButton") == 0)
	{
		mainForm->setVisible(false);
		generateObjectsForm->setVisible(true);
	}
	else if (strcmp(control->getId(), "SaveButton") == 0)
	{
		mainForm->setVisible(false);
		saveForm->setVisible(true);
	}
	else if (strcmp(control->getId(), "LoadButton") == 0)
	{
		loadForm->setVisible(true);
		mainForm->setVisible(false);
	}

	//=============================

	//====================NAVIGATOR
	else if (strcmp(control->getId(), "MoveSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		camera->setMoveSpeed(slider->getValue());
	}
	else if (strcmp(control->getId(), "LODSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		pager->parameters.lodQuality = slider->getValue();
	}
	else if (strcmp(control->getId(), "LoadSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		if (pager->parameters.distanceTerrainUnload > (slider->getValue() * pager->parameters.boundingBox))
		{
			pager->parameters.distanceTerrainLoad = (slider->getValue() * pager->parameters.boundingBox);
		}
		else if (pager->parameters.distanceTerrainUnload < (slider->getValue() * pager->parameters.boundingBox))
		{
			Control * control2 = mainForm->getControl("UnloadSlider");
			Slider * slider2 = (Slider *)control2;
			slider2->setValue(slider->getValue() + 1);
			//setloadSlider value below unloadSlider
			pager->parameters.distanceTerrainUnload = (slider2->getValue() * pager->parameters.boundingBox);
			pager->parameters.distanceTerrainLoad = (slider->getValue() * pager->parameters.boundingBox);
		}
	}
	else if (strcmp(control->getId(), "ObjectsRenderSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		pager->parameters.distanceMaxModelRender = ((slider->getValue() * pager->parameters.boundingBox) * 0.2);
	}
	else if (strcmp(control->getId(), "TerrainsRenderSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		pager->parameters.distanceTerrainMaxRender = (slider->getValue() * pager->parameters.boundingBox);
	}
	else if (strcmp(control->getId(), "UnloadSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		if (pager->parameters.distanceTerrainLoad < (slider->getValue() * pager->parameters.boundingBox))
		{
			pager->parameters.distanceTerrainUnload = (slider->getValue() * pager->parameters.boundingBox);
		}
		else if (pager->parameters.distanceTerrainLoad >(slider->getValue() * pager->parameters.boundingBox))
		{
			Control * control2 = mainForm->getControl("LoadSlider");
			Slider * slider2 = (Slider *)control2;
			slider2->setValue(slider->getValue() - 1);
			//setloadSlider value below unloadSlider
			pager->parameters.distanceTerrainLoad = (slider2->getValue() * pager->parameters.boundingBox);
			pager->parameters.distanceTerrainUnload = (slider->getValue() * pager->parameters.boundingBox);
		}
	}
	//=============================

	//=======================EDITOR
	else if (strcmp(control->getId(), "SizeSlider") == 0)
	{
		Slider * slider = (Slider *)control;
		float _selectionScale = slider->getValue() * ((pager->parameters.heightFieldResolution / 8) * (pager->parameters.scale.x / 8));

		if (pager->loadedTerrains.size() > 0)
		{
			selectionRing->setScale(_selectionScale, pager->loadedTerrains[0]);
		}
	}
	else if (strcmp(control->getId(), "AlignButton") == 0)
	{
		TerrainEditor TerrEdit;
		std::vector<std::vector<HeightField*>> fields;
		fields.resize(pager->zoneList.size());
		for (size_t i = 0; i < pager->zoneList.size(); i++)
		{
			for (size_t g = 0; g < pager->zoneList[i].size(); g++)
			{
				fields[i].push_back(pager->zoneList[i][g]->heightField);
			}
		}
		TerrEdit.aligningTerrainsVertexes(fields);
		pager->reloadTerrains();
	}
	else if (strcmp(control->getId(), "RaiseButton") == 0)
	{
		control = mainForm->getControl("RaiseButton");
		Button * button = (Button *)control;
		button->setSkinColor(activeButton, gameplay::Control::State::NORMAL);
		control = mainForm->getControl("LowerButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = mainForm->getControl("FlattenButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = mainForm->getControl("SmoothButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
	}
	else if (strcmp(control->getId(), "LowerButton") == 0)
	{
		control = mainForm->getControl("LowerButton");
		Button * button = (Button *)control;
		button->setSkinColor(activeButton, gameplay::Control::State::NORMAL);
		control = mainForm->getControl("RaiseButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = mainForm->getControl("FlattenButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = mainForm->getControl("SmoothButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
	}
	else if (strcmp(control->getId(), "FlattenButton") == 0)
	{
		control = mainForm->getControl("FlattenButton");
		Button * button = (Button *)control;
		button->setSkinColor(activeButton, gameplay::Control::State::NORMAL);
		control = mainForm->getControl("LowerButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = mainForm->getControl("RaiseButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = mainForm->getControl("SmoothButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
	}
	else if (strcmp(control->getId(), "SmoothButton") == 0)
	{
		control = mainForm->getControl("SmoothButton");
		Button * button = (Button *)control;
		button->setSkinColor(activeButton, gameplay::Control::State::NORMAL);
		control = mainForm->getControl("LowerButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = mainForm->getControl("RaiseButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
		control = mainForm->getControl("FlattenButton");
		button = (Button *)control;
		if (button->getSkinColor(gameplay::Control::State::NORMAL) == activeButton)
		{
			button->setSkinColor(normalButton, gameplay::Control::State::NORMAL);
		}
	}
	else if (strcmp(control->getId(), "SetTexture") == 0)
	{
		int closest = pager->findTerrain(selectionRing->getPosition());

		int indexX = pager->loadedTerrains[closest]->getNode()->getTranslationWorld().x / pager->parameters.boundingBox;
		int indexZ = pager->loadedTerrains[closest]->getNode()->getTranslationWorld().z / pager->parameters.boundingBox;

		Control * control = mainForm->getControl("Texture0Pos");
		TextBox * textBox = (TextBox *)control;
		pager->zoneList[indexX][indexZ]->texturesLocation[0] = textBox->getText();

		control = mainForm->getControl("Texture1Pos");
		textBox = (TextBox *)control;
		pager->zoneList[indexX][indexZ]->texturesLocation[1] = textBox->getText();

		control = mainForm->getControl("Texture2Pos");
		textBox = (TextBox *)control;
		pager->zoneList[indexX][indexZ]->texturesLocation[2] = textBox->getText();

		std::vector<int> closestV;
		closestV.push_back(closest);
		pager->reload(closestV);
	}
	else if (strcmp(control->getId(), "PaintButton") == 0)
	{
		control = mainForm->getControl("PaintToolbar");
		Container * container = (Container *)control;
		if (container->isVisible() == true)
		{
			container->setVisible(false);
			control = mainForm->getControl("HeightfieldEditor");
			container = (Container *)control;
			container->setVisible(true);
		}
		else
		{
			container->setVisible(true);
			control = mainForm->getControl("HeightfieldEditor");
			container = (Container *)control;
			container->setVisible(false);
		}
	}
	//=============================
	else if (strcmp(control->getId(), "CancelGenerateTerrainsButton") == 0)
	{
		mainForm->setVisible(true);
		generateTerrainsForm->setVisible(false);
	}
	else if (strcmp(control->getId(), "ConfirmGenerateTerrainsButton") == 0)
	{
		mainForm->setVisible(true);
		generateTerrainsForm->setVisible(false);
	}
	//===========================
	else if (strcmp(control->getId(), "CancelGenerateBlendmapsButton") == 0)
	{
		mainForm->setVisible(true);
		generateBlendmapsForm->setVisible(false);
	}
	else if (strcmp(control->getId(), "ConfirmGenerateBlendmapsButton") == 0)
	{
		mainForm->setVisible(true);
		generateBlendmapsForm->setVisible(false);
	}
	//===========================
	else if (strcmp(control->getId(), "CancelLoadButton") == 0)
	{
		mainForm->setVisible(true);
		loadForm->setVisible(false);
	}
	else if (strcmp(control->getId(), "ConfirmLoadButton") == 0)
	{
		mainForm->setVisible(true);
		loadForm->setVisible(false);
	}
	//===========================
	else if (strcmp(control->getId(), "CancelSaveButton") == 0)
	{
		mainForm->setVisible(true);
		saveForm->setVisible(false);
	}
	//===========================
	else if (strcmp(control->getId(), "CancelGenerateObjectsButton") == 0)
	{
		mainForm->setVisible(true);
		generateObjectsForm->setVisible(false);
	}
	else if (strcmp(control->getId(), "ConfirmGenerateObjectsButton") == 0)
	{
		mainForm->setVisible(true);
		generateObjectsForm->setVisible(false);
	}
}

void UI::updateForms(float elapsedTime)
{
	mainForm->update(elapsedTime);
	generateTerrainsForm->update(elapsedTime);
	generateBlendmapsForm->update(elapsedTime);
	generateObjectsForm->update(elapsedTime);
	loadForm->update(elapsedTime);
	saveForm->update(elapsedTime);
}

void UI::renderForms()
{
	if (mainForm->isVisible())
	{
		mainForm->draw();
	}
	if (generateTerrainsForm->isVisible())
	{
		generateTerrainsForm->draw();
	}
	if (generateBlendmapsForm->isVisible())
	{
		generateBlendmapsForm->draw();
	}
	if (loadForm->isVisible())
	{
		loadForm->draw();
	}
	if (saveForm->isVisible())
	{
		saveForm->draw();
	}
	if (generateObjectsForm->isVisible())
	{
		generateObjectsForm->draw();
	}
}

