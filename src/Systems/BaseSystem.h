// Systems/BaseSystem.h
#ifndef BASESYSTEM_H
#define BASESYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include "Types/Types.h"

struct BuildingType {
    std::string id;
    std::string name;
    std::string description;
    int width, height;
    int buildCost;
    int upgradeCost;
    std::string requiredResource;
    int requiredQuantity;
    bool isUpgradeable;
    int maxLevel;

    BuildingType()
        : width(1), height(1), buildCost(0), upgradeCost(0),
          requiredQuantity(0), isUpgradeable(false), maxLevel(3) {}
};

struct PlacedBuilding {
    std::string typeId;
    std::string name;
    int tileX, tileY;
    int level;
    bool active;

    PlacedBuilding() : tileX(0), tileY(0), level(1), active(true) {}
};

struct BaseUpgrade {
    int storageBonus;
    int defenseBonus;
    int productionBonus;
    int happinessBonus;

    BaseUpgrade() : storageBonus(0), defenseBonus(0), productionBonus(0), happinessBonus(0) {}
};

class BaseSystem {
private:
    std::unordered_map<std::string, BuildingType> buildingTypes;
    std::vector<PlacedBuilding> placedBuildings;
    BaseUpgrade globalBonuses;
    std::string baseName;
    int baseLevel;

    void recalculateBonuses();

public:
    BaseSystem();
    ~BaseSystem() = default;

    void registerBuildingType(const BuildingType& type);
    bool canPlaceBuilding(const std::string& typeId, int x, int y) const;
    bool placeBuilding(const std::string& typeId, int x, int y);
    bool upgradeBuilding(int index);
    bool removeBuilding(int index);

    const std::vector<PlacedBuilding>& getPlacedBuildings() const;
    const BuildingType* getBuildingType(const std::string& typeId) const;
    const BaseUpgrade& getGlobalBonuses() const;

    std::string getBaseName() const;
    int getBaseLevel() const;
    void setBaseName(const std::string& name);

    void initializeDefaultBuildings();

    BaseSystem(const BaseSystem&) = delete;
    BaseSystem& operator=(const BaseSystem&) = delete;
};

#endif // BASESYSTEM_H
