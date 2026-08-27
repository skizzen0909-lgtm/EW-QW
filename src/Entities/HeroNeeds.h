// Entities/HeroNeeds.h
#ifndef HERONEEDS_H
#define HERONEEDS_H

enum class NeedType {
    HEALTH,
    HUNGER,
    THIRST,
    ENERGY
};

enum class NeedState {
    CRITICAL,    // < 10%
    LOW,         // 10-30%
    NORMAL,      // 30-70%
    HIGH,        // 70-90%
    FULL         // > 90%
};

class HeroNeeds {
private:
    float health;
    float hunger;
    float thirst;
    float energy;

    float maxHealth;
    float maxHunger;
    float maxThirst;
    float maxEnergy;

    float hungerRate;
    float thirstRate;
    float energyRecoveryRate;

    void recalculateEffects();

public:
    HeroNeeds();

    void update(float deltaTime);

    float getHealth() const;
    float getHunger() const;
    float getThirst() const;
    float getEnergy() const;

    float getMaxHealth() const;
    float getMaxHunger() const;
    float getMaxThirst() const;
    float getMaxEnergy() const;

    void setHealth(float value);
    void setHunger(float value);
    void setThirst(float value);
    void setEnergy(float value);

    void addHealth(float amount);
    void addHunger(float amount);
    void addThirst(float amount);
    void addEnergy(float amount);

    void takeDamage(float amount);
    void heal(float amount);
    void eat(float foodValue);
    void drink(float waterValue);
    void rest(float restTime);

    NeedState getHealthState() const;
    NeedState getHungerState() const;
    NeedState getThirstState() const;
    NeedState getEnergyState() const;

    bool isCritical() const;
    bool needsFood() const;
    bool needsWater() const;
    bool needsRest() const;

    float getMovementSpeedModifier() const;
    float getAttackDamageModifier() const;
    float getRegenerationModifier() const;

    void setHungerRate(float rate);
    void setThirstRate(float rate);
};

#endif // HERONEEDS_H
