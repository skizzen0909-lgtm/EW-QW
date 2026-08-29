#include "UI/MinimapSystem.h"
#include "Entities/Entity.h"
#include "World/ChunkSystem.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

MinimapSystem::MinimapSystem()
    : visible(false)
    , mapSize(150)
    , viewRadius(50)
    , dotSize(2.0f)
    , mapX(0)
    , mapY(0) {
}

bool MinimapSystem::initialize(sf::Font& f) {
    font = f;
    setupShapes();
    std::cout << "[MINIMAP] Initialized" << std::endl;
    return true;
}

void MinimapSystem::setupShapes() {
    mapBg.setSize(sf::Vector2f(static_cast<float>(mapSize), static_cast<float>(mapSize)));
    mapBg.setFillColor(sf::Color(10, 15, 25, 200));
    mapBg.setOutlineThickness(2);
    mapBg.setOutlineColor(sf::Color(80, 120, 180, 180));
    mapBg.setPosition(mapX, mapY);

    mapBorder.setSize(sf::Vector2f(static_cast<float>(mapSize + 4), static_cast<float>(mapSize + 4)));
    mapBorder.setFillColor(sf::Color::Transparent);
    mapBorder.setOutlineThickness(1);
    mapBorder.setOutlineColor(sf::Color(60, 100, 160, 120));
    mapBorder.setPosition(mapX - 2, mapY - 2);

    mapTitle.setFont(font);
    mapTitle.setCharacterSize(10);
    mapTitle.setFillColor(sf::Color(150, 180, 220));
    mapTitle.setPosition(mapX + 4, mapY + 2);

    coordText.setFont(font);
    coordText.setCharacterSize(8);
    coordText.setFillColor(sf::Color(120, 140, 160));
    coordText.setPosition(mapX + 4, mapY + mapSize - 12);

    playerDot.setRadius(dotSize + 1.0f);
    playerDot.setFillColor(sf::Color::Green);
    playerDot.setOrigin(dotSize + 1.0f, dotSize + 1.0f);

    heroDot.setRadius(dotSize);
    heroDot.setFillColor(sf::Color(100, 180, 255));
    heroDot.setOrigin(dotSize, dotSize);

    legendBg.setSize(sf::Vector2f(130, 70));
    legendBg.setFillColor(sf::Color(10, 15, 25, 180));
    legendBg.setOutlineThickness(1);
    legendBg.setOutlineColor(sf::Color(60, 80, 120, 120));
    legendBg.setPosition(mapX, mapY + mapSize + 8);

    legendPlayer.setFont(font);
    legendPlayer.setCharacterSize(8);
    legendPlayer.setFillColor(sf::Color::Green);
    legendPlayer.setPosition(mapX + 14, mapY + mapSize + 12);
    legendPlayer.setString("Игрок");

    legendHero.setFont(font);
    legendHero.setCharacterSize(8);
    legendHero.setFillColor(sf::Color(100, 180, 255));
    legendHero.setPosition(mapX + 14, mapY + mapSize + 24);
    legendHero.setString("Герой");

    legendNPC.setFont(font);
    legendNPC.setCharacterSize(8);
    legendNPC.setFillColor(sf::Color::Yellow);
    legendNPC.setPosition(mapX + 14, mapY + mapSize + 36);
    legendNPC.setString("NPC");

    legendEnemy.setFont(font);
    legendEnemy.setCharacterSize(8);
    legendEnemy.setFillColor(sf::Color::Red);
    legendEnemy.setPosition(mapX + 14, mapY + mapSize + 48);
    legendEnemy.setString("Враг");

    legendResource.setFont(font);
    legendResource.setCharacterSize(8);
    legendResource.setFillColor(sf::Color(139, 90, 43));
    legendResource.setPosition(mapX + 70, mapY + mapSize + 12);
    legendResource.setString("Ресурс");
}

