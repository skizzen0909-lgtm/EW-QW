// UI/GodPanelUI.cpp
#include "GodPanelUI.h"
#include "Systems/DivineSystem.h"
#include "Entities/Player.h"
#include <iostream>
#include <sstream>
#include <cmath>

GodPanelUI::GodPanelUI()
    : window(nullptr), visible(false), initialized(false)
    , divineSystem(nullptr), player(nullptr)
    , selectedAbilityIndex(-1) {
}

bool GodPanelUI::initialize(sf::RenderWindow& win, sf::Font& f) {
    window = &win;
    font = f;
    initialized = true;

    panelBg.setSize(sf::Vector2f(360, 480));
    panelBg.setFillColor(sf::Color(10, 5, 30, 240));
    panelBg.setOutlineThickness(2);
    panelBg.setOutlineColor(sf::Color(120, 80, 220));

    titleText.setFont(font);
    titleText.setString("BOЖЕСТВЕННЫE СПОСОБНОСТИ");
    titleText.setCharacterSize(14);
    titleText.setFillColor(sf::Color(200, 170, 255));

    faithBarBg.setSize(sf::Vector2f(320, 18));
    faithBarBg.setFillColor(sf::Color(30, 30, 30));
    faithBarBg.setOutlineThickness(1);
    faithBarBg.setOutlineColor(sf::Color(80, 80, 80));

    faithBarFill.setSize(sf::Vector2f(0, 18));
    faithBarFill.setFillColor(sf::Color(180, 140, 255));

    faithText.setFont(font);
    faithText.setCharacterSize(11);
    faithText.setFillColor(sf::Color(220, 200, 255));

    powerBarBg.setSize(sf::Vector2f(320, 18));
    powerBarBg.setFillColor(sf::Color(30, 30, 30));
    powerBarBg.setOutlineThickness(1);
    powerBarBg.setOutlineColor(sf::Color(80, 80, 80));

    powerBarFill.setSize(sf::Vector2f(0, 18));
    powerBarFill.setFillColor(sf::Color(255, 200, 60));

    powerText.setFont(font);
    powerText.setCharacterSize(11);
    powerText.setFillColor(sf::Color(255, 230, 150));

    hint.setFont(font);
    hint.setString("[G] закрыть | [1-8] использовать | клик = выбрать");
    hint.setCharacterSize(10);
    hint.setFillColor(sf::Color(120, 120, 120));

    layoutPanel();
    layoutAbilityButtons();
    layoutKarmaBars();

    return true;
}

void GodPanelUI::setDivineSystem(DivineSystem* ds) { divineSystem = ds; }
void GodPanelUI::setPlayer(Player* p) { player = p; }
void GodPanelUI::toggle() { visible ? hide() : show(); }
void GodPanelUI::show() { visible = true; }
void GodPanelUI::hide() { visible = false; selectedAbilityIndex = -1; }
bool GodPanelUI::isVisible() const { return visible; }

void GodPanelUI::layoutPanel() {
    if (!window) return;
    float wx = static_cast<float>(window->getSize().x);
    float wy = static_cast<float>(window->getSize().y);
    float px = (wx - 360.0f) / 2.0f;
    float py = (wy - 480.0f) / 2.0f;
    panelBg.setPosition(px, py);
    titleText.setPosition(px + 20, py + 12);
    faithBarBg.setPosition(px + 20, py + 42);
    faithBarFill.setPosition(px + 20, py + 42);
    faithText.setPosition(px + 25, py + 43);
    powerBarBg.setPosition(px + 20, py + 68);
    powerBarFill.setPosition(px + 20, py + 68);
    powerText.setPosition(px + 25, py + 69);
    hint.setPosition(px + 20, py + 460);
}

