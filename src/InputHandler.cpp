#include "InputHandler.h"
#include "Game.h"
#include <iostream>

InputHandler* InputHandler::s_pInstance = 0;

InputHandler::InputHandler() : m_keystates(0),
        m_bJoysticksInitialised(false),
        m_mousePosition(new Vector2D(0, 0)) {
    // create button states for the mouse
    for (int i = 0; i < 3; i++) {
        m_mouseButtonStates.push_back(false);
    }
}

InputHandler::~InputHandler() {
    delete m_mousePosition;
    m_joystickValues.clear();
    m_triggerValues.clear(); // ADDED
    m_controllers.clear();
    m_instanceIDs.clear();
    m_buttonStates.clear();
    m_mouseButtonStates.clear();
}

void InputHandler::clean() {
    // close every controller we opened
    for (unsigned int i = 0; i < m_controllers.size(); i++) {
        if (m_controllers[i] != nullptr) {
            SDL_GameControllerClose(m_controllers[i]);
        }
    }
    m_controllers.clear();
}

void InputHandler::initialiseJoysticks() {
    if (SDL_WasInit(SDL_INIT_JOYSTICK) == 0) {
        SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    }

    if (SDL_WasInit(SDL_INIT_GAMECONTROLLER) == 0) {
        SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
    }

    int numJoysticks = SDL_NumJoysticks();
    // DIAGNOSTIC PRINT 1:
    SDL_Log("InputHandler: System detected %d potential joystick/controller devices.", numJoysticks);

    if (numJoysticks > 0) {
        for (int i = 0; i < numJoysticks; i++) {
            if (SDL_IsGameController(i)) {
                SDL_Log("InputHandler: Device index %d is a valid Game Controller!", i);
                addController(i);
            } else {
                SDL_Log("InputHandler: Device index %d is a Joystick, NOT a Game Controller.", i);
            }
        }
        m_bJoysticksInitialised = true;
        SDL_Log("InputHandler: Joysticks/Controllers initialised successfully.");
    } else {
        m_bJoysticksInitialised = false;
        SDL_Log("InputHandler: No joysticks or controllers found during initialization.");
    }
}

