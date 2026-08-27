// Systems/TradeSystem.cpp
#include "TradeSystem.h"
#include "Entities/Entity.h"
#include "Items/Item.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cmath>

TradeSystem::TradeSystem()
    : currentTradePartner(nullptr)
    , trading(false) {
    initializeDefaultTradeOffers();
}

void TradeSystem::registerTradeOffer(const TradeOffer& offer) {
    tradeOffers[offer.npcId] = offer;
}

void TradeSystem::unregisterTradeOffer(const std::string& npcId) {
    tradeOffers.erase(npcId);
}

bool TradeSystem::startTrading(const std::string& npcId, Entity* player) {
    if (!player || trading) return false;

    auto it = tradeOffers.find(npcId);
    if (it == tradeOffers.end()) return false;

    currentTradePartner = player;
    trading = true;

    std::ostringstream oss;
    oss << "Торговля начата с " << it->second.npcName;
    std::cout << oss.str() << std::endl;

    return true;
}

void TradeSystem::stopTrading() {
    currentTradePartner = nullptr;
    trading = false;
    std::cout << "Торговля завершена." << std::endl;
}

bool TradeSystem::isTrading() const {
    return trading;
}

bool TradeSystem::buyItem(const std::string& itemId, Entity* player) {
    if (!trading || !currentTradePartner || !player) return false;

    for (auto& [npcId, offer] : tradeOffers) {
        for (auto& item : offer.buyItems) {
            if (item.itemId == itemId && item.isAvailable) {
                if (item.stock == 0) {
                    std::cout << "Товар закончился: " << item.itemName << std::endl;
                    return false;
                }

                int price = calculateBuyPrice(item, 0);
                if (!player->spendGold(price)) {
                    std::cout << "Недостаточно золота! Нужно: " << price << ", есть: " << player->getGold() << std::endl;
                    return false;
                }

                if (item.stock > 0) {
                    item.stock--;
                }

                auto newItem = std::make_shared<Item>(
                    item.itemId, item.itemName, "Куплено у торговца",
                    ItemType::MISC, ItemRarity::COMMON, item.buyPrice);
                player->addItem(newItem);

                std::cout << player->getName() << " покупает " << item.itemName
                    << " за " << price << " золота" << std::endl;
                return true;
            }
        }
    }

    std::cout << "Предмет не найден в магазине." << std::endl;
    return false;
}

bool TradeSystem::sellItem(const std::string& itemId, Entity* player, int quantity) {
    if (!trading || !player) return false;

    for (auto& [npcId, offer] : tradeOffers) {
        for (auto& item : offer.sellItems) {
            if (item.itemId == itemId) {
                int price = calculateSellPrice(item, 0) * quantity;

                // Check if player has the item
                bool found = false;
                for (const auto& invItem : player->getInventory()) {
                    if (invItem && invItem->getId() == itemId) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    std::cout << "Предмет не найден в инвентаре: " << item.itemName << std::endl;
                    return false;
                }

                // Remove from inventory and add gold
                for (auto& invItem : player->getInventory()) {
                    if (invItem && invItem->getId() == itemId) {
                        player->removeItem(invItem);
                        break;
                    }
                }
                player->addGold(price);

                std::cout << player->getName() << " продаёт " << item.itemName
                    << " x" << quantity << " за " << price << " золота" << std::endl;
                return true;
            }
        }
    }

    return false;
}

int TradeSystem::calculateBuyPrice(const TradeItem& item, int playerReputation) const {
    float repModifier = 1.0f - (playerReputation * 0.001f);
    repModifier = std::max(0.5f, std::min(1.5f, repModifier));
    return static_cast<int>(item.buyPrice * repModifier);
}

int TradeSystem::calculateSellPrice(const TradeItem& item, int playerReputation) const {
    float repModifier = 1.0f + (playerReputation * 0.001f);
    repModifier = std::max(0.5f, std::min(2.0f, repModifier));
    return std::max(1, static_cast<int>(item.sellPrice * repModifier));
}

