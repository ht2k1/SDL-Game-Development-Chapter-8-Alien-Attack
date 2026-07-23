#ifndef __Enemy__
#define __Enemy__

#include <iostream>
#include "ShooterObject.h"

// Enemy base class 
class Enemy : public ShooterObject {
    public:
        virtual std::string type() { return "Enemy"; }

    protected:
        int m_health;
        Enemy() : ShooterObject() {}
        virtual ~Enemy() {}  // for polymorphism
};

#endif