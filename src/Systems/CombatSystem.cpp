// Systems/CombatSystem.cpp
#include "CombatSystem.h"
#include "Entities/Entity.h"
#include "Entities/Enemy.h"
#include "Entities/Boss.h"
#include "Entities/Player.h"
#include "Events/CombatEvent.h"
#include "Events/EventManager.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <memory>
#include <sstream>

CombatSystem::CombatSystem()
    : combatActive(false) {
}

void CombatSystem::initiateCombat(std::shared_ptr<Entity> attacker, std::shared_ptr<Entity> defender) {
    if (!attacker || !defender || !attacker->isAlive() || !defender->isAlive()) {
        return;
    }

    combatActive = true;

    std::cout << "Бой начинается между " << attacker->getName() << " и " << defender->getName() << "!\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> damageDist(5, 15);

    int damage = damageDist(gen);
    auto combatEvent = std::make_shared<CombatEvent>(attacker, defender, damage);
    addCombatEvent(combatEvent);
}

void CombatSystem::processCombatRound() {
    if (combatEvents.empty()) return;

    for (const auto& event : combatEvents) {
        if (event && !event->isHandled()) {
            event->execute();

            auto defender = event->getDefender();
            auto attacker = event->getAttacker();

            if (defender && !defender->isAlive()) {
                handleEntityDeath(attacker, defender);
            }
        }
    }

    combatEvents.clear();
}

void CombatSystem::handleEntityDeath(std::shared_ptr<Entity> killer, std::shared_ptr<Entity> victim) {
    if (!killer || !victim) return;

    // Используем getEnemyType() вместо getType() для правильной таблицы лута
    std::string enemyType = "unknown";
    if (auto enemy = std::dynamic_pointer_cast<Enemy>(victim)) {
        enemyType = enemy->getEnemyType();
    } else {
        enemyType = victim->getType();
    }
    
    int playerLevel = killer->getLevel();

    std::ostringstream oss;
    oss << victim->getName() << " повержен!";
    std::cout << oss.str() << std::endl;

    if (auto enemy = std::dynamic_pointer_cast<Enemy>(victim)) {
        if (auto player = std::dynamic_pointer_cast<Player>(killer)) {
            player->gainExperience(enemy->getLootExperience());
        }
    }

    auto [vx, vy] = victim->getPosition();
    lootSystem.spawnLootOnGround(enemyType, playerLevel, static_cast<float>(vx), static_cast<float>(vy));

    comboSystem.resetCombo();
}

bool CombatSystem::isCombatActive() const {
    return combatActive;
}

void CombatSystem::update(float deltaTime) {
    comboSystem.update(deltaTime);
    lootSystem.update(deltaTime);

    static float combatTimer = 0.0f;
    combatTimer += deltaTime;

    if (combatTimer >= 1.0f && isCombatActive()) {
        processCombatRound();
        combatTimer = 0.0f;
    }

    if (combatEvents.empty() && isCombatActive()) {
        combatActive = false;
    }
}

void CombatSystem::addCombatEvent(std::shared_ptr<CombatEvent> event) {
    if (event) {
        combatEvents.push_back(event);
        combatActive = true;
    }
}

void CombatSystem::removeCombatEvent(const std::shared_ptr<CombatEvent>& event) {
    if (!event) return;
    auto it = std::find(combatEvents.begin(), combatEvents.end(), event);
    if (it != combatEvents.end()) {
        combatEvents.erase(it);
    }
}

const std::vector<std::shared_ptr<CombatEvent>>& CombatSystem::getCombatEvents() const {
    return combatEvents;
}

ComboSystem& CombatSystem::getComboSystem() {
    return comboSystem;
}

LootSystem& CombatSystem::getLootSystem() {
    return lootSystem;
}

void CombatSystem::displayCombatStatus() const {
    std::cout << "\n=== Статус боя ===\n";
    if (comboSystem.isComboActive()) {
        std::cout << "КОМБО x" << comboSystem.getComboCount() << ": "
                  << comboSystem.getCurrentComboName() << "\n";
    }
    for (const auto& event : combatEvents) {
        if (event) {
            std::cout << event->getDescription() << "\n";
        }
    }
    std::cout << "=================\n";
}
