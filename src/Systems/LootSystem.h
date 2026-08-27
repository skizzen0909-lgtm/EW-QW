// Systems/LootSystem.h
#ifndef LOOTSYSTEM_H
#define LOOTSYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <random>

class Entity;
class Item;

struct LootDrop {
    std::string itemId;
    std::string itemName;
    float dropChance;
    int minQuantity;
    int maxQuantity;
    int rarity; // 0=common, 1=uncommon, 2=rare, 3=epic, 4=legendary
};

struct LootTable {
    std::string id;
    std::string enemyType;
    int playerLevel;
    std::vector<LootDrop> drops;
    int experienceReward;
    int goldReward;
};

struct DroppedItem {
    std::string itemId;
    std::string itemName;
    int quantity;
    float worldX, worldY;
    bool collected;
};

class LootSystem {
private:
    std::unordered_map<std::string, LootTable> lootTables;
    std::vector<DroppedItem> groundItems;
    std::mt19937 rng;

    float calculateDropChance(float baseChance, int playerLevel, int rarity) const;

public:
    LootSystem();
    ~LootSystem() = default;

    void registerLootTable(const LootTable& table);
    void unregisterLootTable(const std::string& tableId);

    std::vector<std::pair<std::string, int>> generateLoot(const std::string& enemyType, int playerLevel);
    void spawnLootOnGround(const std::string& enemyType, int playerLevel, float x, float y);

    bool collectNearbyLoot(Entity* collector, float radius = 2.0f);
    const std::vector<DroppedItem>& getGroundItems() const;
    void removeCollectedItems();

    void update(float deltaTime);

    void initializeDefaultLootTables();

    LootSystem(const LootSystem&) = delete;
    LootSystem& operator=(const LootSystem&) = delete;
};

#endif // LOOTSYSTEM_H
