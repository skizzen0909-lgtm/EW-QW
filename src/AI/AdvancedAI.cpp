// src/AI/AdvancedAI.cpp
#include "AI/AdvancedAI.h"
#include "Entities/Entity.h"
#include "Entities/Player.h"
#include "Entities/Enemy.h"
#include "World/TileBasedLocation.h"
#include "World/TileMap.h"
#include "Events/EventManager.h"
#include "Events/CombatEvent.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <climits>
#include <memory>

AdvancedAI::AdvancedAI(const AIPersonality& personality)
    : personality(personality)
    , currentState(AIState::IDLE)
    , currentThreatLevel(0.0f)
    , currentOpportunityLevel(0.0f)
    , perceivedDangerRadius(10.0f)
    , gen(std::random_device{}())
    , dist(-1, 1)
    , goalSystem(std::make_unique<GoalSystem>())
    , learningSystem(0.3f, 0.01f, 50) {
    emotionalSystem.setBaseMood(
        personality.aggression * 0.3f -
        personality.caution * 0.2f +
        (1.0f - personality.curiosity) * 0.1f
    );
}

// === Геттеры ===
const AIPersonality& AdvancedAI::getPersonality() const {
    return personality;
}

AIState AdvancedAI::getCurrentState() const {
    return currentState;
}

MemorySystem& AdvancedAI::getMemorySystem() {
    return memorySystem;
}

const MemorySystem& AdvancedAI::getMemorySystem() const {
    return memorySystem;
}

EmotionalSystem& AdvancedAI::getEmotionalSystem() {
    return emotionalSystem;
}

const EmotionalSystem& AdvancedAI::getEmotionalSystem() const {
    return emotionalSystem;
}

// 🔑 ДОБАВЛЕНО: геттер для GoalSystem
GoalSystem* AdvancedAI::getGoalSystem() {
    return goalSystem.get();
}

// === Геттеры для LearningSystem ===
LearningSystem& AdvancedAI::getLearningSystem() {
    return learningSystem;
}

const LearningSystem& AdvancedAI::getLearningSystem() const {
    return learningSystem;
}

// === Сеттеры ===
void AdvancedAI::setPersonality(const AIPersonality& newPersonality) {
    personality = newPersonality;
}

void AdvancedAI::setFollowTarget(const std::shared_ptr<Entity>& target) {
    followTarget = target;
}

void AdvancedAI::setFleeTarget(const std::shared_ptr<Entity>& target) {
    fleeTarget = target;
}

void AdvancedAI::setCurrentTarget(const std::shared_ptr<Entity>& target) {
    currentTarget = target;
}

void AdvancedAI::setState(AIState newState) {
    if (currentState != newState) {
        currentState = newState;
    }
}

// === Вспомогательные методы поиска ===
std::shared_ptr<Entity> AdvancedAI::findHostileTarget() const {
    for (const auto& memory : memorySystem.getMemories()) {
        if ((memory.eventType == "attacked_by" || memory.eventType == "seen") && memory.entity.lock()) {
            auto entity = memory.entity.lock();
            if (!entity || !entity->isAlive()) continue;

            // Проверяем тип: враг или игрок
            if (entity->getType() == "enemy" || entity->getType() == "player") {
                return entity;
            }
        }
    }
    return nullptr;
}

std::shared_ptr<Entity> AdvancedAI::findFriendlyTarget() const {
    for (const auto& memory : memorySystem.getMemories()) {
        if ((memory.eventType == "seen" || memory.eventType == "socialized_with") && memory.entity.lock()) {
            auto entity = memory.entity.lock();
            if (entity && entity->isAlive()) {
                if (entity->getType() == "npc" || entity->getType() == "villager" || entity->getType() == "player") {
                    return entity;
                }
            }
        }
    }
    return nullptr;
}

// === Дерево решений ===
void AdvancedAI::assessThreats() {
    currentThreatLevel = 0.0f;

    for (const auto& memory : memorySystem.getMemories()) {
        if (memory.entity.expired()) continue;
        auto entity = memory.entity.lock();
        if (!entity || !entity->isAlive()) continue;

        if (memory.eventType == "attacked_by") {
            currentThreatLevel += memory.importance * 2.0f;
        }
        else if (memory.eventType == "seen" &&
            (entity->getType() == "enemy")) {
            auto [px, py] = entity->getPosition();
            if (px != -1 && py != -1) {
                currentThreatLevel += memory.importance * 0.5f;
            }
        }
    }

    currentThreatLevel = std::clamp(currentThreatLevel, 0.0f, 1.0f);
}

