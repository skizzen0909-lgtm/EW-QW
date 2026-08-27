// Systems/LootSystem.cpp
#include "LootSystem.h"
#include "Entities/Entity.h"
#include "Items/Item.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cmath>

LootSystem::LootSystem()
    : rng(std::random_device{}()) {
    initializeDefaultLootTables();
}

float LootSystem::calculateDropChance(float baseChance, int playerLevel, int rarity) const {
    float levelBonus = playerLevel * 0.005f;
    float rarityMultiplier = 1.0f / (1.0f + rarity * 0.5f);
    return std::min(0.95f, baseChance * rarityMultiplier * (1.0f + levelBonus));
}

void LootSystem::registerLootTable(const LootTable& table) {
    lootTables[table.id] = table;
}

void LootSystem::unregisterLootTable(const std::string& tableId) {
    lootTables.erase(tableId);
}

std::vector<std::pair<std::string, int>> LootSystem::generateLoot(const std::string& enemyType, int playerLevel) {
    std::vector<std::pair<std::string, int>> result;

    // Ищем подходящую таблицу лута
    for (const auto& [id, table] : lootTables) {
        if (table.enemyType == enemyType) {
            for (const auto& drop : table.drops) {
                float adjustedChance = calculateDropChance(drop.dropChance, playerLevel, drop.rarity);
                std::uniform_real_distribution<> dist(0.0, 1.0);

                if (dist(rng) < adjustedChance) {
                    int quantity = drop.minQuantity;
                    if (drop.maxQuantity > drop.minQuantity) {
                        std::uniform_int_distribution<> qDist(drop.minQuantity, drop.maxQuantity);
                        quantity = qDist(rng);
                    }
                    if (quantity > 0) {
                        result.emplace_back(drop.itemId, quantity);

                        std::ostringstream oss;
                        oss << "[LOOT] Получен предмет: " << drop.itemName
                            << " x" << quantity;
                        if (drop.rarity > 0) {
                            const char* rarityNames[] = {"", "[Редкий]", "[Эпический]", "[Легендарный]"};
                            if (drop.rarity > 0 && drop.rarity < 4) {
                                oss << " " << rarityNames[drop.rarity];
                            }
                        }
                        std::cout << oss.str() << std::endl;
                    }
                }
            }
        }
    }

    return result;
}

void LootSystem::spawnLootOnGround(const std::string& enemyType, int playerLevel, float x, float y) {
    auto loot = generateLoot(enemyType, playerLevel);

    for (const auto& [itemId, quantity] : loot) {
        DroppedItem item;
        item.itemId = itemId;
        item.itemName = itemId;
        item.quantity = quantity;
        item.worldX = x + std::uniform_real_distribution<float>(-0.5f, 0.5f)(rng);
        item.worldY = y + std::uniform_real_distribution<float>(-0.5f, 0.5f)(rng);
        item.collected = false;
        groundItems.push_back(item);
    }
}

bool LootSystem::collectNearbyLoot(Entity* collector, float radius) {
    if (!collector) return false;

    auto [px, py] = collector->getPosition();
    bool collectedAny = false;

    for (auto& item : groundItems) {
        if (item.collected) continue;

        float dx = item.worldX - static_cast<float>(px);
        float dy = item.worldY - static_cast<float>(py);
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= radius) {
            item.collected = true;
            collectedAny = true;

            auto newItem = std::make_shared<Item>(
                item.itemId, item.itemName, "Подобрано с земли",
                ItemType::MISC, ItemRarity::COMMON, 1);
            collector->addItem(newItem);

            std::cout << collector->getName() << " подбирает " << item.itemName
                << " x" << item.quantity << std::endl;
        }
    }

    if (collectedAny) {
        removeCollectedItems();
    }

    return collectedAny;
}

const std::vector<DroppedItem>& LootSystem::getGroundItems() const {
    return groundItems;
}

void LootSystem::removeCollectedItems() {
    groundItems.erase(
        std::remove_if(groundItems.begin(), groundItems.end(),
            [](const DroppedItem& item) { return item.collected; }),
        groundItems.end());
}

