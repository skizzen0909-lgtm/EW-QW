// Entities/Skeleton.cpp
#include "Skeleton.h"
#include <iostream>

Skeleton::Skeleton(int level)
    : Enemy("Скелет", "skeleton", 25 + level * 8, 8 + level * 2)
    , nightSpawned(false)
    , shield(false)
    , reanimateAbility(true) {
    renderKey = "skeleton";
    setLootExperience(8 + level * 4);
}

void Skeleton::update(float deltaTime) {
    Enemy::update(deltaTime);
}

bool Skeleton::isSkeletonNightSpawned() const { return nightSpawned; }
void Skeleton::setNightSpawned(bool night) { nightSpawned = night; }

bool Skeleton::hasShield() const { return shield; }
void Skeleton::setShield(bool shieldEnabled) { shield = shieldEnabled; }

bool Skeleton::canReanimate() const { return reanimateAbility && !isAlive(); }

void Skeleton::reanimate() {
    if (!canReanimate()) return;
    setHealth(static_cast<int>(getMaxHealth() * 0.3f));
    reanimateAbility = false;
    std::cout << "[SKELETON] " << getName() << " reanimated!" << std::endl;
}

std::unique_ptr<Skeleton> Skeleton::createBoneWarrior(int level) {
    auto warrior = std::make_unique<Skeleton>(level + 1);
    warrior->setName("Костяной Воин");
    warrior->setShield(true);
    warrior->setAttackPower(static_cast<int>(warrior->getAttackPower() * 1.4f));
    return warrior;
}

std::unique_ptr<Skeleton> Skeleton::createLichMinion(int level) {
    auto minion = std::make_unique<Skeleton>(level + 2);
    minion->setName("Слуга Лича");
    minion->setNightSpawned(true);
    minion->setAttackPower(static_cast<int>(minion->getAttackPower() * 1.6f));
    minion->setLootExperience(minion->getLootExperience() * 2);
    return minion;
}