void AdvancedAI::assessOpportunities() {
    currentOpportunityLevel = 0.0f;

    for (const auto& memory : memorySystem.getMemories()) {
        if (memory.entity.expired()) continue;
        auto entity = memory.entity.lock();
        if (!entity || !entity->isAlive()) continue;

        if (memory.eventType == "seen" || memory.eventType == "socialized_with") {
            if (entity->getType() == "npc" || entity->getType() == "villager") {
                currentOpportunityLevel += memory.importance * 0.5f;
            }
        }
    }

    currentOpportunityLevel = std::clamp(currentOpportunityLevel, 0.0f, 1.0f);
}

// === Обучение и адаптация ===
void AdvancedAI::learnFromAction(const std::string& situation, const std::string& action, float outcome) {
    learningSystem.addExperience(situation, action, outcome);
    
    // Вывод для отладки
    std::cout << "[Learning] " << situation << " -> " << action 
              << " (outcome: " << outcome << ")" << std::endl;
}

void AdvancedAI::adaptToSituation(Entity* entity) {
    if (!entity) return;
    
    // Определяем текущую ситуацию
    std::string situationType;
    
    if (memorySystem.hasHostileEntities() || memorySystem.wasRecentlyAttacked()) {
        situationType = "combat";
    } else if (memorySystem.hasFriendlyEntities()) {
        situationType = "social";
    } else {
        situationType = "exploration";
    }
    
    // Получаем лучшее действие из обучения
    std::string bestAction = learningSystem.getBestAction(situationType);
    
    if (!bestAction.empty()) {
        // Применяем обученное поведение
        std::cout << "[Adaptation] " << entity->getName() 
                  << " использует опыт для ситуации '" << situationType 
                  << "': " << bestAction << std::endl;
        
        // Здесь можно изменить состояние или параметры на основе лучшего действия
        if (situationType == "combat" && bestAction == "aggressive_attack") {
            personality.aggression = std::min(1.0f, personality.aggression + 0.1f);
        } else if (situationType == "combat" && bestAction == "defensive_retreat") {
            personality.caution = std::min(1.0f, personality.caution + 0.1f);
        }
    }
}

void AdvancedAI::makeDecision(Entity* entity) {
    if (!entity) return;

    assessThreats();
    assessOpportunities();

    float mood = emotionalSystem.getMood();
    float aggression = personality.aggression;
    float caution = personality.caution;
    float curiosity = personality.curiosity;
    float sociability = personality.sociability;

    // Проверяем обученное поведение для текущей ситуации
    std::string situationType;
    if (currentThreatLevel > 0.3f || memorySystem.wasRecentlyAttacked()) {
        situationType = "combat";
    } else if (currentOpportunityLevel > 0.2f) {
        situationType = "social";
    } else {
        situationType = "exploration";
    }

    std::string learnedAction = learningSystem.getBestAction(situationType);
    if (!learnedAction.empty() && learningSystem.getSuccessRate(situationType) > 0.6f) {
        if (learnedAction == "aggressive_attack" && aggression > 0.5f) {
            auto hostile = findHostileTarget();
            if (hostile) {
                setCurrentTarget(hostile);
                setState(AIState::ATTACK);
                return;
            }
        }
        else if (learnedAction == "defensive_retreat" || learnedAction == "avoid_conflict") {
            if (currentThreatLevel > 0.5f) {
                setState(AIState::FLEE);
                return;
            }
        }
        else if (learnedAction == "initiate_conversation" || learnedAction == "approach_friendly") {
            setState(AIState::SOCIALIZE);
            return;
        }
    }

    // 1. Угрозы (по уровню угрозы из assessThreats)
    if (currentThreatLevel > 0.3f || memorySystem.wasRecentlyAttacked()) {
        if (mood < -0.3f || caution > 0.6f) {
            setState(AIState::FLEE);
            return;
        }
        else if (aggression > 0.7f) {
            auto hostile = findHostileTarget();
            if (hostile) {
                setCurrentTarget(hostile);
                setState(AIState::ATTACK);
                return;
            }
        }
    }

    // 2. Дружелюбные цели
    if (currentOpportunityLevel > 0.2f && sociability > 0.6f) {
        auto friendly = findFriendlyTarget();
        if (friendly) {
            setFollowTarget(friendly);
            setState(AIState::FOLLOW);
            return;
        }
    }

    // 3. Социализация
    if (memorySystem.wasRecentlySocialized() && sociability > 0.5f) {
        setState(AIState::SOCIALIZE);
        return;
    }

    // 4. Любопытство
    if (curiosity > 0.7f) {
        setState(AIState::WANDER);
        return;
    }

    // 5. Поведение по умолчанию
    if (entity->getType() == "player") {
        setState(AIState::WANDER);
    }
    else {
        setState(AIState::IDLE);
    }
}

