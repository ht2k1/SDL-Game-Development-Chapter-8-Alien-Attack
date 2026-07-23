#include "Player.h"
#include "Game.h"
#include "InputHandler.h"
#include "TileLayer.h"
#include "BulletHandler.h"
#include "SoundManager.h"

using namespace std;

Player::Player() : ShooterObject(),
        m_invulnerable(false),
        m_invulnerableTime(200),
        m_invulnerableCounter(0) {
    //
}

void Player::collision() {
    // if the player is not invulnerable then set to dying and change values for death animation tile sheet
    if (!m_invulnerable && !TheGame::Instance()->getLevelComplete()) {
        m_textureID = "largeexplosion";
        m_currentFrame = 0;
        m_numFrames = 9;
        m_width = 60;
        m_height = 60;
        m_bDying = true;
    }
}

void Player::load(std::unique_ptr<LoaderParams> const &pParams) {
    // inherited load function
    ShooterObject::load(std::move(pParams));

    // can set up the players inherited values here

    // set up bullets
    m_bulletFiringSpeed = 13;
    m_moveSpeed = 3;

    // we want to be able to fire instantly
    m_bulletCounter = m_bulletFiringSpeed;

    // time it takes for death explosion
    m_dyingTime = 100;
}

void Player::draw() {
    // player has no special drawing requirements
    ShooterObject::draw();
}

void Player::handleAnimation() {
    // if the player is invulnerable we can flash its alpha to let people know
    if (m_invulnerable) {
        // invulnerability is finished, set values back
        if (m_invulnerableCounter == m_invulnerableTime) {
            m_invulnerable = false;
            m_invulnerableCounter = 0;
            m_alpha = 255;
        }
        else {  // otherwise, flash the alpha on and off
            if (m_alpha == 255) {
                m_alpha = 0;
            }
            else {
                m_alpha = 255;
            }
        }

        // increment our counter
        m_invulnerableCounter++;
    }

    // if the player is not dead then we can change the angle with the velocity to give the impression of a moving helicopter
    if (!m_bDead) {
        if (m_velocity.getX() < 0) {
            m_angle = -10.0;
        }
        else if (m_velocity.getX() > 0) {
            m_angle = 10.0;
        }
        else {
            m_angle = 0.0;
        }
    }

    // our standard animation code - for helicopter propellors
    m_currentFrame = int((SDL_GetTicks() / 100) % m_numFrames);
}

void Player::update() {
    // if the level is complete then fly off the screen
    if (TheGame::Instance()->getLevelComplete()) {
        if (m_position.getX() >= TheGame::Instance()->getGameWidth()) {
            TheGame::Instance()->setCurrentLevel(TheGame::Instance()->getCurrentLevel() + 1);
        }
        else {
            m_velocity.setY(0);
            m_velocity.setX(3);
            ShooterObject::update();
            handleAnimation();
        }
    }
    else {
        // if the player is not doing its death animation then update it normally
        if (!m_bDying) {
            // reset velocity
            m_velocity.setX(0);
            m_velocity.setY(0);

            // get input
            handleInput();

            // do normal position += velocity update
            ShooterObject::update();

            // update the animation
            handleAnimation();
        }
        else {  // if the player is doing the death animation
            m_currentFrame = int((SDL_GetTicks() / 100) % m_numFrames);

            // if the death animation has completed
            if (m_dyingCounter == m_dyingTime) {
                // ressurect the player
                ressurect();
            }

            m_dyingCounter++;
        }
    }
}

void Player::ressurect() {
    TheGame::Instance()->setPlayerLives(TheGame::Instance()->getPlayerLives() - 1);

    m_position.setX(10);
    m_position.setY(200);
    m_bDying = false;

    m_textureID = "player";

    m_currentFrame = 0;
    m_numFrames = 5;
    m_width = 101;
    m_height = 46;

    m_dyingCounter = 0;
    m_invulnerable = true;
}

