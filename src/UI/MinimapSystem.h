#ifndef MINIMAPSYSTEM_H
#define MINIMAPSYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Entity;
class ChunkSystem;

struct MinimapDot {
    sf::Vector2f position;
    sf::Color color;
    std::string label;
};

class MinimapSystem {
private:
    bool visible;
    int mapSize;
    int viewRadius;
    float dotSize;
    float mapX;
    float mapY;

    sf::Font font;
    sf::RectangleShape mapBg;
    sf::RectangleShape mapBorder;
    sf::Text mapTitle;
    sf::Text coordText;

    sf::CircleShape playerDot;
    sf::CircleShape heroDot;
    std::vector<sf::CircleShape> npcDots;
    std::vector<sf::CircleShape> enemyDots;
    std::vector<sf::CircleShape> resourceDots;

    sf::RectangleShape legendBg;
    sf::Text legendPlayer;
    sf::Text legendHero;
    sf::Text legendNPC;
    sf::Text legendEnemy;
    sf::Text legendResource;

    void setupShapes();

public:
    MinimapSystem();
    ~MinimapSystem() = default;

    bool initialize(sf::Font& font);
    void shutdown();

    void update(float deltaTime, Entity* player, Entity* hero,
                const std::vector<std::shared_ptr<Entity>>& entities,
                ChunkSystem* chunks);

    void draw(sf::RenderTarget& target);
    void drawLegend(sf::RenderTarget& target);

    void toggle();
    void show();
    void hide();
    bool isVisible() const;

    void setMapSize(int size);
    void setViewRadius(int radius);
    void setPosition(float x, float y);

    MinimapSystem(const MinimapSystem&) = delete;
    MinimapSystem& operator=(const MinimapSystem&) = delete;
};

#endif // MINIMAPSYSTEM_H