// === Логика движения ===
std::pair<int, int> AdvancedAI::wanderLogic(Entity* entity) {
    if (!entity) return { -1, -1 };

    auto [posX, posY] = entity->getPosition();
    if (posX == -1 || posY == -1) return { -1, -1 };

    TileBasedLocation* tileLoc = dynamic_cast<TileBasedLocation*>(entity->getCurrentLocation());
    if (!tileLoc || !tileLoc->getTileMap()) return { -1, -1 };

    // 1. Сначала пробуем исследовать границы мира
    int worldSize = tileLoc->getTileMap()->getWidth();
    int viewRadius = 200;
    bool nearEdge = (posX < viewRadius || posX > worldSize - viewRadius ||
        posY < viewRadius || posY > worldSize - viewRadius);

    int searchRadius = nearEdge ? 10 : 3; // у края — дальше ищем, в центре — ближе

    // 2. Ищем проходимый тайл в радиусе
    auto target = tileLoc->findNearbyWalkableTile(posX, posY, searchRadius);
    if (target.first != -1 && target.second != -1) {
        return target;
    }

    // 3. Если не нашли — fallback на старое поведение
    int newX = posX + dist(gen);
    int newY = posY + dist(gen);
    newX = std::clamp(newX, 0, worldSize - 1);
    newY = std::clamp(newY, 0, worldSize - 1);
    return { newX, newY };
}

std::pair<int, int> AdvancedAI::followLogic(Entity* entity) {
    auto target = followTarget.lock();
    if (!target) {
        setState(AIState::IDLE);
        return { -1, -1 };
    }
    auto entityPos = entity->getPosition();
    auto targetPos = target->getPosition();
    int dx = targetPos.first - entityPos.first;
    int dy = targetPos.second - entityPos.second;
    int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
    return { entityPos.first + stepX, entityPos.second + stepY };
}

std::pair<int, int> AdvancedAI::fleeLogic(Entity* entity) {
    auto target = fleeTarget.lock();
    if (!target) {
        setState(AIState::IDLE);
        return { -1, -1 };
    }
    auto entityPos = entity->getPosition();
    auto targetPos = target->getPosition();
    int dx = entityPos.first - targetPos.first;
    int dy = entityPos.second - targetPos.second;
    int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
    return { entityPos.first + stepX, entityPos.second + stepY };
}

std::pair<int, int> AdvancedAI::attackMovementLogic(Entity* entity) {
    auto target = currentTarget.lock();
    if (!target || !target->isAlive()) {
        setState(AIState::IDLE);
        return { -1, -1 };
    }
    auto entityPos = entity->getPosition();
    auto targetPos = target->getPosition();
    int distance = std::abs(entityPos.first - targetPos.first) + std::abs(entityPos.second - targetPos.second);
    if (distance <= 1) {
        return { -1, -1 };
    }
    else {
        return followLogic(entity);
    }
}
void AdvancedAI::checkForLoot(Entity* entity) {
    // Заглушка: в будущем здесь будет проверка тайла на наличие предметов
    // Пока просто логируем
    if (entity) {
        // Пример: 2% шанс найти что-то при блуждании
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> chance(0.0, 1.0);
        if (chance(gen) < 0.02f) {
            std::cout << entity->getName() << " находит что-то интересное на земле!" << std::endl;
            // Позже: вызов системы лута, например: LootSystem::pickupNearby(entity);
        }
    }
}