void GodPanelUI::layoutAbilityButtons() {
    if (!window) return;
    float px = panelBg.getPosition().x;
    float py = panelBg.getPosition().y;
    float btnW = 320.0f;
    float btnH = 40.0f;
    float startY = py + 100.0f;
    float gap = 6.0f;
    int maxButtons = 8;

    abilityButtons.clear();
    for (int i = 0; i < maxButtons; i++) {
        AbilityButton btn;
        float y = startY + i * (btnH + gap);

        btn.background.setSize(sf::Vector2f(btnW, btnH));
        btn.background.setPosition(px + 20, y);
        btn.background.setFillColor(sf::Color(30, 25, 50));
        btn.background.setOutlineThickness(1);
        btn.background.setOutlineColor(sf::Color(70, 60, 100));

        btn.nameText.setFont(font);
        btn.nameText.setCharacterSize(12);
        btn.nameText.setFillColor(sf::Color(200, 200, 220));
        btn.nameText.setPosition(px + 50, y + 4);

        btn.costText.setFont(font);
        btn.costText.setCharacterSize(9);
        btn.costText.setFillColor(sf::Color(160, 140, 200));
        btn.costText.setPosition(px + 50, y + 22);

        btn.cooldownText.setFont(font);
        btn.cooldownText.setCharacterSize(9);
        btn.cooldownText.setFillColor(sf::Color(255, 100, 100));
        btn.cooldownText.setPosition(px + 200, y + 22);

        btn.cooldownBar.setSize(sf::Vector2f(btnW - 4, 3));
        btn.cooldownBar.setPosition(px + 21, y + btnH - 3);
        btn.cooldownBar.setFillColor(sf::Color(255, 80, 80, 150));

        btn.selectHighlight.setSize(sf::Vector2f(btnW + 4, btnH + 4));
        btn.selectHighlight.setPosition(px + 18, y - 2);
        btn.selectHighlight.setFillColor(sf::Color::Transparent);
        btn.selectHighlight.setOutlineThickness(2);
        btn.selectHighlight.setOutlineColor(sf::Color(200, 170, 255));

        btn.selected = false;
        btn.unlocked = false;
        btn.cooldownRatio = 0.0f;
        btn.abilityIndex = i;

        abilityButtons.push_back(btn);
    }
}

void GodPanelUI::layoutKarmaBars() {
    if (!window) return;
    float px = panelBg.getPosition().x;
    float py = panelBg.getPosition().y;
    float startY = py + 100.0f + 8 * 46.0f + 10.0f;

    karmaBars.clear();
    const char* labels[] = { "Мудрость", "Милосердие", "Гнев", "Хитрость", "Хаос" };
    for (int i = 0; i < 5; i++) {
        KarmaBar kb;
        kb.bg.setSize(sf::Vector2f(150, 8));
        kb.bg.setPosition(px + 20 + (i % 3) * 110, startY + (i / 3) * 22);
        kb.bg.setFillColor(sf::Color(30, 30, 30));
        kb.bg.setOutlineThickness(1);
        kb.bg.setOutlineColor(sf::Color(60, 60, 60));

        kb.fill.setSize(sf::Vector2f(0, 8));
        kb.fill.setPosition(px + 20 + (i % 3) * 110, startY + (i / 3) * 22);
        kb.fill.setFillColor(sf::Color(100, 200, 255));

        kb.labelText.setFont(font);
        kb.labelText.setString(labels[i]);
        kb.labelText.setCharacterSize(8);
        kb.labelText.setFillColor(sf::Color(150, 150, 170));
        kb.labelText.setPosition(px + 20 + (i % 3) * 110, startY + (i / 3) * 22 - 10);

        kb.valueText.setFont(font);
        kb.valueText.setCharacterSize(8);
        kb.valueText.setFillColor(sf::Color(180, 180, 200));
        kb.valueText.setPosition(px + 175 + (i % 3) * 110, startY + (i / 3) * 22 - 10);

        karmaBars.push_back(kb);
    }
}

