#ifndef __InputHandler__
#define __InputHandler__

#include <SDL.h>
#include <vector>
#include <utility>
#include "Vector2D.h"


// ADD THIS ENUM:
enum mouse_buttons
{
    LEFT = 0,
    MIDDLE = 1,
    RIGHT = 2
};

class InputHandler {
public:
    static InputHandler* Instance() {
        if (s_pInstance == nullptr) {
            s_pInstance = new InputHandler();
        }
        return s_pInstance;
    }

    // ADD THIS DECLARATION:
    void reset();

    // ADD THIS DECLARATION:
    int getTrigger(int joy, int trigger) const;

    // Init and clean
    void initialiseJoysticks();
    void clean();

    // Event loop update
    void update();

    // Keyboard events
    bool isKeyDown(SDL_Scancode key) const;

    // Joystick / Controller events
    bool joysticksInitialised() const { return m_bJoysticksInitialised; }
    int getAxisX(int joy, int stick) const;
    int getAxisY(int joy, int stick) const;
    bool getButtonState(int joy, int buttonNumber) const;

    // Mouse events (if applicable in your project)
    bool getMouseButtonState(int buttonNumber) const;
    Vector2D* getMousePosition() const;

private:
    InputHandler();
    ~InputHandler();

    // ADD THESE HELPER METHOD DECLARATIONS:
    void addController(int deviceIndex);
    void removeController(SDL_JoystickID instanceID);
    int findControllerSlot(SDL_JoystickID instanceID) const;

    InputHandler(const InputHandler&);
    InputHandler& operator=(const InputHandler&);

    // Private helper functions for handling SDL events
    void onKeyDown();
    void onKeyUp();
    void onMouseMove(SDL_Event& event);
    void onMouseButtonDown(SDL_Event& event);
    void onMouseButtonUp(SDL_Event& event);
    void onJoystickAxisMove(SDL_Event& event);
    void onJoystickButtonDown(SDL_Event& event);
    void onJoystickButtonUp(SDL_Event& event);

    // ADD THESE THREE EVENT HANDLER DECLARATIONS:
    void onControllerAxisMove(SDL_Event& event);
    void onControllerButtonDown(SDL_Event& event);
    void onControllerButtonUp(SDL_Event& event);

    // Member variables
    static InputHandler* s_pInstance;

    const Uint8* m_keystates;

    std::vector<std::pair<Vector2D*, Vector2D*>> m_joystickValues;
    std::vector<SDL_GameController*> m_controllers;
    std::vector<std::vector<bool>> m_buttonStates;

    // ADD THESE MEMBER VARIABLES:
    std::vector<SDL_JoystickID> m_instanceIDs;
    std::vector<std::pair<int, int>> m_triggerValues;

    bool m_bJoysticksInitialised;
    std::vector<bool> m_mouseButtonStates;
    Vector2D* m_mousePosition;

    // ADD THIS DEAD-ZONE VARIABLE (Standard SDL Game Dev value is usually 10000):
    const int m_joystickDeadZone = 10000;
};

typedef InputHandler TheInputHandler;

#endif /* defined(__InputHandler__) */