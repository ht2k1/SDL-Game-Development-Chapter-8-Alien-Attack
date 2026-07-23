#ifndef __ShooterObject__
#define __ShooterObject__

#include "GameObject.h"
#include "LoaderParams.h"
#include <memory>
#include <string>

class ShooterObject : public GameObject {
    public: 
        virtual ~ShooterObject() {} // for polymorphism
        virtual void load(std::unique_ptr<LoaderParams> const &pParams);
        virtual void draw();
        virtual void update();
        virtual void clean() {} // not implemented in this class
        virtual void collision() {} // not implemented in this class
        virtual std::string type() { return "SDLGameObject"; }

    protected:
        // we won't directly create ShooterObject's
        ShooterObject();

        // draw the animation for the object for the object being destroyed
        void doDyingAnimation();

        // how fast will this object fire bullets? with a counter
        int m_bulletFiringSpeed;
        int m_bulletCounter;

        // how fast will this object move?
        int m_moveSpeed;

        // how long will the death animation takes? with a counter
        int m_dyingTime;
        int m_dyingCounter;

        // has the explosion sound played?
        bool m_bPlayedDeathSound;
};

#endif