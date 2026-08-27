// Entities/HeroNeeds.cpp
#include "HeroNeeds.h"
#include <algorithm>

HeroNeeds::HeroNeeds()
    : health(100.0f), hunger(100.0f), thirst(100.0f), energy(100.0f)
    , maxHealth(100.0f), maxHunger(100.0f), maxThirst(100.0f), maxEnergy(100.0f)
    , hungerRate(1.0f), thirstRate(1.5f), energyRecoveryRate(5.0f) {
}

void HeroNeeds::update(float deltaTime) {
    hunger -= hungerRate * deltaTime;
    thirst -= thirstRate * deltaTime;

    if (hunger <= 0) { hunger = 0; health -= 0.5f * deltaTime; }
    if (thirst <= 0) { thirst = 0; health -= 0.7f * deltaTime; }

    if (energy > 0 && (hunger < maxHunger * 0.3f || thirst < maxThirst * 0.3f)) {
        energy -= 1.0f * deltaTime;
    }

    energy = std::min(energy + energyRecoveryRate * deltaTime, maxEnergy);

    health = std::clamp(health, 0.0f, maxHealth);
    hunger = std::clamp(hunger, 0.0f, maxHunger);
    thirst = std::clamp(thirst, 0.0f, maxThirst);
    energy = std::clamp(energy, 0.0f, maxEnergy);
}

float HeroNeeds::getHealth() const { return health; }
float HeroNeeds::getHunger() const { return hunger; }
float HeroNeeds::getThirst() const { return thirst; }
float HeroNeeds::getEnergy() const { return energy; }

float HeroNeeds::getMaxHealth() const { return maxHealth; }
float HeroNeeds::getMaxHunger() const { return maxHunger; }
float HeroNeeds::getMaxThirst() const { return maxThirst; }
float HeroNeeds::getMaxEnergy() const { return maxEnergy; }

void HeroNeeds::setHealth(float value) { health = std::clamp(value, 0.0f, maxHealth); }
void HeroNeeds::setHunger(float value) { hunger = std::clamp(value, 0.0f, maxHunger); }
void HeroNeeds::setThirst(float value) { thirst = std::clamp(value, 0.0f, maxThirst); }
void HeroNeeds::setEnergy(float value) { energy = std::clamp(value, 0.0f, maxEnergy); }

void HeroNeeds::addHealth(float amount) { health = std::clamp(health + amount, 0.0f, maxHealth); }
void HeroNeeds::addHunger(float amount) { hunger = std::clamp(hunger + amount, 0.0f, maxHunger); }
void HeroNeeds::addThirst(float amount) { thirst = std::clamp(thirst + amount, 0.0f, maxThirst); }
void HeroNeeds::addEnergy(float amount) { energy = std::clamp(energy + amount, 0.0f, maxEnergy); }

void HeroNeeds::takeDamage(float amount) {
    health = std::clamp(health - amount, 0.0f, maxHealth);
}

void HeroNeeds::heal(float amount) {
    health = std::clamp(health + amount, 0.0f, maxHealth);
}

void HeroNeeds::eat(float foodValue) {
    hunger = std::clamp(hunger + foodValue, 0.0f, maxHunger);
}

void HeroNeeds::drink(float waterValue) {
    thirst = std::clamp(thirst + waterValue, 0.0f, maxThirst);
}

void HeroNeeds::rest(float restTime) {
    energy = std::clamp(energy + restTime * energyRecoveryRate, 0.0f, maxEnergy);
}

NeedState HeroNeeds::getHealthState() const {
    float ratio = health / maxHealth;
    if (ratio < 0.1f) return NeedState::CRITICAL;
    if (ratio < 0.3f) return NeedState::LOW;
    if (ratio < 0.7f) return NeedState::NORMAL;
    if (ratio < 0.9f) return NeedState::HIGH;
    return NeedState::FULL;
}

NeedState HeroNeeds::getHungerState() const {
    float ratio = hunger / maxHunger;
    if (ratio < 0.1f) return NeedState::CRITICAL;
    if (ratio < 0.3f) return NeedState::LOW;
    if (ratio < 0.7f) return NeedState::NORMAL;
    if (ratio < 0.9f) return NeedState::HIGH;
    return NeedState::FULL;
}

NeedState HeroNeeds::getThirstState() const {
    float ratio = thirst / maxThirst;
    if (ratio < 0.1f) return NeedState::CRITICAL;
    if (ratio < 0.3f) return NeedState::LOW;
    if (ratio < 0.7f) return NeedState::NORMAL;
    if (ratio < 0.9f) return NeedState::HIGH;
    return NeedState::FULL;
}

NeedState HeroNeeds::getEnergyState() const {
    float ratio = energy / maxEnergy;
    if (ratio < 0.1f) return NeedState::CRITICAL;
    if (ratio < 0.3f) return NeedState::LOW;
    if (ratio < 0.7f) return NeedState::NORMAL;
    if (ratio < 0.9f) return NeedState::HIGH;
    return NeedState::FULL;
}

bool HeroNeeds::isCritical() const {
    return health < maxHealth * 0.1f || hunger < maxHunger * 0.1f || thirst < maxThirst * 0.1f;
}

bool HeroNeeds::needsFood() const { return hunger < maxHunger * 0.5f; }
bool HeroNeeds::needsWater() const { return thirst < maxThirst * 0.5f; }
bool HeroNeeds::needsRest() const { return energy < maxEnergy * 0.3f; }

float HeroNeeds::getMovementSpeedModifier() const {
    float modifier = 1.0f;
    if (hunger < maxHunger * 0.3f) modifier *= 0.8f;
    if (thirst < maxThirst * 0.3f) modifier *= 0.7f;
    if (energy < maxEnergy * 0.3f) modifier *= 0.6f;
    return modifier;
}

float HeroNeeds::getAttackDamageModifier() const {
    float modifier = 1.0f;
    if (hunger < maxHunger * 0.3f) modifier *= 0.7f;
    if (thirst < maxThirst * 0.3f) modifier *= 0.8f;
    if (energy < maxEnergy * 0.3f) modifier *= 0.6f;
    return modifier;
}

float HeroNeeds::getRegenerationModifier() const {
    float modifier = 1.0f;
    if (hunger > maxHunger * 0.7f) modifier *= 1.3f;
    if (thirst > maxThirst * 0.7f) modifier *= 1.2f;
    if (energy > maxEnergy * 0.7f) modifier *= 1.4f;
    return modifier;
}

void HeroNeeds::setHungerRate(float rate) { hungerRate = rate; }
void HeroNeeds::setThirstRate(float rate) { thirstRate = rate; }