void GodPanelUI::updateBars() {
    if (!divineSystem) return;

    float faithRatio = divineSystem->getFaith() / divineSystem->getMaxFaith();
    faithBarFill.setSize(sf::Vector2f(320.0f * faithRatio, 18));
    faithBarFill.setPosition(faithBarBg.getPosition());

    std::ostringstream faithSS;
    faithSS << "Bepa: " << static_cast<int>(divineSystem->getFaith())
            << " / " << static_cast<int>(divineSystem->getMaxFaith());
    faithText.setString(faithSS.str());

    float powerRatio = std::min(divineSystem->getPower() / 5.0f, 1.0f);
    powerBarFill.setSize(sf::Vector2f(320.0f * powerRatio, 18));
    powerBarFill.setPosition(powerBarBg.getPosition());

    std::ostringstream powerSS;
    powerSS << "Bласть: " << divineSystem->getPower();
    powerText.setString(powerSS.str());

    const KarmaScale scales[] = {
        KarmaScale::WISDOM, KarmaScale::MERCY, KarmaScale::RAGE,
        KarmaScale::CUNNING, KarmaScale::CHAOS
    };
    for (int i = 0; i < 5 && i < static_cast<int>(karmaBars.size()); i++) {
        float val = divineSystem->getKarma(scales[i]);
        float ratio = std::abs(val) / 100.0f;
        float w = 150.0f * ratio;
        if (val >= 0) {
            karmaBars[i].fill.setFillColor(sf::Color(100, 200, 255));
        } else {
            karmaBars[i].fill.setFillColor(sf::Color(255, 100, 100));
        }
        karmaBars[i].fill.setSize(sf::Vector2f(w, 8));
        karmaBars[i].fill.setPosition(karmaBars[i].bg.getPosition());

        std::ostringstream valSS;
        valSS << (val >= 0 ? "+" : "") << static_cast<int>(val);
        karmaBars[i].valueText.setString(valSS.str());
    }
}

void GodPanelUI::updateCooldowns() {
    if (!divineSystem) return;

    const DivineAbilityType types[] = {
        DivineAbilityType::BLESS_POWER,
        DivineAbilityType::BLESS_HEALTH,
        DivineAbilityType::LIGHTNING,
        DivineAbilityType::WEATHER_CONTROL,
        DivineAbilityType::EMOTION_CONTROL,
        DivineAbilityType::ENEMY_DISTRACT,
        DivineAbilityType::TIME_MANIPULATION,
        DivineAbilityType::DIRECTION_GUIDE
    };

    const char* hotkeys[] = { "1", "2", "3", "4", "5", "6", "7", "8" };

    for (int i = 0; i < static_cast<int>(abilityButtons.size()) && i < 8; i++) {
        const DivineAbility* ability = divineSystem->getAbility(types[i]);
        AbilityButton& btn = abilityButtons[i];

        if (!ability) {
            btn.unlocked = false;
            btn.nameText.setString("");
            btn.costText.setString("");
            btn.cooldownText.setString("");
            btn.background.setFillColor(sf::Color(20, 20, 20));
            continue;
        }

        btn.unlocked = ability->unlocked;
        btn.selected = (i == selectedAbilityIndex);

        if (!ability->unlocked) {
            btn.nameText.setString("??? " + ability->name);
            btn.nameText.setFillColor(sf::Color(100, 100, 100));
            btn.costText.setString("Условие: " + ability->unlockCondition);
            btn.costText.setFillColor(sf::Color(100, 80, 80));
            btn.cooldownText.setString("");
            btn.background.setFillColor(sf::Color(20, 20, 25));
            btn.selectHighlight.setOutlineColor(sf::Color(60, 60, 80));
        } else {
            bool canUse = divineSystem->canUseAbility(types[i]);
            btn.nameText.setString(std::string("[") + hotkeys[i] + "] " + ability->name);
            btn.nameText.setFillColor(canUse ? sf::Color(220, 220, 240) : sf::Color(140, 140, 160));

            std::ostringstream costSS;
            costSS << "Bepa: " << static_cast<int>(ability->faithRequired) << "  CD: " << static_cast<int>(ability->cooldown) << "s";
            btn.costText.setString(costSS.str());
            btn.costText.setFillColor(canUse ? sf::Color(140, 200, 140) : sf::Color(120, 100, 100));

            if (ability->currentCooldown > 0) {
                btn.cooldownText.setString("ПЕРЕЗАРЯДКА");
                btn.cooldownRatio = ability->currentCooldown / ability->cooldown;
            } else {
                btn.cooldownText.setString(canUse ? "ГОТОВО" : "НЕДОСТАТОЧНО ВЕРЫ");
                btn.cooldownRatio = 0.0f;
            }

            if (btn.selected) {
                btn.background.setFillColor(sf::Color(40, 35, 70));
                btn.selectHighlight.setOutlineColor(sf::Color(200, 170, 255));
            } else if (canUse) {
                btn.background.setFillColor(sf::Color(30, 25, 50));
                btn.selectHighlight.setOutlineColor(sf::Color(70, 60, 100));
            } else {
                btn.background.setFillColor(sf::Color(25, 22, 40));
                btn.selectHighlight.setOutlineColor(sf::Color(50, 50, 70));
            }
        }

        btn.cooldownBar.setSize(sf::Vector2f((320.0f - 4) * btn.cooldownRatio, 3));
    }
}