void LootSystem::update(float deltaTime) {
    // Периодическая очистка старых предметов на земле (30 секунд)
    static float cleanupTimer = 0.0f;
    cleanupTimer += deltaTime;
    if (cleanupTimer >= 30.0f) {
        cleanupTimer = 0.0f;
        removeCollectedItems();
    }
}

void LootSystem::initializeDefaultLootTables() {
    // Лут от волков
    LootTable wolfLoot;
    wolfLoot.id = "wolf_loot";
    wolfLoot.enemyType = "wolf";
    wolfLoot.playerLevel = 1;
    wolfLoot.drops = {
        {"wolf_fang", "Волчий клык", 0.4f, 1, 2, 0},
        {"wolf_pelt", "Волчья шкура", 0.3f, 1, 1, 1},
        {"raw_meat", "Сырое мясо", 0.6f, 1, 3, 0},
        {"minor_health_potion", "Малое зелье здоровья", 0.15f, 1, 1, 0}
    };
    wolfLoot.experienceReward = 20;
    wolfLoot.goldReward = 5;
    registerLootTable(wolfLoot);

    // Лут от медведей
    LootTable bearLoot;
    bearLoot.id = "bear_loot";
    bearLoot.enemyType = "bear";
    bearLoot.playerLevel = 5;
    bearLoot.drops = {
        {"bear_claw", "Медвежий коготь", 0.35f, 1, 2, 1},
        {"bear_pelt", "Медвежья шкура", 0.25f, 1, 1, 2},
        {"raw_meat", "Сырое мясо", 0.5f, 2, 5, 0},
        {"health_potion", "Зелье здоровья", 0.2f, 1, 2, 0},
        {"ancient_bone", "Древняя кость", 0.05f, 1, 1, 3}
    };
    bearLoot.experienceReward = 50;
    bearLoot.goldReward = 15;
    registerLootTable(bearLoot);

    // Лут от диких созданий (общий)
    LootTable wildlifeLoot;
    wildlifeLoot.id = "wildlife_loot";
    wildlifeLoot.enemyType = "wildlife";
    wildlifeLoot.playerLevel = 1;
    wildlifeLoot.drops = {
        {"raw_meat", "Сырое мясо", 0.5f, 1, 2, 0},
        {"leather_scrap", "Кожаный обрезок", 0.3f, 1, 1, 0},
        {"bone_fragment", "Фрагмент кости", 0.2f, 1, 1, 0}
    };
    wildlifeLoot.experienceReward = 10;
    wildlifeLoot.goldReward = 3;
    registerLootTable(wildlifeLoot);

    // Лут от скелетов
    LootTable skeletonLoot;
    skeletonLoot.id = "skeleton_loot";
    skeletonLoot.enemyType = "skeleton";
    skeletonLoot.playerLevel = 3;
    skeletonLoot.drops = {
        {"bone_fragment", "Фрагмент кости", 0.6f, 1, 3, 0},
        {"rusty_sword", "Ржавый меч", 0.15f, 1, 1, 1},
        {"ancient_coin", "Древняя монета", 0.3f, 1, 5, 0},
        {"soul_ember", "Пепел души", 0.1f, 1, 1, 2}
    };
    skeletonLoot.experienceReward = 30;
    skeletonLoot.goldReward = 10;
    registerLootTable(skeletonLoot);

    // Лут от боссов
    LootTable bossLoot;
    bossLoot.id = "boss_loot";
    bossLoot.enemyType = "boss";
    bossLoot.playerLevel = 10;
    bossLoot.drops = {
        {"boss_trophy", "Трофей босса", 1.0f, 1, 1, 3},
        {"legendary_weapon", "Легендарное оружие", 0.3f, 1, 1, 4},
        {"rare_gem", "Редкий самоцвет", 0.5f, 1, 3, 2},
        {"health_potion", "Зелье здоровья", 0.8f, 3, 5, 0},
        {"mana_potion", "Зелье маны", 0.8f, 3, 5, 0},
        {"epic_armor_fragment", "Фрагмент эпической брони", 0.2f, 1, 1, 3}
    };
    bossLoot.experienceReward = 200;
    bossLoot.goldReward = 100;
    registerLootTable(bossLoot);
}
