// Systems/DivineSystem.cpp
#include "DivineSystem.h"
#include <iostream>
#include <sstream>
#include <algorithm>

DivineSystem::DivineSystem()
    : currentFaith(10.0f)
    , maxFaith(100.0f)
    , faithRegenRate(0.5f)
    , currentPower(1.0f) {
    karmaScales[KarmaScale::WISDOM] = 0.0f;
    karmaScales[KarmaScale::MERCY] = 0.0f;
    karmaScales[KarmaScale::RAGE] = 0.0f;
    karmaScales[KarmaScale::CUNNING] = 0.0f;
    karmaScales[KarmaScale::CHAOS] = 0.0f;
    initializeDefaultAbilities();
}

void DivineSystem::initializeDefaultAbilities() {
    DivineAbility blessPower;
    blessPower.id = "bless_power";
    blessPower.name = "Благословение силы";
    blessPower.description = "Увеличивает урон героя на 30% на 10 секунд";
    blessPower.type = DivineAbilityType::BLESS_POWER;
    blessPower.faithRequired = 5.0f;
    blessPower.cooldown = 15.0f;
    blessPower.unlocked = true;
    abilities[DivineAbilityType::BLESS_POWER] = blessPower;

    DivineAbility blessHealth;
    blessHealth.id = "bless_health";
    blessHealth.name = "Благословение здоровья";
    blessHealth.description = "Исцеляет героя на 25% максимального здоровья";
    blessHealth.type = DivineAbilityType::BLESS_HEALTH;
    blessHealth.faithRequired = 10.0f;
    blessHealth.cooldown = 20.0f;
    blessHealth.unlocked = true;
    abilities[DivineAbilityType::BLESS_HEALTH] = blessHealth;

    DivineAbility lightning;
    lightning.id = "lightning";
    lightning.name = "Удар молнией";
    lightning.description = "Наносит молнию по области, поражая всех врагов";
    lightning.type = DivineAbilityType::LIGHTNING;
    lightning.faithRequired = 20.0f;
    lightning.cooldown = 30.0f;
    lightning.unlocked = false;
    lightning.unlockCondition = "Убить 10 врагов";
    abilities[DivineAbilityType::LIGHTNING] = lightning;

    DivineAbility weatherControl;
    weatherControl.id = "weather_control";
    weatherControl.name = "Управление погодой";
    weatherControl.description = "Изменяет погоду на 30 секунд";
    weatherControl.type = DivineAbilityType::WEATHER_CONTROL;
    weatherControl.faithRequired = 30.0f;
    weatherControl.cooldown = 60.0f;
    weatherControl.unlocked = false;
    weatherControl.unlockCondition = "Достичь 20 уровня";
    abilities[DivineAbilityType::WEATHER_CONTROL] = weatherControl;

    DivineAbility emotionControl;
    emotionControl.id = "emotion_control";
    emotionControl.name = "Влиять на эмоции";
    emotionControl.description = "Управляет эмоциями героя";
    emotionControl.type = DivineAbilityType::EMOTION_CONTROL;
    emotionControl.faithRequired = 15.0f;
    emotionControl.cooldown = 10.0f;
    emotionControl.unlocked = false;
    emotionControl.unlockCondition = "Помочь 5 NPC";
    abilities[DivineAbilityType::EMOTION_CONTROL] = emotionControl;

    DivineAbility enemyDistract;
    enemyDistract.id = "enemy_distract";
    enemyDistract.name = "Отвлечение врага";
    enemyDistract.description = "Враг засыпает на 5 секунд";
    enemyDistract.type = DivineAbilityType::ENEMY_DISTRACT;
    enemyDistract.faithRequired = 8.0f;
    enemyDistract.cooldown = 25.0f;
    enemyDistract.unlocked = false;
    enemyDistract.unlockCondition = "Пройти пещеру Тени";
    abilities[DivineAbilityType::ENEMY_DISTRACT] = enemyDistract;

    DivineAbility timeManip;
    timeManip.id = "time_manip";
    timeManip.name = "Манипуляция временем";
    timeManip.description = "Ускоряет или замедляет время для героя";
    timeManip.type = DivineAbilityType::TIME_MANIPULATION;
    timeManip.faithRequired = 40.0f;
    timeManip.cooldown = 90.0f;
    timeManip.unlocked = false;
    timeManip.unlockCondition = "Найти Артефакт Времени";
    abilities[DivineAbilityType::TIME_MANIPULATION] = timeManip;

    DivineAbility directionGuide;
    directionGuide.id = "direction_guide";
    directionGuide.name = "Указать направление";
    directionGuide.description = "Подсказывает герою куда идти";
    directionGuide.type = DivineAbilityType::DIRECTION_GUIDE;
    directionGuide.faithRequired = 3.0f;
    directionGuide.cooldown = 5.0f;
    directionGuide.unlocked = true;
    abilities[DivineAbilityType::DIRECTION_GUIDE] = directionGuide;
}

