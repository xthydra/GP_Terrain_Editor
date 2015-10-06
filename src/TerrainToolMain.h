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

#include "gameplay.h"

#include "main.h"

/*i'm using that to confirm a thread termination*/
struct Threads
{
	bool blendmap,
		normalmap,
		objectPos;

	void blendBool(){ blendmap = true, normalmap = false, objectPos = false; }
	void normalBool(){ blendmap = false, normalmap = true, objectPos = false; }
	void objectBool(){ blendmap = false, normalmap = false, objectPos = true; }
};


using namespace gameplay;

/**
 * Main game class.
 */
class TerrainToolMain: public Game, Control::Listener
{
public:

    /**
     * Constructor.
     */
    TerrainToolMain();
    
    /**
     * @see Game::keyEvent
     */
	void keyEvent(Keyboard::KeyEvent evt, int key);

	/**
	* @see Game::mouseEvent
	*/
	bool mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta);

protected:

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
private:
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

	//std::vector<std::thread> threads;
	std::vector<std::future<void>> threads;
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
    enum INPUT_MODE { NAVIGATION, TERRAIN, PAINT,GENERATOR };
       
    /**
     * How fast we fly.
     **/
    float MOVE_SPEED;

	/**
	* The Terrain Pager
	**/
	TerrainPager * TerrPager;

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
    FirstPersonCamera _camera;
    
    /**
     * The scene light.
     **/
    Light* _light;
    
    /**
     * A ui form.
     **/
    Form* _mainForm;
    
    /**
     * A ui form.
     **/
    Form* _sizeForm;
    
    /**
     * UI form for generating terrains.
     **/
    Form* _generateTerrainsForm;

	/**
	* UI form for modiying blendmaps intensity.
	**/
	Form* _generateBlendmapsForm;

	/**
	* UI form for modiying noise parameters.
	**/
	Form* _noiseForm;
    
    /**
     * UI form for loading files.
     **/
    Form* _loadForm;

	/**
	* UI form for saving files.
	**/
	Form* _saveForm;
    
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
     * The scale of the selection.
     **/
    float _selectionScale;
    
    /**
     * The current input mode.
     **/
    INPUT_MODE _inputMode;
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