// === Выполнение действий ===
void AdvancedAI::executeWander(Entity* entity) {
    if (!entity) return;
    // Побочные эффекты блуждания
    checkForLoot(entity);

    // Можно добавить шанс найти ресурс, триггер события и т.д.
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> chance(0.0, 1.0);

    if (chance(gen) < 0.01f) { // 1% шанс на событие
        std::cout << entity->getName() << " замечает что-то интересное поблизости!" << std::endl;
    }
}
void AdvancedAI::executeFollow(Entity* entity) {}
void AdvancedAI::executeSocialize(Entity* entity) {
    if (!entity) return;

    // Если это NPC — используем социализацию NPC-to-NPC
    if (entity->getType() == "npc" || entity->getType() == "villager") {
        executeNPCSocialize(entity);
        return;
    }

    // 1. Ищем ближайшего NPC в радиусе 5 клеток
    auto [posX, posY] = entity->getPosition();
    if (posX == -1 || posY == -1) return;

    TileBasedLocation* tileLoc = dynamic_cast<TileBasedLocation*>(entity->getCurrentLocation());
    if (!tileLoc) return;

    const auto& nearby = tileLoc->getEntitiesInRange(posX, posY, 5);
    std::shared_ptr<Entity> nearestNPC = nullptr;
    int minDist = INT_MAX;

    for (const auto& other : nearby) {
        if (!other || other == entity->shared_from_this()) continue;
        if (other->getType() == "npc" || other->getType() == "villager") {
            auto [ox, oy] = other->getPosition();
            int dist = std::abs(posX - ox) + std::abs(posY - oy);
            if (dist < minDist) {
                minDist = dist;
                nearestNPC = other;
            }
        }
    }

    // 2. Если NPC найден — инициируем диалог
    if (nearestNPC) {
        std::cout << entity->getName() << " подходит к " << nearestNPC->getName() << " для разговора." << std::endl;
        memorySystem.addMemory("socialized_with", nearestNPC, 0.8f);
        emotionalSystem.addMoodModifier(0.1f);
        learnFromAction("social_interaction", "initiate_conversation", 0.7f);
    }
    else {
        std::cout << entity->getName() << " ищет кого-нибудь для общения..." << std::endl;
        setState(AIState::WANDER);
        learnFromAction("social_interaction", "search_for_company", 0.2f);
    }
}

void AdvancedAI::executeNPCSocialize(Entity* entity) {
    if (!entity) return;

    auto [posX, posY] = entity->getPosition();
    if (posX == -1 || posY == -1) return;

    TileBasedLocation* tileLoc = dynamic_cast<TileBasedLocation*>(entity->getCurrentLocation());
    if (!tileLoc) return;

    // Ищем других NPC в радиусе 3 клеток
    const auto& nearby = tileLoc->getEntitiesInRange(posX, posY, 3);
    std::shared_ptr<Entity> nearestNPC = nullptr;
    int minDist = INT_MAX;

    for (const auto& other : nearby) {
        if (!other || other.get() == entity) continue;
        if (other->getType() == "npc" || other->getType() == "villager") {
            auto [ox, oy] = other->getPosition();
            int d = std::abs(posX - ox) + std::abs(posY - oy);
            if (d < minDist) {
                minDist = d;
                nearestNPC = other;
            }
        }
    }

    if (nearestNPC) {
        std::cout << entity->getName() << " обменивается новостями с " << nearestNPC->getName() << "." << std::endl;

        // Обмен информацией: делимся воспоминаниями
        memorySystem.addMemory("socialized_with", nearestNPC, 0.6f);
        emotionalSystem.addMoodModifier(0.15f);

        // NPC может передать информацию о врагах или возможностях
        if (auto otherAI = nearestNPC->getAI()) {
            if (auto* otherAdvanced = dynamic_cast<AdvancedAI*>(otherAI)) {
                const auto& otherMemories = otherAdvanced->getMemorySystem().getMemories();
                for (const auto& mem : otherMemories) {
                    if (mem.entity.expired()) continue;
                    auto sharedMem = mem.entity.lock();
                    if (sharedMem && (mem.eventType == "seen" || mem.eventType == "attacked_by")) {
                        // Добавляем в свою память с уменьшенной важностью
                        memorySystem.addMemory("told_about", sharedMem, mem.importance * 0.5f);
                    }
                }
            }
        }

        learnFromAction("npc_social", "exchange_information", 0.6f);
    }
    else {
        setState(AIState::WANDER);
        learnFromAction("npc_social", "wander_to_find", 0.2f);
    }
}
void AdvancedAI::executeFlee(Entity* entity) {
    if (!entity) return;

    auto fleeFrom = fleeTarget.lock();
    if (!fleeFrom || !fleeFrom->isAlive()) {
        setState(AIState::IDLE);
        learnFromAction("flee", "successfully_escaped", 0.6f);
        return;
    }

    auto [px, py] = entity->getPosition();
    auto [tx, ty] = fleeFrom->getPosition();
    int distance = std::abs(px - tx) + std::abs(py - ty);

    // Если убежали достаточно далеко — останавливаемся
    if (distance > 15) {
        setState(AIState::IDLE);
        emotionalSystem.addMoodModifier(0.2f);
        learnFromAction("flee", "successfully_escaped", 0.8f);
    }
    else {
        learnFromAction("flee", "fleeing", 0.3f);
    }
}
void AdvancedAI::executeAttack(Entity* entity) {
    auto target = currentTarget.lock();
    if (!target || !target->isAlive()) {
        setState(AIState::IDLE);
        learnFromAction("combat_attack", "target_defeated", 0.8f);
        return;
    }
    auto entityPos = entity->getPosition();
    auto targetPos = target->getPosition();
    int distance = std::abs(entityPos.first - targetPos.first) + std::abs(entityPos.second - targetPos.second);
    if (distance <= 1) {
        int damage = 5 + static_cast<int>(personality.aggression * 10);
        auto combatEvent = std::make_shared<CombatEvent>(
            entity->shared_from_this(),
            target,
            damage
        );
        EventManager::getInstance().emit(combatEvent);

        // Обучение: оцениваем результат по здоровью цели
        float healthRatio = static_cast<float>(target->getHealth()) / static_cast<float>(target->getMaxHealth());
        float outcome = target->isAlive() ? (1.0f - healthRatio) * 0.5f : 0.9f;

        // Если мы тоже получили урон — учитываем это
        float ownHealthRatio = static_cast<float>(entity->getHealth()) / static_cast<float>(entity->getMaxHealth());
        if (ownHealthRatio < 0.3f) {
            outcome -= 0.3f; // Бой был опасен
        }

        learnFromAction("combat_attack", "direct_attack", outcome);
    }
    else {
        learnFromAction("combat_attack", "closing_distance", 0.3f);
    }
}
void AdvancedAI::executeIdle(Entity* entity) {}
void AdvancedAI::executeAlert(Entity* entity) {
    std::cout << entity->getName() << " насторожен!" << std::endl;
}