void MinimapSystem::update(float deltaTime, Entity* player, Entity* hero,
                           const std::vector<std::shared_ptr<Entity>>& entities,
                           ChunkSystem* chunks) {
    if (!visible || !player) return;

    auto [px, py] = player->getPosition();

    npcDots.clear();
    enemyDots.clear();
    resourceDots.clear();

    for (const auto& e : entities) {
        if (!e || !e->isAlive()) continue;
        auto [ex, ey] = e->getPosition();
        float relX = static_cast<float>(ex - px) / viewRadius;
        float relY = static_cast<float>(ey - py) / viewRadius;

        if (std::abs(relX) > 1.0f || std::abs(relY) > 1.0f) continue;

        float dotX = mapX + mapSize / 2.0f + relX * mapSize / 2.0f;
        float dotY = mapY + mapSize / 2.0f + relY * mapSize / 2.0f;

        std::string etype = e->getType();
        if (etype == "npc" || etype == "villager" || etype == "merchant") {
            sf::CircleShape dot(dotSize);
            dot.setFillColor(sf::Color::Yellow);
            dot.setOrigin(dotSize, dotSize);
            dot.setPosition(dotX, dotY);
            npcDots.push_back(dot);
        } else if (etype == "enemy" || etype == "wolf" || etype == "bear" || etype == "skeleton") {
            sf::CircleShape dot(dotSize);
            dot.setFillColor(sf::Color::Red);
            dot.setOrigin(dotSize, dotSize);
            dot.setPosition(dotX, dotY);
            enemyDots.push_back(dot);
        } else if (etype == "resource") {
            sf::CircleShape dot(dotSize - 0.5f);
            dot.setFillColor(sf::Color(139, 90, 43));
            dot.setOrigin(dotSize - 0.5f, dotSize - 0.5f);
            dot.setPosition(dotX, dotY);
            resourceDots.push_back(dot);
        }
    }

    playerDot.setPosition(mapX + mapSize / 2.0f, mapY + mapSize / 2.0f);

    if (hero) {
        auto [hx, hy] = hero->getPosition();
        float relHX = static_cast<float>(hx - px) / viewRadius;
        float relHY = static_cast<float>(hy - py) / viewRadius;
        relHX = std::max(-1.0f, std::min(1.0f, relHX));
        relHY = std::max(-1.0f, std::min(1.0f, relHY));
        heroDot.setPosition(mapX + mapSize / 2.0f + relHX * mapSize / 2.0f,
                            mapY + mapSize / 2.0f + relHY * mapSize / 2.0f);
    }

    std::ostringstream css;
    css << "[" << px << ", " << py << "]";
    coordText.setString(css.str());
}

void MinimapSystem::draw(sf::RenderTarget& target) {
    if (!visible) return;

    target.draw(mapBorder);
    target.draw(mapBg);

    for (const auto& dot : resourceDots) target.draw(dot);
    for (const auto& dot : enemyDots) target.draw(dot);
    for (const auto& dot : npcDots) target.draw(dot);

    target.draw(heroDot);
    target.draw(playerDot);

    target.draw(mapTitle);
    target.draw(coordText);
}

void MinimapSystem::drawLegend(sf::RenderTarget& target) {
    if (!visible) return;

    target.draw(legendBg);
    target.draw(legendPlayer);
    target.draw(legendHero);
    target.draw(legendNPC);
    target.draw(legendEnemy);
    target.draw(legendResource);
}

void MinimapSystem::toggle() { visible = !visible; }
void MinimapSystem::show() { visible = true; }
void MinimapSystem::hide() { visible = false; }
bool MinimapSystem::isVisible() const { return visible; }

void MinimapSystem::setMapSize(int size) {
    mapSize = size;
    setupShapes();
}

void MinimapSystem::setViewRadius(int radius) { viewRadius = radius; }
void MinimapSystem::setPosition(float x, float y) {
    mapX = x;
    mapY = y;
    setupShapes();
}
