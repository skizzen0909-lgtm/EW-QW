// Entities/Bear.h
#ifndef BEAR_H
#define BEAR_H

#include "Entities/Enemy.h"

class Bear : public Enemy {
private:
    bool enraged;
    float rageTimer;
    float rageThreshold;

public:
    Bear(int level = 1);
    ~Bear() override = default;

    void update(float deltaTime) override;
    void takeDamage(int damage) override;

    bool isBearEnraged() const;
    float getRageThreshold() const;
    void enrage();

    static std::unique_ptr<Bear> createCaveBear(int level);
    static std::unique_ptr<Bear> createGrizzly(int level);
};

#endif // BEAR_H