// === Основной метод обновления ===
void AdvancedAI::update(float deltaTime, Entity* entity) {
    if (!entity) return;

    emotionalSystem.update(deltaTime);
    memorySystem.decayMemories(deltaTime);

    // 1. Сначала обрабатываем цели — они могут установить состояние
    if (goalSystem) {
        goalSystem->update(entity, deltaTime);
    }

    // 2. Адаптация к ситуации на основе обучения
    adaptToSituation(entity);

    // 3. Только если состояние НЕ установлено целями — принимаем решение
    if (currentState == AIState::IDLE || currentState == AIState::WANDER) {
        makeDecision(entity);
    }

    // 4. Планируем движение
    std::pair<int, int> newPosition = decideMovement(entity);
    if (newPosition.first != -1 && newPosition.second != -1 &&
        (newPosition.first != entity->getPosition().first || newPosition.second != entity->getPosition().second)) {
        entity->plannedMoveX = newPosition.first;
        entity->plannedMoveY = newPosition.second;
        entity->hasPlannedMove = true;
    }
    switch (currentState) {
    case AIState::WANDER:   executeWander(entity); break;
    case AIState::FOLLOW:   executeFollow(entity); break;
    case AIState::FLEE:     executeFlee(entity); break;
    case AIState::ATTACK:   executeAttack(entity); break;
    case AIState::SOCIALIZE: executeSocialize(entity); break;
    case AIState::IDLE:
    case AIState::ALERT:
    default: break;
    }
}

// === Реакция на встречу ===
void AdvancedAI::onEntityEncounter(Entity* self, const std::shared_ptr<Entity>& other) {
    if (!self || !other) return;

    memorySystem.addMemory("seen", other, 0.5f);

    if (dynamic_cast<Enemy*>(other.get()) != nullptr) {
        emotionalSystem.addMoodModifier(-0.2f);
        memorySystem.setLastAttackedTime();
        setCurrentTarget(other);
        
        // Записываем опыт встречи с врагом
        learnFromAction("combat_encounter", "prepare_defense", 0.3f);
    }
    else if (dynamic_cast<Player*>(other.get()) != nullptr) {
        if (emotionalSystem.getMood() < -0.3f) {
            setFleeTarget(other);
            learnFromAction("social_encounter", "avoid_conflict", 0.5f);
        }
        else if (personality.sociability > 0.7f) {
            setFollowTarget(other);
            memorySystem.setLastSocializedTime();
            learnFromAction("social_encounter", "approach_friendly", 0.7f);
        }
    }
}

// === Принятие решения о движении ===
std::pair<int, int> AdvancedAI::decideMovement(Entity* entity) {
    switch (currentState) {
    case AIState::WANDER:   return wanderLogic(entity);
    case AIState::FOLLOW:   return followLogic(entity);
    case AIState::FLEE:     return fleeLogic(entity);
    case AIState::ATTACK:   return attackMovementLogic(entity);
    case AIState::IDLE:
    case AIState::SOCIALIZE:
    case AIState::ALERT:
    default:                return { -1, -1 };
    }
}