// Quests/WorldImpactQuest.cpp
#include "WorldImpactQuest.h"
#include "World/TileMap.h"
#include "World/Location.h"
#include "Entities/Entity.h"
#include <iostream>
#include <sstream>

WorldImpactQuest::WorldImpactQuest(const std::string& questId, const std::string& title,
                                   const std::string& description, QuestType type)
    : Quest(questId, title, description, type)
    , impactApplied(false) {
}

void WorldImpactQuest::addImpact(const WorldImpact& impact) {
    impacts.push_back(impact);
}

const std::vector<WorldImpact>& WorldImpactQuest::getImpacts() const {
    return impacts;
}

bool WorldImpactQuest::isImpactApplied() const {
    return impactApplied;
}

bool WorldImpactQuest::checkCompletionConditions(Entity* player) {
    if (completed) return false;
    return !impacts.empty();
}

void WorldImpactQuest::complete(Entity* player) {
    if (completed) return;
    completed = true;
    std::ostringstream oss;
    oss << "[QUEST] Квест завершён: " << title << "! Миp изменится...";
    std::cout << oss.str() << std::endl;
}

std::string WorldImpactQuest::getProgressString() const {
    if (completed) return "Завершён (влияние применено)";
    return "Выполняется (награда: изменение мира)";
}

void WorldImpactQuest::applyWorldImpacts(TileMap* tileMap, Location* location) {
    if (impactApplied || !tileMap) return;

    for (const auto& impact : impacts) {
        switch (impact.type) {
        case WorldImpactType::PLACE_TILE:
            if (tileMap->isValidPosition(impact.targetX, impact.targetY)) {
                tileMap->setTile(impact.targetX, impact.targetY, impact.tileTo);
                std::cout << "[WORLD] Тайл установлен: (" << impact.targetX
                    << ", " << impact.targetY << ")" << std::endl;
            }
            break;

        case WorldImpactType::REMOVE_TILE:
            if (tileMap->isValidPosition(impact.targetX, impact.targetY)) {
                tileMap->setTile(impact.targetX, impact.targetY, impact.tileFrom);
                std::cout << "[WORLD] Тайл удалён: (" << impact.targetX
                    << ", " << impact.targetY << ")" << std::endl;
            }
            break;

        case WorldImpactType::CHANGE_TILE:
            if (tileMap->isValidPosition(impact.targetX, impact.targetY)) {
                TileType current = tileMap->getTile(impact.targetX, impact.targetY);
                if (current == impact.tileFrom) {
                    tileMap->setTile(impact.targetX, impact.targetY, impact.tileTo);
                    std::cout << "[WORLD] Тайл изменён: (" << impact.targetX
                        << ", " << impact.targetY << ")" << std::endl;
                }
            }
            break;

        case WorldImpactType::SPAWN_ENTITY:
            std::cout << "[WORLD] Спавн сущности: " << impact.entityId
                << " на (" << impact.targetX << ", " << impact.targetY << ")" << std::endl;
            break;

        case WorldImpactType::REMOVE_ENTITY:
            std::cout << "[WORLD] Удаление сущности: " << impact.entityId << std::endl;
            break;

        case WorldImpactType::UNLOCK_LOCATION:
            std::cout << "[WORLD] Открыта новая локация: " << impact.locationId << std::endl;
            break;

        case WorldImpactType::CHANGE_WEATHER:
            std::cout << "[WORLD] Погода изменена: " << impact.intValue << std::endl;
            break;

        case WorldImpactType::MODIFY_NPC_RELATION:
            std::cout << "[WORLD] Отношения NPC изменены: " << impact.entityId
                << " на " << impact.intValue << std::endl;
            break;
        }
    }

    impactApplied = true;
}
