// Systems/TradeSystem.h
#ifndef TRADESYSTEM_H
#define TRADESYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class Entity;
class Item;

struct TradeItem {
    std::string itemId;
    std::string itemName;
    int buyPrice;
    int sellPrice;
    int stock;
    int requiredPlayerLevel;
    bool isAvailable;

    TradeItem()
        : buyPrice(0), sellPrice(0), stock(-1), requiredPlayerLevel(1), isAvailable(true) {}
    TradeItem(const std::string& id, const std::string& name, int buy, int sell, int st = -1, int lvl = 1)
        : itemId(id), itemName(name), buyPrice(buy), sellPrice(sell), stock(st),
          requiredPlayerLevel(lvl), isAvailable(true) {}
};

struct TradeOffer {
    std::string npcId;
    std::string npcName;
    std::vector<TradeItem> buyItems;
    std::vector<TradeItem> sellItems;
    int reputationRequired;
    float priceModifier;

    TradeOffer()
        : reputationRequired(0), priceModifier(1.0f) {}
};

class TradeSystem {
private:
    std::unordered_map<std::string, TradeOffer> tradeOffers;
    Entity* currentTradePartner;
    bool trading;

public:
    TradeSystem();
    ~TradeSystem() = default;

    void registerTradeOffer(const TradeOffer& offer);
    void unregisterTradeOffer(const std::string& npcId);

    bool startTrading(const std::string& npcId, Entity* player);
    void stopTrading();
    bool isTrading() const;

    bool buyItem(const std::string& itemId, Entity* player);
    bool sellItem(const std::string& itemId, Entity* player, int quantity = 1);

    int calculateBuyPrice(const TradeItem& item, int playerReputation) const;
    int calculateSellPrice(const TradeItem& item, int playerReputation) const;

    const TradeOffer* findTradeOffer(const std::string& npcId) const;
    const TradeOffer* getCurrentTradeOffer() const;
    std::vector<TradeItem> getAvailableBuyItems(const std::string& npcId, int playerLevel, int playerReputation) const;

    void initializeDefaultTradeOffers();

    TradeSystem(const TradeSystem&) = delete;
    TradeSystem& operator=(const TradeSystem&) = delete;
};

#endif // TRADESYSTEM_H
