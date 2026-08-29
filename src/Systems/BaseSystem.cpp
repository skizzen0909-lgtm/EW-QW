// Systems/BaseSystem.cpp
#include "BaseSystem.h"
#include <iostream>
#include <sstream>
#include <algorithm>

BaseSystem::BaseSystem()
    : baseName("Лагерь Странника")
    , baseLevel(1) {
    initializeDefaultBuildings();
}

void BaseSystem::registerBuildingType(const BuildingType& type) {
    buildingTypes[type.id] = type;
}

bool BaseSystem::canPlaceBuilding(const std::string& typeId, int x, int y) const {
    auto it = buildingTypes.find(typeId);
    if (it == buildingTypes.end()) return false;

    for (const auto& building : placedBuildings) {
        if (building.tileX == x && building.tileY == y) return false;
    }

    return true;
}

bool BaseSystem::placeBuilding(const std::string& typeId, int x, int y) {
    if (!canPlaceBuilding(typeId, x, y)) return false;

    auto it = buildingTypes.find(typeId);
    if (it == buildingTypes.end()) return false;

    PlacedBuilding building;
    building.typeId = typeId;
    building.name = it->second.name;
    building.tileX = x;
    building.tileY = y;
    building.level = 1;
    building.active = true;

    placedBuildings.push_back(building);
    recalculateBonuses();

    std::ostringstream oss;
    oss << "[BASE] Построено: " << building.name << " на (" << x << ", " << y << ")";
    std::cout << oss.str() << std::endl;

    return true;
}

bool BaseSystem::upgradeBuilding(int index) {
    if (index < 0 || index >= static_cast<int>(placedBuildings.size())) return false;

    auto& building = placedBuildings[index];
    auto typeIt = buildingTypes.find(building.typeId);
    if (typeIt == buildingTypes.end()) return false;
    if (!typeIt->second.isUpgradeable) return false;
    if (building.level >= typeIt->second.maxLevel) return false;

    building.level++;
    recalculateBonuses();

    std::ostringstream oss;
    oss << "[BASE] " << building.name << " улучшена до уровня " << building.level;
    std::cout << oss.str() << std::endl;

    return true;
}

bool BaseSystem::removeBuilding(int index) {
    if (index < 0 || index >= static_cast<int>(placedBuildings.size())) return false;

    std::ostringstream oss;
    oss << "[BASE] Демонтаж: " << placedBuildings[index].name;
    std::cout << oss.str() << std::endl;

    placedBuildings.erase(placedBuildings.begin() + index);
    recalculateBonuses();
    return true;
}

void BaseSystem::recalculateBonuses() {
    globalBonuses = BaseUpgrade();

    for (const auto& building : placedBuildings) {
        if (!building.active) continue;

        int lvl = building.level;
        if (building.typeId == "storage") {
            globalBonuses.storageBonus += 10 * lvl;
        }
        else if (building.typeId == "wall") {
            globalBonuses.defenseBonus += 5 * lvl;
        }
        else if (building.typeId == "farm") {
            globalBonuses.productionBonus += 3 * lvl;
        }
        else if (building.typeId == "tavern") {
            globalBonuses.happinessBonus += 2 * lvl;
        }
        else if (building.typeId == "workshop") {
            globalBonuses.productionBonus += 5 * lvl;
        }
        else if (building.typeId == "watchtower") {
            globalBonuses.defenseBonus += 8 * lvl;
        }
    }
}

const std::vector<PlacedBuilding>& BaseSystem::getPlacedBuildings() const {
    return placedBuildings;
}

const BuildingType* BaseSystem::getBuildingType(const std::string& typeId) const {
    auto it = buildingTypes.find(typeId);
    return (it != buildingTypes.end()) ? &it->second : nullptr;
}

const BaseUpgrade& BaseSystem::getGlobalBonuses() const {
    return globalBonuses;
}

std::string BaseSystem::getBaseName() const {
    return baseName;
}

int BaseSystem::getBaseLevel() const {
    return baseLevel;
}

void BaseSystem::setBaseName(const std::string& name) {
    baseName = name;
}

void BaseSystem::initializeDefaultBuildings() {
    BuildingType storage;
    storage.id = "storage";
    storage.name = "Хранилище";
    storage.description = "Увеличивает вместимость инвентаря";
    storage.width = 2;
    storage.height = 2;
    storage.buildCost = 50;
    storage.upgradeCost = 75;
    storage.requiredResource = "wood";
    storage.requiredQuantity = 10;
    storage.isUpgradeable = true;
    storage.maxLevel = 5;
    registerBuildingType(storage);

    BuildingType wall;
    wall.id = "wall";
    wall.name = "Стена";
    wall.description = "Увеличивает оборону базы";
    wall.width = 1;
    wall.height = 1;
    wall.buildCost = 30;
    wall.upgradeCost = 45;
    wall.requiredResource = "stone";
    wall.requiredQuantity = 5;
    wall.isUpgradeable = true;
    wall.maxLevel = 5;
    registerBuildingType(wall);

    BuildingType farm;
    farm.id = "farm";
    farm.name = "Ферма";
    farm.description = "Производит еду и ресурсы";
    farm.width = 2;
    farm.height = 2;
    farm.buildCost = 40;
    farm.upgradeCost = 60;
    farm.requiredResource = "wood";
    farm.requiredQuantity = 8;
    farm.isUpgradeable = true;
    farm.maxLevel = 3;
    registerBuildingType(farm);

    BuildingType tavern;
    tavern.id = "tavern";
    tavern.name = "Таверна";
    tavern.description = "Повышает счастье жителей";
    tavern.width = 2;
    tavern.height = 2;
    tavern.buildCost = 80;
    tavern.upgradeCost = 120;
    tavern.requiredResource = "wood";
    tavern.requiredQuantity = 15;
    tavern.isUpgradeable = true;
    tavern.maxLevel = 3;
    registerBuildingType(tavern);

    BuildingType workshop;
    workshop.id = "workshop";
    workshop.name = "Мастерская";
    workshop.description = "Улучшает производство и крафт";
    workshop.width = 2;
    workshop.height = 2;
    workshop.buildCost = 100;
    workshop.upgradeCost = 150;
    workshop.requiredResource = "iron_ingot";
    workshop.requiredQuantity = 5;
    workshop.isUpgradeable = true;
    workshop.maxLevel = 3;
    registerBuildingType(workshop);

    BuildingType watchtower;
    watchtower.id = "watchtower";
    watchtower.name = "Сторожевая башня";
    watchtower.description = "Увеличивает обзор и оборону";
    watchtower.width = 1;
    watchtower.height = 1;
    watchtower.buildCost = 60;
    watchtower.upgradeCost = 90;
    watchtower.requiredResource = "stone";
    watchtower.requiredQuantity = 10;
    watchtower.isUpgradeable = true;
    watchtower.maxLevel = 3;
    registerBuildingType(watchtower);
}
