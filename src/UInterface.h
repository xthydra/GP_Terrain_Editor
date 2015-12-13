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