void Player::clean() {
    ShooterObject::clean();
}

void Player::handleInput() {
    if (!m_bDead) {
        // Defensive reset of velocities
        m_velocity.setX(0);
        m_velocity.setY(0);

        int targetX = 0;
        int targetY = 0;
        bool firePressed = false;

        // 1. GAME CONTROLLER INPUTS (GameSir G8 / Bluetooth Gamepad)
        if (TheInputHandler::Instance()->joysticksInitialised()) {
            // Read Analog Left Thumbstick (returns -1, 0, or 1 based on deadzone)
            targetX = TheInputHandler::Instance()->getAxisX(1, 1);
            targetY = TheInputHandler::Instance()->getAxisY(1, 1);

            // Explicit D-Pad Fallback check if thumbstick is idle
            if (targetX == 0) {
                if (TheInputHandler::Instance()->getButtonState(1, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) targetX = -1;
                if (TheInputHandler::Instance()->getButtonState(1, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) targetX = 1;
            }
            if (targetY == 0) {
                if (TheInputHandler::Instance()->getButtonState(1, SDL_CONTROLLER_BUTTON_DPAD_UP)) targetY = -1;
                if (TheInputHandler::Instance()->getButtonState(1, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) targetY = 1;
            }

            // Check Face Buttons (A, B, X, Y) and Right Bumper (RB/R1) for firing
            if (TheInputHandler::Instance()->getButtonState(1, SDL_CONTROLLER_BUTTON_A) ||
                TheInputHandler::Instance()->getButtonState(1, SDL_CONTROLLER_BUTTON_B) ||
                TheInputHandler::Instance()->getButtonState(1, SDL_CONTROLLER_BUTTON_X) ||
                TheInputHandler::Instance()->getButtonState(1, SDL_CONTROLLER_BUTTON_Y) ||
                TheInputHandler::Instance()->getButtonState(1, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) {
                firePressed = true;
            }
        }

        // 2. KEYBOARD FALLBACK INPUTS (Desktop / Emulator debugging)
        if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_LEFT) || TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_A)) {
            targetX = -1;
        }
        if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_RIGHT) || TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_D)) {
            targetX = 1;
        }
        if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_UP) || TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_W)) {
            targetY = -1;
        }
        if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_DOWN) || TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_S)) {
            targetY = 1;
        }
        if (TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_SPACE) || TheInputHandler::Instance()->isKeyDown(SDL_SCANCODE_Z)) {
            firePressed = true;
        }

        // 3. APPLY HORIZONTAL VELOCITY & BOUNDARY CHECKS
        if (targetX > 0 && (m_position.getX() + m_width) < TheGame::Instance()->getGameWidth()) {
            m_velocity.setX(m_moveSpeed);
        }
        else if (targetX < 0 && m_position.getX() > 0) {
            m_velocity.setX(-m_moveSpeed);
        }

        // 4. APPLY VERTICAL VELOCITY & BOUNDARY CHECKS
        if (targetY > 0 && (m_position.getY() + m_height) < TheGame::Instance()->getGameHeight() - 10) {
            m_velocity.setY(m_moveSpeed);
        }
        else if (targetY < 0 && m_position.getY() > 0) {
            m_velocity.setY(-m_moveSpeed);
        }

        // 5. PROCESS WEAPON FIRING & COOLDOWN
        if (firePressed) {  
            if (m_bulletCounter >= m_bulletFiringSpeed) {
                TheSoundManager::Instance()->playSound("shoot", 0);
                TheBulletHandler::Instance()->addPlayerBullet(
                    m_position.getX() + 90, m_position.getY() + 12, 11, 11, "bullet1", 1, Vector2D(10, 0)
                );
                m_bulletCounter = 0;
            }
            m_bulletCounter++;
        }
        else {
            m_bulletCounter = m_bulletFiringSpeed;
        }
    }
}