#ifndef HERODIARY_H
#define HERODIARY_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct DiaryEntry {
    int day;
    std::string timeOfDay;
    std::string text;
    sf::Color color;

    DiaryEntry() : day(0), color(sf::Color::White) {}
    DiaryEntry(int d, const std::string& tod, const std::string& t, sf::Color c = sf::Color::White)
        : day(d), timeOfDay(tod), text(t), color(c) {}
};

class HeroDiary {
private:
    bool visible;
    int scrollOffset;
    int maxVisible;

    sf::Font font;
    sf::RectangleShape panelBg;
    sf::RectangleShape panelBorder;
    sf::Text titleText;
    sf::Text scrollHint;
    sf::Text dayText;

    std::vector<DiaryEntry> entries;

public:
    HeroDiary();
    ~HeroDiary() = default;

    bool initialize(sf::Font& f);
    void shutdown();

    void addEntry(int day, const std::string& timeOfDay, const std::string& text,
                  sf::Color color = sf::Color::White);

    void update(float deltaTime);
    void draw(sf::RenderTarget& target);
    void handleEvent(const sf::Event& event);

    void toggle();
    void show();
    void hide();
    bool isVisible() const;

    int getEntryCount() const;
    const DiaryEntry& getEntry(int index) const;

    HeroDiary(const HeroDiary&) = delete;
    HeroDiary& operator=(const HeroDiary&) = delete;
};

#endif // HERODIARY_H
