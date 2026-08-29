// Systems/ComboSystem.cpp
#include "ComboSystem.h"
#include "Entities/Entity.h"
#include <iostream>
#include <algorithm>
#include <sstream>

ComboSystem::ComboSystem()
    : comboTimer(0.0f)
    , comboTimeLimit(3.0f)
    , comboCount(0) {
    initializeDefaultCombos();
}

bool ComboSystem::isSequenceMatched(const std::vector<ComboStep>& buffer, const ComboSequence& sequence) const {
    if (buffer.size() > sequence.steps.size()) return false;
    for (size_t i = 0; i < buffer.size(); ++i) {
        if (buffer[i].abilityId != sequence.steps[i].abilityId) return false;
    }
    return true;
}

void ComboSystem::registerCombo(const ComboSequence& combo) {
    registeredCombos[combo.id] = combo;
}

void ComboSystem::unregisterCombo(const std::string& comboId) {
    registeredCombos.erase(comboId);
}

void ComboSystem::onAbilityUsed(const std::string& abilityId, Entity* caster, Entity* target) {
    ComboStep step;
    step.abilityId = abilityId;
    step.name = abilityId;
    step.damage = 0;
    step.timeWindow = comboTimeLimit;
    step.executed = true;

    currentComboBuffer.push_back(step);
    comboTimer = comboTimeLimit;
    comboCount++;

    for (const auto& [comboId, sequence] : registeredCombos) {
        if (isSequenceMatched(currentComboBuffer, sequence)) {
            if (currentComboBuffer.size() == sequence.steps.size()) {
                std::ostringstream oss;
                oss << "[COMBO] " << sequence.name << " выполнен! (" << sequence.totalDamage << " урона)";
                std::cout << oss.str() << std::endl;

                if (target) {
                    int bonusDamage = sequence.totalDamage;
                    target->takeDamage(bonusDamage);
                    std::ostringstream dmgOss;
                    dmgOss << "[COMBO] Бонусный урон: " << bonusDamage << " по " << target->getName();
                    std::cout << dmgOss.str() << std::endl;
                }

                currentComboBuffer.clear();
                return;
            }
        }
    }

    if (currentComboBuffer.size() > 6) {
        currentComboBuffer.erase(currentComboBuffer.begin());
    }
}

void ComboSystem::update(float deltaTime) {
    if (comboTimer > 0.0f) {
        comboTimer -= deltaTime;
        if (comboTimer <= 0.0f) {
            resetCombo();
        }
    }
}

bool ComboSystem::isComboActive() const {
    return comboTimer > 0.0f && comboCount > 1;
}

int ComboSystem::getComboCount() const {
    return comboCount;
}

std::string ComboSystem::getCurrentComboName() const {
    for (const auto& [comboId, sequence] : registeredCombos) {
        if (isSequenceMatched(currentComboBuffer, sequence)) {
            return sequence.name;
        }
    }
    return "";
}

float ComboSystem::getComboTimer() const {
    return comboTimer;
}

void ComboSystem::resetCombo() {
    currentComboBuffer.clear();
    comboTimer = 0.0f;
    comboCount = 0;
}

const std::unordered_map<std::string, ComboSequence>& ComboSystem::getRegisteredCombos() const {
    return registeredCombos;
}

void ComboSystem::initializeDefaultCombos() {
    ComboSequence doubleStrike;
    doubleStrike.id = "double_strike";
    doubleStrike.name = "Двойной удар";
    doubleStrike.description = "Два быстрых удара подряд";
    doubleStrike.steps = {
        {"basic_attack", "Удар 1", 10, 1.5f, false},
        {"basic_attack", "Удар 2", 15, 1.5f, false}
    };
    doubleStrike.totalDamage = 25;
    doubleStrike.overallTimeLimit = 2.0f;
    registerCombo(doubleStrike);

    ComboSequence fireIceCombo;
    fireIceCombo.id = "fire_ice_blast";
    fireIceCombo.name = "Огненный ледяной взрыв";
    fireIceCombo.description = "Огненное заклинание followed by ice for elemental burst";
    fireIceCombo.steps = {
        {"fire_bolt", "Огненная стрела", 20, 2.0f, false},
        {"ice_bolt", "Ледяная стрела", 25, 2.0f, false}
    };
    fireIceCombo.totalDamage = 60;
    fireIceCombo.overallTimeLimit = 3.0f;
    registerCombo(fireIceCombo);

    ComboSequence tripleSlash;
    tripleSlash.id = "triple_slash";
    tripleSlash.name = "Тройной разрез";
    tripleSlash.description = "Три последовательных разреза";
    tripleSlash.steps = {
        {"basic_attack", "Разрез 1", 10, 1.0f, false},
        {"basic_attack", "Разрез 2", 12, 1.0f, false},
        {"basic_attack", "Разрез 3", 20, 1.0f, false}
    };
    tripleSlash.totalDamage = 42;
    tripleSlash.overallTimeLimit = 2.5f;
    registerCombo(tripleSlash);

    ComboSequence healStrike;
    healStrike.id = "heal_strike";
    healStrike.name = "Исцеляющий удар";
    healStrike.description = "Атака с последующим лечением";
    healStrike.steps = {
        {"basic_attack", "Удар", 10, 1.5f, false},
        {"heal", "Лечение", 0, 2.0f, false}
    };
    healStrike.totalDamage = 10;
    healStrike.overallTimeLimit = 3.0f;
    registerCombo(healStrike);
}
