// Entities/Merchant.h
#ifndef MERCHANT_H
#define MERCHANT_H

#include "Entities/NPC.h"
#include <vector>
#include <string>

struct MerchantItem {
    std::string itemId;
    std::string itemName;
    int price;
    int stock;

    MerchantItem(const std::string& id, const std::string& name, int p, int s)
        : itemId(id), itemName(name), price(p), stock(s) {}
};

class Merchant : public NPC {
private:
    std::vector<MerchantItem> items;
    int gold;
    float priceModifier;

public:
    Merchant(const std::string& name, int health = 80);
    ~Merchant() override = default;

    void interact(Entity* player) override;
    std::string getDialogue() const override;

    void addItem(const MerchantItem& item);
    bool buyItem(const std::string& itemId, int quantity = 1);
    bool sellItem(const std::string& itemId, int quantity = 1);

    const std::vector<MerchantItem>& getItems() const;
    int getGold() const;
    float getPriceModifier() const;
    void setPriceModifier(float modifier);

    void restock();

    static std::unique_ptr<Merchant> createVillageMerchant(int level);
    static std::unique_ptr<Merchant> createBlacksmith(int level);
    static std::unique_ptr<Merchant> createAlchemist(int level);
};

#endif // MERCHANT_H
