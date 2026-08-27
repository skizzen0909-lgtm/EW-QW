// Systems/DivineSystem.h
#ifndef DIVINESYSTEM_H
#define DIVINESYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

enum class DivineAbilityType {
    BLESS_POWER,
    BLESS_HEALTH,
    DIRECTION_GUIDE,
    LIGHTNING,
    WEATHER_CONTROL,
    EMOTION_CONTROL,
    ENEMY_DISTRACT,
    TIME_MANIPULATION,
    FAITH_DRAIN,
    KARMA_SHIFT
};

enum class KarmaScale {
    WISDOM,
    MERCY,
    RAGE,
    CUNNING,
    CHAOS
};

struct DivineAbility {
    std::string id;
    std::string name;
    std::string description;
    DivineAbilityType type;
    float faithRequired;
    float cooldown;
    float currentCooldown;
    bool unlocked;
    std::string unlockCondition;
    std::function<void()> execute;

    DivineAbility()
        : type(DivineAbilityType::BLESS_POWER)
        , faithRequired(0.0f), cooldown(0.0f), currentCooldown(0.0f)
        , unlocked(false) {}
};

class DivineSystem {
private:
    float currentFaith;
    float maxFaith;
    float faithRegenRate;
    float currentPower;

    std::unordered_map<KarmaScale, float> karmaScales;
    std::unordered_map<DivineAbilityType, DivineAbility> abilities;
    std::vector<std::string> unlockedAbilities;

    void initializeDefaultAbilities();

public:
    DivineSystem();
    ~DivineSystem() = default;

    void update(float deltaTime);

    float getFaith() const;
    float getMaxFaith() const;
    float getPower() const;
    float getKarma(KarmaScale scale) const;

    void addFaith(float amount);
    void drainFaith(float amount);
    void addPower(float amount);

    void modifyKarma(KarmaScale scale, float amount);

    bool canUseAbility(DivineAbilityType type) const;
    bool useAbility(DivineAbilityType type);
    void unlockAbility(DivineAbilityType type);
    void unlockAbilityByCondition(const std::string& condition);

    const DivineAbility* getAbility(DivineAbilityType type) const;
    std::vector<DivineAbility> getAvailableAbilities() const;
    std::vector<DivineAbility> getUnlockedAbilities() const;

    void onHeroFaithGrows(float faithIncrease);
    void onHeroPrays();
    void onGoodDeed();
    void onEvilDeed();
    void onHeroKillsEnemy();
    void onHeroHelpsNPC();

    DivineSystem(const DivineSystem&) = delete;
    DivineSystem& operator=(const DivineSystem&) = delete;
};

#endif // DIVINESYSTEM_H
