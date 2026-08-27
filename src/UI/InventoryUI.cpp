// src/UI/InventoryUI.cpp
#include "Entities/Entity.h"
#include "Items/Item.h"
#include "UI/InventoryUI.h"
#include <iostream>

InventoryUI::InventoryUI(const sf::Font& font)
    : visible(false), font(font), scrollOffset(0) {
    background.setSize(sf::Vector2f(300, 400));
    background.setPosition(362, 100);
    background.setFillColor(sf::Color(50, 50, 50, 220));
    background.setOutlineThickness(2);
    background.setOutlineColor(sf::Color::White);

    titleText.setFont(font);
    titleText.setCharacterSize(20);
    titleText.setFillColor(sf::Color::White);
    titleText.setString("Инвентарь");
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition(background.getPosition().x + (background.getSize().x - titleBounds.width) / 2.0f,
                          background.getPosition().y + 10);

    emptyText.setFont(font);
    emptyText.setCharacterSize(14);
    emptyText.setFillColor(sf::Color(150, 150, 150));
    emptyText.setString("Пусто");
    emptyText.setPosition(background.getPosition().x + 100, background.getPosition().y + 60);
}

void InventoryUI::setVisible(bool isVisible) {
    visible = isVisible;
}

bool InventoryUI::isVisible() const {
    return visible;
}

void InventoryUI::update(float deltaTime, const Entity* player) {
    if (!visible || !player) return;
    updateUI(player); // Обновляем содержимое при каждом вызове, если видим
}

void InventoryUI::updateUI(const Entity* player) {
    itemTexts.clear();
    if (!player) return;

    const auto& inventory = player->getInventory();
    float baseX = background.getPosition().x + 10;
    float baseY = background.getPosition().y + 50;
    int visibleSlots = 13;

    if (inventory.empty()) {
        return;
    }

    int maxScroll = std::max(0, static_cast<int>(inventory.size()) - visibleSlots);
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

    for (int i = 0; i < visibleSlots && (i + scrollOffset) < static_cast<int>(inventory.size()); ++i) {
        int idx = i + scrollOffset;
        const auto& item = inventory[idx];
        sf::Text itemText;
        itemText.setFont(font);
        itemText.setCharacterSize(14);

        std::string itemName = item ? item->getName() : "???";
        std::string typeStr;
        if (item) {
            switch (item->getType()) {
            case ItemType::WEAPON: typeStr = "[Оружие]"; break;
            case ItemType::ARMOR: typeStr = "[Броня]"; break;
            case ItemType::CONSUMABLE: typeStr = "[Расходник]"; break;
            case ItemType::QUEST_ITEM: typeStr = "[Квест]"; break;
            default: typeStr = ""; break;
            }
        }

        sf::Color itemColor;
        if (item) {
            switch (item->getRarity()) {
            case ItemRarity::COMMON: itemColor = sf::Color::White; break;
            case ItemRarity::UNCOMMON: itemColor = sf::Color(100, 255, 100); break;
            case ItemRarity::RARE: itemColor = sf::Color(100, 150, 255); break;
            case ItemRarity::EPIC: itemColor = sf::Color(200, 100, 255); break;
            case ItemRarity::LEGENDARY: itemColor = sf::Color(255, 200, 50); break;
            default: itemColor = sf::Color::White; break;
            }
        } else {
            itemColor = sf::Color::White;
        }

        itemText.setString(itemName + " " + typeStr);
        itemText.setFillColor(itemColor);
        itemText.setPosition(baseX, baseY + i * 24);
        itemTexts.push_back(std::move(itemText));
    }
}

void InventoryUI::draw(sf::RenderTarget& target) {
    if (!visible) return;
    target.draw(background);
    target.draw(titleText);
    if (itemTexts.empty()) {
        target.draw(emptyText);
    }
    for (const auto& text : itemTexts) {
        target.draw(text);
    }
}

void InventoryUI::handleEvent(const sf::Event& event) {
    if (!visible) return;
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::I || event.key.code == sf::Keyboard::Escape) {
            setVisible(false);
        } else if (event.key.code == sf::Keyboard::Up) {
            if (scrollOffset > 0) scrollOffset--;
        } else if (event.key.code == sf::Keyboard::Down) {
            scrollOffset++;
        }
    }
}