int GodPanelUI::getAbilityIndexAtPosition(float mx, float my) const {
    for (int i = 0; i < static_cast<int>(abilityButtons.size()); i++) {
        sf::FloatRect bounds = abilityButtons[i].background.getGlobalBounds();
        if (bounds.contains(mx, my)) {
            return i;
        }
    }
    return -1;
}

void GodPanelUI::handleEvent(const sf::Event& event) {
    if (!visible || !divineSystem) return;

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::G) {
            hide();
            return;
        }
        int hotkeyIndex = -1;
        if (event.key.code >= sf::Keyboard::Num1 && event.key.code <= sf::Keyboard::Num8) {
            hotkeyIndex = event.key.code - sf::Keyboard::Num1;
        } else if (event.key.code >= sf::Keyboard::Numpad1 && event.key.code <= sf::Keyboard::Numpad8) {
            hotkeyIndex = event.key.code - sf::Keyboard::Numpad1;
        }

        if (hotkeyIndex >= 0 && hotkeyIndex < static_cast<int>(abilityButtons.size())) {
            const DivineAbilityType types[] = {
                DivineAbilityType::BLESS_POWER,
                DivineAbilityType::BLESS_HEALTH,
                DivineAbilityType::LIGHTNING,
                DivineAbilityType::WEATHER_CONTROL,
                DivineAbilityType::EMOTION_CONTROL,
                DivineAbilityType::ENEMY_DISTRACT,
                DivineAbilityType::TIME_MANIPULATION,
                DivineAbilityType::DIRECTION_GUIDE
            };
            if (divineSystem->useAbility(types[hotkeyIndex])) {
                std::cout << "[GOD PANEL] Ability used: " << hotkeyIndex + 1 << std::endl;
            }
        }
    }

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        float mx = static_cast<float>(event.mouseButton.x);
        float my = static_cast<float>(event.mouseButton.y);
        int idx = getAbilityIndexAtPosition(mx, my);
        if (idx >= 0) {
            if (idx == selectedAbilityIndex) {
                const DivineAbilityType types[] = {
                    DivineAbilityType::BLESS_POWER,
                    DivineAbilityType::BLESS_HEALTH,
                    DivineAbilityType::LIGHTNING,
                    DivineAbilityType::WEATHER_CONTROL,
                    DivineAbilityType::EMOTION_CONTROL,
                    DivineAbilityType::ENEMY_DISTRACT,
                    DivineAbilityType::TIME_MANIPULATION,
                    DivineAbilityType::DIRECTION_GUIDE
                };
                divineSystem->useAbility(types[idx]);
                selectedAbilityIndex = -1;
            } else {
                selectedAbilityIndex = idx;
            }
        } else {
            selectedAbilityIndex = -1;
        }
    }
}

void GodPanelUI::update(float deltaTime) {
    if (!visible) return;
    updateBars();
    updateCooldowns();
}

void GodPanelUI::draw(sf::RenderTarget& target) {
    if (!visible) return;

    target.draw(panelBg);
    target.draw(titleText);

    target.draw(faithBarBg);
    target.draw(faithBarFill);
    target.draw(faithText);

    target.draw(powerBarBg);
    target.draw(powerBarFill);
    target.draw(powerText);

    for (const auto& btn : abilityButtons) {
        if (btn.nameText.getString().isEmpty() && btn.costText.getString().isEmpty()) continue;
        target.draw(btn.background);
        if (btn.selected) target.draw(btn.selectHighlight);
        target.draw(btn.nameText);
        target.draw(btn.costText);
        if (!btn.cooldownText.getString().isEmpty()) target.draw(btn.cooldownText);
        if (btn.cooldownRatio > 0) target.draw(btn.cooldownBar);
    }

    for (const auto& kb : karmaBars) {
        target.draw(kb.bg);
        target.draw(kb.fill);
        target.draw(kb.labelText);
        target.draw(kb.valueText);
    }

    target.draw(hint);
}
