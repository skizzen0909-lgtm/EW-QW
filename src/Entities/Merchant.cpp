// Entities/Merchant.cpp
#include "Merchant.h"
#include <iostream>

Merchant::Merchant(const std::string& name, int health)
    : NPC(name, "merchant", health)
    , gold(100)
    , priceModifier(1.0f) {
    items.emplace_back("health_potion", "Здоровье зелье", 20, 5);
    items.emplace_back("bread", "Хлеб", 5, 10);
    items.emplace_back("water_flask", "Фляга воды", 3, 10);
}

void Merchant::interact(Entity* player) {
    NPC::interact(player);
    std::cout << "[MERCHANT] " << getName() << ": Добро пожаловать!" << std::endl;
}

std::string Merchant::getDialogue() const {
    return "Добро пожаловать в мою лавку!";
}

void Merchant::addItem(const MerchantItem& item) {
    items.push_back(item);
}

bool Merchant::buyItem(const std::string& itemId, int quantity) {
    for (auto& item : items) {
        if (item.itemId == itemId && item.stock >= quantity) {
            item.stock -= quantity;
            gold += item.price * quantity;
            return true;
        }
    }
    return false;
}

bool Merchant::sellItem(const std::string& itemId, int quantity) {
    for (auto& item : items) {
        if (item.itemId == itemId && gold >= item.price * quantity) {
            item.stock += quantity;
            gold -= item.price * quantity;
            return true;
        }
    }
    return false;
}

const std::vector<MerchantItem>& Merchant::getItems() const { return items; }
int Merchant::getGold() const { return gold; }
float Merchant::getPriceModifier() const { return priceModifier; }
void Merchant::setPriceModifier(float modifier) { priceModifier = modifier; }

void Merchant::restock() {
    for (auto& item : items) {
        item.stock = std::min(item.stock + 3, 20);
    }
}

std::unique_ptr<Merchant> Merchant::createVillageMerchant(int level) {
    auto merchant = std::make_unique<Merchant>("Торговец", 80);
    merchant->addItem(MerchantItem("iron_sword", "Железный меч", 50 + level * 10, 2));
    merchant->addItem(MerchantItem("leather_armor", "Кожаная броня", 40 + level * 8, 2));
    merchant->addItem(MerchantItem("health_potion", "Здоровье зелье", 20, 10));
    return merchant;
}

std::unique_ptr<Merchant> Merchant::createBlacksmith(int level) {
    auto smith = std::make_unique<Merchant>("Кузнец", 100);
    smith->setPriceModifier(1.2f);
    smith->addItem(MerchantItem("steel_sword", "Стальной меч", 100 + level * 15, 1));
    smith->addItem(MerchantItem("steel_armor", "Стальная броня", 80 + level * 12, 1));
    smith->addItem(MerchantItem("repair_kit", "Ремкомплект", 30, 5));
    return smith;
}

std::unique_ptr<Merchant> Merchant::createAlchemist(int level) {
    auto alchemist = std::make_unique<Merchant>("Алхимик", 70);
    alchemist->setPriceModifier(0.9f);
    alchemist->addItem(MerchantItem("health_potion", "Здоровье зелье", 25, 8));
    alchemist->addItem(MerchantItem("mana_potion", "Мана зелье", 30, 5));
    alchemist->addItem(MerchantItem("antidote", "Противоядие", 15, 6));
    return alchemist;
}
