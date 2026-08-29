// Entities/Bear.cpp
#include "Bear.h"
#include <iostream>

Bear::Bear(int level)
    : Enemy("Медведь", "bear", 80 + level * 20, 12 + level * 3)
    , enraged(false)
    , rageTimer(0.0f)
    , rageThreshold(0.3f) {
    renderKey = "bear";
    setLootExperience(15 + level * 5);
}

void Bear::update(float deltaTime) {
    Enemy::update(deltaTime);
    if (enraged) {
        rageTimer -= deltaTime;
        if (rageTimer <= 0) enraged = false;
    }
}

void Bear::takeDamage(int damage) {
    Enemy::takeDamage(damage);
    float healthRatio = static_cast<float>(getHealth()) / getMaxHealth();
    if (healthRatio <= rageThreshold && !enraged) {
        enrage();
    }
}

bool Bear::isBearEnraged() const { return enraged; }
float Bear::getRageThreshold() const { return rageThreshold; }

void Bear::enrage() {
    enraged = true;
    rageTimer = 10.0f;
    setAttackPower(static_cast<int>(getAttackPower() * 1.5f));
    std::cout << "[BEAR] " << getName() << " enraged!" << std::endl;
}

std::unique_ptr<Bear> Bear::createCaveBear(int level) {
    auto caveBear = std::make_unique<Bear>(level + 1);
    caveBear->setName("Пещерный Медведь");
    caveBear->setAttackPower(static_cast<int>(caveBear->getAttackPower() * 1.3f));
    return caveBear;
}

std::unique_ptr<Bear> Bear::createGrizzly(int level) {
    auto grizzly = std::make_unique<Bear>(level + 3);
    grizzly->setName("Гризли");
    grizzly->setAttackPower(static_cast<int>(grizzly->getAttackPower() * 1.8f));
    grizzly->setLootExperience(grizzly->getLootExperience() * 2);
    return grizzly;
}