const TradeOffer* TradeSystem::findTradeOffer(const std::string& npcId) const {
    auto it = tradeOffers.find(npcId);
    return (it != tradeOffers.end()) ? &it->second : nullptr;
}

const TradeOffer* TradeSystem::getCurrentTradeOffer() const {
    if (!trading || !currentTradePartner) return nullptr;
    // Ищем оффер для текущего NPC (упрощённо)
    for (const auto& [npcId, offer] : tradeOffers) {
        return &offer;
    }
    return nullptr;
}

std::vector<TradeItem> TradeSystem::getAvailableBuyItems(const std::string& npcId, int playerLevel, int playerReputation) const {
    std::vector<TradeItem> result;
    auto it = tradeOffers.find(npcId);
    if (it != tradeOffers.end()) {
        for (const auto& item : it->second.buyItems) {
            if (item.isAvailable && playerLevel >= item.requiredPlayerLevel) {
                result.push_back(item);
            }
        }
    }
    return result;
}

void TradeSystem::initializeDefaultTradeOffers() {
    // Торговец в деревне
    TradeOffer villageMerchant;
    villageMerchant.npcId = "village_merchant";
    villageMerchant.npcName = "Торговец";
    villageMerchant.reputationRequired = 0;
    villageMerchant.priceModifier = 1.0f;
    villageMerchant.buyItems = {
        {"health_potion", "Зелье здоровья", 25, 8, -1, 1},
        {"mana_potion", "Зелье маны", 30, 10, -1, 1},
        {"bread", "Хлеб", 5, 2, -1, 1},
        {"iron_sword", "Железный меч", 100, 35, 3, 3},
        {"leather_armor", "Кожаная броня", 80, 25, 3, 2},
        {"arrow", "Стрела", 3, 1, 50, 1},
        {"torch", "Факел", 8, 3, -1, 1}
    };
    villageMerchant.sellItems = {
        {"wolf_fang", "Волчий клык", 0, 8},
        {"wolf_pelt", "Волчья шкура", 0, 15},
        {"leather_scrap", "Кожаный обрезок", 0, 5},
        {"bone_fragment", "Фрагмент кости", 0, 3}
    };
    registerTradeOffer(villageMerchant);

    // Кузнец
    TradeOffer blacksmith;
    blacksmith.npcId = "blacksmith";
    blacksmith.npcName = "Кузнец";
    blacksmith.reputationRequired = 10;
    blacksmith.priceModifier = 0.9f;
    blacksmith.buyItems = {
        {"steel_sword", "Стальной меч", 250, 80, 2, 5},
        {"iron_shield", "Железный щит", 120, 40, 2, 3},
        {"steel_ingot", "Стальной слиток", 60, 20, -1, 3},
        {"chainmail", "Кольчуга", 300, 100, 1, 7},
        {"war_hammer", "Боевой молот", 200, 65, 2, 5}
    };
    blacksmith.sellItems = {
        {"iron_ingot", "Железный слиток", 0, 25},
        {"steel_ingot", "Стальной слиток", 0, 50},
        {"ruby", "Рубин", 0, 100},
        {"ancient_coin", "Древняя монета", 0, 15}
    };
    registerTradeOffer(blacksmith);

    // Алхимик
    TradeOffer alchemist;
    alchemist.npcId = "alchemist";
    alchemist.npcName = "Алхимик";
    alchemist.reputationRequired = 5;
    alchemist.priceModifier = 1.1f;
    alchemist.buyItems = {
        {"greater_health_potion", "Большое зелье здоровья", 80, 30, 5, 5},
        {"greater_mana_potion", "Большое зелье маны", 90, 35, 5, 5},
        {"antidote", "Противоядие", 40, 15, 10, 2},
        {"strength_elixir", "Эликсир силы", 150, 60, 2, 8},
        {"mana_crystal", "Кристалл маны", 50, 18, -1, 3}
    };
    alchemist.sellItems = {
        {"herb", "Трава", 0, 4},
        {"mushroom", "Гриб", 0, 6},
        {"mana_crystal", "Кристалл маны", 0, 30},
        {"dragon_scale", "Чешуя дракона", 0, 200}
    };
    registerTradeOffer(alchemist);
}