void InputHandler::addController(int deviceIndex) {
    SDL_GameController* controller = SDL_GameControllerOpen(deviceIndex);

    if (controller == nullptr) {
        std::cout << "Could not open game controller: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Joystick* underlyingJoystick = SDL_GameControllerGetJoystick(controller);
    SDL_JoystickID instanceID = SDL_JoystickInstanceID(underlyingJoystick);

    // avoid double-adding the same physical controller
    if (findControllerSlot(instanceID) != -1) {
        SDL_GameControllerClose(controller);
        return;
    }

    m_controllers.push_back(controller);
    m_instanceIDs.push_back(instanceID);
    m_joystickValues.push_back(std::make_pair(new Vector2D(0, 0), new Vector2D(0, 0)));
    m_triggerValues.push_back(std::make_pair(0, 0));

    std::vector<bool> tempButtons;
    for (int j = 0; j < SDL_CONTROLLER_BUTTON_MAX; j++) {
        tempButtons.push_back(false);
    }
    m_buttonStates.push_back(tempButtons);

    m_bJoysticksInitialised = true;

    std::cout << "Controller connected: " << SDL_GameControllerName(controller)
              << " (slot " << (m_controllers.size() - 1) << ")\n";
}

void InputHandler::removeController(SDL_JoystickID instanceID) {
    int slot = findControllerSlot(instanceID);
    if (slot == -1) {
        return;
    }

    std::cout << "Controller disconnected (slot " << slot << ")\n";

    SDL_GameControllerClose(m_controllers[slot]);

    delete m_joystickValues[slot].first;
    delete m_joystickValues[slot].second;

    m_controllers.erase(m_controllers.begin() + slot);
    m_instanceIDs.erase(m_instanceIDs.begin() + slot);
    m_joystickValues.erase(m_joystickValues.begin() + slot);
    m_triggerValues.erase(m_triggerValues.begin() + slot);
    m_buttonStates.erase(m_buttonStates.begin() + slot);

    m_bJoysticksInitialised = !m_controllers.empty();
}

int InputHandler::findControllerSlot(SDL_JoystickID instanceID) const {
    for (unsigned int i = 0; i < m_instanceIDs.size(); i++) {
        if (m_instanceIDs[i] == instanceID) {
            return (int)i;
        }
    }
    return -1;
}

void InputHandler::reset() {
    m_mouseButtonStates[LEFT] = false;
    m_mouseButtonStates[RIGHT] = false;
    m_mouseButtonStates[MIDDLE] = false;
}

bool InputHandler::isKeyDown(SDL_Scancode key) const {
    if (m_keystates != 0) {
        if (m_keystates[key] == 1) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

int InputHandler::getAxisX(int joy, int stick) const {
    if (joy < 0 || joy >= (int)m_joystickValues.size()) return 0;

    if (stick == 1) {
        // Check thumbstick value first
        int val = (int)m_joystickValues[joy].first->getX();
        
        // If thumbstick is idle, fallback to D-Pad Digital buttons
        if (val == 0) {
            if (getButtonState(joy, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) return -1;
            if (getButtonState(joy, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) return 1;
        }
        return val;
    }
    else if (stick == 2) {
        return (int)m_joystickValues[joy].second->getX();
    }
    return 0;
}

int InputHandler::getAxisY(int joy, int stick) const {
    if (joy < 0 || joy >= (int)m_joystickValues.size()) return 0;

    if (stick == 1) {
        // Check thumbstick value first
        int val = (int)m_joystickValues[joy].first->getY();
        
        // If thumbstick is idle, fallback to D-Pad Digital buttons
        if (val == 0) {
            if (getButtonState(joy, SDL_CONTROLLER_BUTTON_DPAD_UP)) return -1;
            if (getButtonState(joy, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) return 1;
        }
        return val;
    }
    else if (stick == 2) {
        return (int)m_joystickValues[joy].second->getY();
    }
    return 0;
}

bool InputHandler::getButtonState(int joy, int buttonNumber) const {
    if (joy < 0 || joy >= (int)m_buttonStates.size()) {
        return false;
    }
    if (buttonNumber < 0 || buttonNumber >= (int)m_buttonStates[joy].size()) {
        return false;
    }
    return m_buttonStates[joy][buttonNumber];
}

bool InputHandler::getMouseButtonState(int buttonNumber) const {
    return m_mouseButtonStates[buttonNumber];
}

Vector2D* InputHandler::getMousePosition() const {
    return m_mousePosition;
}

void InputHandler::update() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                TheGame::Instance()->quit();
                break;

            // a controller connected - most important case for Android,
            // since a Bluetooth pad usually finishes pairing after the
            // app is already running
            // Inside InputHandler::update(), where you handle SDL_PollEvent(&event):
            case SDL_CONTROLLERDEVICEADDED:
                SDL_Log("InputHandler: SDL_CONTROLLERDEVICEADDED event fired for device %d!", event.cdevice.which);
                addController(event.cdevice.which);
                break;

            case SDL_JOYDEVICEADDED:
                SDL_Log("InputHandler: SDL_JOYDEVICEADDED event fired for device %d!", event.jdevice.which);
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                removeController(event.cdevice.which);
                SDL_Log("InputHandler: SDL_CONTROLLERDEVICEREMOVED event fired for device %d!", event.jdevice.which);
                break;

            case SDL_CONTROLLERAXISMOTION:
                onControllerAxisMove(event);
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                onControllerButtonDown(event);
                SDL_Log("InputHandler: SDL_CONTROLLERBUTTONDOWN event fired for device %d!", event.jdevice.which);
                break;

            case SDL_CONTROLLERBUTTONUP:
                onControllerButtonUp(event);
                break;

            case SDL_MOUSEMOTION:
                onMouseMove(event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                onMouseButtonDown(event);
                break;

            case SDL_MOUSEBUTTONUP:
                onMouseButtonUp(event);
                break;

            case SDL_KEYDOWN:
                onKeyDown();
                break;

            case SDL_KEYUP:
                onKeyUp();
                break;

            default:
                break;
        }
    }
}

void InputHandler::onKeyDown() {
    m_keystates = SDL_GetKeyboardState(0);
}

void InputHandler::onKeyUp() {
    m_keystates = SDL_GetKeyboardState(0);
}

void InputHandler::onMouseMove(SDL_Event &event) {
    m_mousePosition->setX(event.motion.x);
    m_mousePosition->setY(event.motion.y);
}

void InputHandler::onMouseButtonDown(SDL_Event &event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        m_mouseButtonStates[LEFT] = true;
    }

    if (event.button.button == SDL_BUTTON_MIDDLE) {
        m_mouseButtonStates[MIDDLE] = true;
    }

    if (event.button.button == SDL_BUTTON_RIGHT) {
        m_mouseButtonStates[RIGHT] = true;
    }
}

void InputHandler::onMouseButtonUp(SDL_Event &event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        m_mouseButtonStates[LEFT] = false;
    }

    if (event.button.button == SDL_BUTTON_MIDDLE) {
        m_mouseButtonStates[MIDDLE] = false;
    }

    if (event.button.button == SDL_BUTTON_RIGHT) {
        m_mouseButtonStates[RIGHT] = false;
    }
}

void InputHandler::onControllerAxisMove(SDL_Event &event) {
    int slot = findControllerSlot(event.caxis.which);
    if (slot == -1) {
        return;
    }

    // left stick X
    if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
        if (event.caxis.value > m_joystickDeadZone) {
            m_joystickValues[slot].first->setX(1);
        }
        else if (event.caxis.value < -m_joystickDeadZone) {
            m_joystickValues[slot].first->setX(-1);
        }
        else {
            m_joystickValues[slot].first->setX(0);
        }
    }

    // left stick Y
    if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
        if (event.caxis.value > m_joystickDeadZone) {
            m_joystickValues[slot].first->setY(1);
        }
        else if (event.caxis.value < -m_joystickDeadZone) {
            m_joystickValues[slot].first->setY(-1);
        }
        else {
            m_joystickValues[slot].first->setY(0);
        }
    }

    // right stick X
    if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
        if (event.caxis.value > m_joystickDeadZone) {
            m_joystickValues[slot].second->setX(1);
        }
        else if (event.caxis.value < -m_joystickDeadZone) {
            m_joystickValues[slot].second->setX(-1);
        }
        else {
            m_joystickValues[slot].second->setX(0);
        }
    }

    // right stick Y
    if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
        if (event.caxis.value > m_joystickDeadZone) {
            m_joystickValues[slot].second->setY(1);
        }
        else if (event.caxis.value < -m_joystickDeadZone) {
            m_joystickValues[slot].second->setY(-1);
        }
        else {
            m_joystickValues[slot].second->setY(0);
        }
    }

    // ADDED: Left Trigger (Values range from 0 to 32767)
    if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
        if (event.caxis.value > m_joystickDeadZone) {
            m_triggerValues[slot].first = event.caxis.value;
        } else {
            m_triggerValues[slot].first = 0;
        }
    }

    // ADDED: Right Trigger (Values range from 0 to 32767)
    if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
        if (event.caxis.value > m_joystickDeadZone) {
            m_triggerValues[slot].second = event.caxis.value;
        } else {
            m_triggerValues[slot].second = 0;
        }
    }
}

void InputHandler::onControllerButtonDown(SDL_Event &event) {
    int slot = findControllerSlot(event.cbutton.which);
    if (slot == -1) {
        return;
    }
    m_buttonStates[slot][event.cbutton.button] = true;
    std::cout << "button: " << (int)event.cbutton.button << std::endl;
}

void InputHandler::onControllerButtonUp(SDL_Event &event) {
    int slot = findControllerSlot(event.cbutton.which);
    if (slot == -1) {
        return;
    }
    m_buttonStates[slot][event.cbutton.button] = false;
}

// ADDED: Getter for trigger axes
int InputHandler::getTrigger(int joy, int trigger) const {
    if (joy < 0 || joy >= (int)m_triggerValues.size()) return 0;
    if (trigger == 1) return m_triggerValues[joy].first;
    if (trigger == 2) return m_triggerValues[joy].second;
    return 0;
}