// Entities/Wolf.cpp
#include "Wolf.h"
#include <iostream>

Wolf::Wolf(int level)
    : Enemy("Волк", "wolf", 30 + level * 10, 5 + level * 2)
    , packLeader(false)
    , howlTimer(0.0f)
    , howlCooldown(15.0f) {
    renderKey = "wolf";
    setLootExperience(5 + level * 3);
}

void Wolf::update(float deltaTime) {
    Enemy::update(deltaTime);
    if (howlTimer > 0) howlTimer -= deltaTime;
}

bool Wolf::isWolfPackLeader() const { return packLeader; }
void Wolf::setPackLeader(bool leader) { packLeader = leader; }

void Wolf::howl() {
    if (!canHowl()) return;
    howlTimer = howlCooldown;
    std::cout << "[WOLF] " << getName() << " howls!" << std::endl;
}

bool Wolf::canHowl() const { return howlTimer <= 0; }

std::unique_ptr<Wolf> Wolf::createAlphaWolf(int level) {
    auto alpha = std::make_unique<Wolf>(level + 2);
    alpha->setPackLeader(true);
    alpha->setName("Альфа-Волк");
    alpha->setAttackPower(static_cast<int>(alpha->getAttackPower() * 1.5f));
    alpha->setLootExperience(alpha->getLootExperience() * 2);
    return alpha;
}
