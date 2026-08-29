#include "UI/HeroDiary.h"
#include <iostream>
#include <sstream>
#include <algorithm>

HeroDiary::HeroDiary()
    : visible(false)
    , scrollOffset(0)
    , maxVisible(14) {
}

bool HeroDiary::initialize(sf::Font& f) {
    font = f;

    panelBg.setSize(sf::Vector2f(400, 350));
    panelBg.setPosition(100, 50);
    panelBg.setFillColor(sf::Color(15, 20, 30, 230));
    panelBg.setOutlineThickness(2);
    panelBg.setOutlineColor(sf::Color(100, 130, 180));

    panelBorder.setSize(sf::Vector2f(396, 346));
    panelBorder.setFillColor(sf::Color::Transparent);
    panelBorder.setOutlineThickness(1);
    panelBorder.setOutlineColor(sf::Color(60, 80, 120, 100));
    panelBorder.setPosition(102, 52);

    titleText.setFont(font);
    titleText.setString("ДНЕВНИК ГЕРОЯ");
    titleText.setCharacterSize(18);
    titleText.setFillColor(sf::Color(180, 200, 240));
    titleText.setPosition(200, 60);

    scrollHint.setFont(font);
    scrollHint.setString("Up/Down - прокрутка, J - закрыть");
    scrollHint.setCharacterSize(10);
    scrollHint.setFillColor(sf::Color(100, 120, 150));
    scrollHint.setPosition(120, 385);

    dayText.setFont(font);
    dayText.setCharacterSize(12);
    dayText.setFillColor(sf::Color(150, 170, 200));

    std::cout << "[DIARY] Initialized" << std::endl;
    return true;
}

void HeroDiary::shutdown() {
    entries.clear();
}

void HeroDiary::addEntry(int day, const std::string& timeOfDay, const std::string& text,
                         sf::Color color) {
    DiaryEntry entry(day, timeOfDay, text, color);
    entries.push_back(entry);

    if (entries.size() > 200) {
        entries.erase(entries.begin());
    }

    if (visible && static_cast<int>(entries.size()) > maxVisible) {
        scrollOffset = static_cast<int>(entries.size()) - maxVisible;
    }
}

void HeroDiary::update(float deltaTime) {
    if (!visible) return;

    int totalEntries = static_cast<int>(entries.size());
    if (totalEntries > maxVisible && scrollOffset > totalEntries - maxVisible) {
        scrollOffset = totalEntries - maxVisible;
    }
    if (scrollOffset < 0) scrollOffset = 0;
}

void HeroDiary::draw(sf::RenderTarget& target) {
    if (!visible) return;

    target.draw(panelBg);
    target.draw(panelBorder);
    target.draw(titleText);

    int totalEntries = static_cast<int>(entries.size());
    if (totalEntries == 0) {
        sf::Text emptyText;
        emptyText.setFont(font);
        emptyText.setString("Дневник пуст...");
        emptyText.setCharacterSize(14);
        emptyText.setFillColor(sf::Color(100, 120, 150));
        emptyText.setPosition(230, 200);
        target.draw(emptyText);
        target.draw(scrollHint);
        return;
    }

    int startIdx = scrollOffset;
    int endIdx = std::min(startIdx + maxVisible, totalEntries);
    int visibleCount = endIdx - startIdx;

    for (int i = 0; i < visibleCount; ++i) {
        int idx = startIdx + i;
        const auto& entry = entries[idx];

        float y = 85.0f + i * 22.0f;

        sf::Text dayLine;
        dayLine.setFont(font);
        std::ostringstream dss;
        dss << "День " << entry.day << " [" << entry.timeOfDay << "]";
        dayLine.setString(dss.str());
        dayLine.setCharacterSize(10);
        dayLine.setFillColor(sf::Color(120, 140, 170));
        dayLine.setPosition(110, y);

        sf::Text textLine;
        textLine.setFont(font);
        textLine.setString(entry.text);
        textLine.setCharacterSize(11);
        textLine.setFillColor(entry.color);
        textLine.setPosition(110, y + 11);

        target.draw(dayLine);
        target.draw(textLine);
    }

    if (totalEntries > maxVisible) {
        float scrollY = 85.0f;
        float scrollH = 310.0f;
        float scrollRatio = static_cast<float>(scrollOffset) / (totalEntries - maxVisible);

        sf::RectangleShape scrollBg;
        scrollBg.setSize(sf::Vector2f(4, scrollH));
        scrollBg.setPosition(485, scrollY);
        scrollBg.setFillColor(sf::Color(30, 40, 60));
        target.draw(scrollBg);

        sf::RectangleShape scrollHandle;
        scrollHandle.setSize(sf::Vector2f(4, 20));
        scrollHandle.setPosition(485, scrollY + scrollRatio * (scrollH - 20));
        scrollHandle.setFillColor(sf::Color(100, 130, 180));
        target.draw(scrollHandle);

        std::ostringstream hint;
        hint << (scrollOffset + 1) << "-" << endIdx << " / " << totalEntries;
        sf::Text countText;
        countText.setFont(font);
        countText.setString(hint.str());
        countText.setCharacterSize(9);
        countText.setFillColor(sf::Color(100, 120, 150));
        countText.setPosition(350, 385);
        target.draw(countText);
    }

    target.draw(scrollHint);
}

void HeroDiary::handleEvent(const sf::Event& event) {
    if (!visible) return;

    if (event.type == sf::Event::KeyPressed) {
        int totalEntries = static_cast<int>(entries.size());
        if (event.key.code == sf::Keyboard::Up) {
            if (scrollOffset > 0) scrollOffset--;
        } else if (event.key.code == sf::Keyboard::Down) {
            if (scrollOffset + maxVisible < totalEntries) scrollOffset++;
        } else if (event.key.code == sf::Keyboard::J || event.key.code == sf::Keyboard::Escape) {
            hide();
        }
    }
}

void HeroDiary::toggle() { visible = !visible; scrollOffset = 0; }
void HeroDiary::show() { visible = true; scrollOffset = 0; }
void HeroDiary::hide() { visible = false; }
bool HeroDiary::isVisible() const { return visible; }
int HeroDiary::getEntryCount() const { return static_cast<int>(entries.size()); }
const DiaryEntry& HeroDiary::getEntry(int index) const { return entries[index]; }
