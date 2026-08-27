// src/UI/InventoryUI.h
#ifndef INVENTORYUI_H
#define INVENTORYUI_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

// Forward declarations
class Entity; // Для получения инвентаря
class Item;  // Для отображения предметов

class InventoryUI {
private:
    bool visible;
    sf::Font font;
    sf::RectangleShape background;
    std::vector<sf::Text> itemTexts;
    sf::Text titleText;
    sf::Text emptyText;
    int scrollOffset;
    void updateUI(const Entity* player);
public:
    explicit InventoryUI(const sf::Font& font);
    ~InventoryUI() = default;
    void setVisible(bool isVisible);
    bool isVisible() const;
    void update(float deltaTime, const Entity* player);
    void draw(sf::RenderTarget& target);
    void handleEvent(const sf::Event& event);
    InventoryUI(const InventoryUI&) = delete;
    InventoryUI& operator=(const InventoryUI&) = delete;
};

#endif // INVENTORYUI_H