void DivineSystem::update(float deltaTime) {
    currentFaith = std::min(currentFaith + faithRegenRate * deltaTime, maxFaith);

    for (auto& [type, ability] : abilities) {
        if (ability.currentCooldown > 0) {
            ability.currentCooldown -= deltaTime;
            if (ability.currentCooldown < 0) ability.currentCooldown = 0;
        }
    }

    currentPower = 1.0f + (currentFaith / maxFaith) * 2.0f;
    for (auto& [scale, value] : karmaScales) {
        currentPower += std::abs(value) * 0.01f;
    }
}

float DivineSystem::getFaith() const { return currentFaith; }
float DivineSystem::getMaxFaith() const { return maxFaith; }
float DivineSystem::getPower() const { return currentPower; }
float DivineSystem::getKarma(KarmaScale scale) const {
    auto it = karmaScales.find(scale);
    return (it != karmaScales.end()) ? it->second : 0.0f;
}

void DivineSystem::addFaith(float amount) {
    currentFaith = std::min(currentFaith + amount, maxFaith);
    std::ostringstream oss;
    oss << "[DIVINE] Вера increased: " << currentFaith << "/" << maxFaith;
    std::cout << oss.str() << std::endl;
}

void DivineSystem::drainFaith(float amount) {
    currentFaith = std::max(currentFaith - amount, 0.0f);
}

void DivineSystem::addPower(float amount) { currentPower += amount; }

void DivineSystem::modifyKarma(KarmaScale scale, float amount) {
    karmaScales[scale] = std::clamp(karmaScales[scale] + amount, -100.0f, 100.0f);
    std::ostringstream oss;
    oss << "[KARMA] Scale " << static_cast<int>(scale) << " changed to " << karmaScales[scale];
    std::cout << oss.str() << std::endl;
}

bool DivineSystem::canUseAbility(DivineAbilityType type) const {
    auto it = abilities.find(type);
    if (it == abilities.end()) return false;
    const auto& ability = it->second;
    return ability.unlocked && currentFaith >= ability.faithRequired && ability.currentCooldown <= 0;
}

bool DivineSystem::useAbility(DivineAbilityType type) {
    if (!canUseAbility(type)) return false;

    auto& ability = abilities[type];
    currentFaith -= ability.faithRequired;
    ability.currentCooldown = ability.cooldown;

    std::ostringstream oss;
    oss << "[DIVINE] Used: " << ability.name << " (faith: " << currentFaith << ")";
    std::cout << oss.str() << std::endl;

    if (ability.execute) ability.execute();
    return true;
}

void DivineSystem::unlockAbility(DivineAbilityType type) {
    auto it = abilities.find(type);
    if (it != abilities.end()) {
        it->second.unlocked = true;
        std::ostringstream oss;
        oss << "[DIVINE] Unlocked: " << it->second.name;
        std::cout << oss.str() << std::endl;
    }
}

void DivineSystem::unlockAbilityByCondition(const std::string& condition) {
    for (auto& [type, ability] : abilities) {
        if (!ability.unlocked && ability.unlockCondition == condition) {
            ability.unlocked = true;
            std::ostringstream oss;
            oss << "[DIVINE] Unlocked by condition: " << ability.name;
            std::cout << oss.str() << std::endl;
        }
    }
}

const DivineAbility* DivineSystem::getAbility(DivineAbilityType type) const {
    auto it = abilities.find(type);
    return (it != abilities.end()) ? &it->second : nullptr;
}

std::vector<DivineAbility> DivineSystem::getAvailableAbilities() const {
    std::vector<DivineAbility> result;
    for (const auto& [type, ability] : abilities) {
        if (ability.unlocked) result.push_back(ability);
    }
    return result;
}

std::vector<DivineAbility> DivineSystem::getUnlockedAbilities() const {
    return getAvailableAbilities();
}

void DivineSystem::onHeroFaithGrows(float faithIncrease) {
    addFaith(faithIncrease * 0.1f);
}

void DivineSystem::onHeroPrays() {
    addFaith(5.0f);
    modifyKarma(KarmaScale::MERCY, 1.0f);
}

void DivineSystem::onGoodDeed() {
    modifyKarma(KarmaScale::MERCY, 5.0f);
    modifyKarma(KarmaScale::WISDOM, 2.0f);
}

void DivineSystem::onEvilDeed() {
    modifyKarma(KarmaScale::RAGE, 5.0f);
    modifyKarma(KarmaScale::CHAOS, 3.0f);
}

void DivineSystem::onHeroKillsEnemy() {
    addFaith(2.0f);
    unlockAbilityByCondition("Убить 10 врагов");
}

void DivineSystem::onHeroHelpsNPC() {
    addFaith(5.0f);
    modifyKarma(KarmaScale::MERCY, 3.0f);
    unlockAbilityByCondition("Помочь 5 NPC");
}
