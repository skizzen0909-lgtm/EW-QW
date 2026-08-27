// UI/GodPanelUI.h
#ifndef GODPANELUI_H
#define GODPANELUI_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>

class DivineSystem;
class Player;

struct AbilityButton {
    sf::RectangleShape background;
    sf::Text nameText;
    sf::Text costText;
    sf::Text cooldownText;
    sf::RectangleShape cooldownBar;
    sf::RectangleShape selectHighlight;
    bool selected;
    bool unlocked;
    float cooldownRatio;
    int abilityIndex;
};

class GodPanelUI {
private:
    sf::Font font;
    sf::RenderWindow* window;
    bool visible;
    bool initialized;

    DivineSystem* divineSystem;
    Player* player;

    sf::RectangleShape panelBg;
    sf::RectangleShape faithBarBg;
    sf::RectangleShape faithBarFill;
    sf::Text faithText;

    sf::RectangleShape powerBarBg;
    sf::RectangleShape powerBarFill;
    sf::Text powerText;

    sf::Text titleText;
    sf::Text hint;

    std::vector<AbilityButton> abilityButtons;
    int selectedAbilityIndex;

    struct KarmaBar {
        sf::RectangleShape bg;
        sf::RectangleShape fill;
        sf::Text labelText;
        sf::Text valueText;
    };
    std::vector<KarmaBar> karmaBars;

    void layoutPanel();
    void layoutAbilityButtons();
    void layoutKarmaBars();
    void updateBars();
    void updateCooldowns();
    int getAbilityIndexAtPosition(float mx, float my) const;

public:
    GodPanelUI();
    ~GodPanelUI() = default;

    bool initialize(sf::RenderWindow& win, sf::Font& f);
    void setDivineSystem(DivineSystem* ds);
    void setPlayer(Player* p);

    void toggle();
    void show();
    void hide();
    bool isVisible() const;

    void handleEvent(const sf::Event& event);
    void update(float deltaTime);
    void draw(sf::RenderTarget& target);
};

#endif // GODPANELUI_H
