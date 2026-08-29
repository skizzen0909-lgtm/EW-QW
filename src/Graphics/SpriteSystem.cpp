// Graphics/SpriteSystem.cpp
#include "SpriteSystem.h"
#include <iostream>
#include <sstream>

SpriteSystem::SpriteSystem()
    : tileSize(64)
    , entitySize(64) {
}

bool SpriteSystem::initialize() {
    loadAllSprites();
    generatePlaceholderTextures();
    return true;
}

void SpriteSystem::generatePlaceholderTextures() {
    int generated = 0;
    std::vector<SpriteType> allTypes = {
        SpriteType::PLAYER_IDLE, SpriteType::PLAYER_WALK, SpriteType::PLAYER_ATTACK, SpriteType::PLAYER_HURT,
        SpriteType::WOLF_IDLE, SpriteType::WOLF_WALK, SpriteType::WOLF_ATTACK,
        SpriteType::BEAR_IDLE, SpriteType::BEAR_WALK, SpriteType::BEAR_ATTACK,
        SpriteType::SKELETON_IDLE, SpriteType::SKELETON_WALK, SpriteType::SKELETON_ATTACK,
        SpriteType::NPC_MERCHANT, SpriteType::NPC_QUEST_GIVER,
        SpriteType::TILE_GRASS, SpriteType::TILE_TREE, SpriteType::TILE_WATER,
        SpriteType::TILE_STONE, SpriteType::TILE_DIRT
    };
    for (auto type : allTypes) {
        if (textures.find(type) != textures.end()) continue;
        sf::Texture texture;
        texture.create(entitySize, entitySize);
        sf::Uint8* pixels = new sf::Uint8[entitySize * entitySize * 4];
        sf::Color col = getPlaceholderColor(type);
        for (int y = 0; y < entitySize; y++) {
            for (int x = 0; x < entitySize; x++) {
                int idx = (y * entitySize + x) * 4;
                bool border = (x == 0 || x == entitySize - 1 || y == 0 || y == entitySize - 1);
                bool cross = (x == y || x == entitySize - 1 - y);
                if (border) {
                    pixels[idx] = 0; pixels[idx+1] = 0; pixels[idx+2] = 0; pixels[idx+3] = 255;
                } else if (cross) {
                    pixels[idx] = static_cast<sf::Uint8>(std::max(0, col.r - 40));
                    pixels[idx+1] = static_cast<sf::Uint8>(std::max(0, col.g - 40));
                    pixels[idx+2] = static_cast<sf::Uint8>(std::max(0, col.b - 40));
                    pixels[idx+3] = 200;
                } else {
                    pixels[idx] = col.r; pixels[idx+1] = col.g; pixels[idx+2] = col.b; pixels[idx+3] = 255;
                }
            }
        }
        texture.update(pixels);
        delete[] pixels;
        texture.setSmooth(false);
        textures[type] = std::move(texture);
        generated++;
    }
    if (generated > 0) std::cout << "[SPRITES] Generated " << generated << " placeholder textures" << std::endl;
}

sf::Color SpriteSystem::getPlaceholderColor(SpriteType type) const {
    switch (type) {
    case SpriteType::PLAYER_IDLE: case SpriteType::PLAYER_WALK:
    case SpriteType::PLAYER_ATTACK: case SpriteType::PLAYER_HURT:
        return sf::Color(0, 180, 255);
    case SpriteType::WOLF_IDLE: case SpriteType::WOLF_WALK: case SpriteType::WOLF_ATTACK:
        return sf::Color(180, 100, 60);
    case SpriteType::BEAR_IDLE: case SpriteType::BEAR_WALK: case SpriteType::BEAR_ATTACK:
        return sf::Color(120, 70, 30);
    case SpriteType::SKELETON_IDLE: case SpriteType::SKELETON_WALK: case SpriteType::SKELETON_ATTACK:
        return sf::Color(220, 220, 200);
    case SpriteType::NPC_MERCHANT: case SpriteType::NPC_QUEST_GIVER:
        return sf::Color(255, 200, 50);
    case SpriteType::TILE_GRASS: return sf::Color(80, 170, 60);
    case SpriteType::TILE_TREE:  return sf::Color(30, 100, 30);
    case SpriteType::TILE_WATER: return sf::Color(40, 100, 200);
    case SpriteType::TILE_STONE: return sf::Color(160, 155, 150);
    case SpriteType::TILE_DIRT:  return sf::Color(140, 110, 70);
    default: return sf::Color(200, 200, 200);
    }
}

void SpriteSystem::loadAllSprites() {
    std::cout << "[SPRITES] Loading sprite textures..." << std::endl;

    loadTexture("assets/sprites/player_idle.png", SpriteType::PLAYER_IDLE);
    loadTexture("assets/sprites/player_walk.png", SpriteType::PLAYER_WALK);
    loadTexture("assets/sprites/player_attack.png", SpriteType::PLAYER_ATTACK);
    loadTexture("assets/sprites/player_hurt.png", SpriteType::PLAYER_HURT);

    loadTexture("assets/sprites/wolf_idle.png", SpriteType::WOLF_IDLE);
    loadTexture("assets/sprites/wolf_walk.png", SpriteType::WOLF_WALK);
    loadTexture("assets/sprites/wolf_attack.png", SpriteType::WOLF_ATTACK);

    loadTexture("assets/sprites/bear_idle.png", SpriteType::BEAR_IDLE);
    loadTexture("assets/sprites/bear_walk.png", SpriteType::BEAR_WALK);
    loadTexture("assets/sprites/bear_attack.png", SpriteType::BEAR_ATTACK);

    loadTexture("assets/sprites/skeleton_idle.png", SpriteType::SKELETON_IDLE);
    loadTexture("assets/sprites/skeleton_walk.png", SpriteType::SKELETON_WALK);
    loadTexture("assets/sprites/skeleton_attack.png", SpriteType::SKELETON_ATTACK);

    loadTexture("assets/sprites/npc_merchant.png", SpriteType::NPC_MERCHANT);
    loadTexture("assets/sprites/npc_quest_giver.png", SpriteType::NPC_QUEST_GIVER);

    loadTexture("assets/sprites/tile_grass.png", SpriteType::TILE_GRASS);
    loadTexture("assets/sprites/tile_tree.png", SpriteType::TILE_TREE);
    loadTexture("assets/sprites/tile_water.png", SpriteType::TILE_WATER);
    loadTexture("assets/sprites/tile_stone.png", SpriteType::TILE_STONE);
    loadTexture("assets/sprites/tile_dirt.png", SpriteType::TILE_DIRT);

    std::cout << "[SPRITES] Loaded " << textures.size() << " textures" << std::endl;
}

bool SpriteSystem::loadTexture(const std::string& path, SpriteType type) {
    sf::Texture texture;
    if (texture.loadFromFile(path)) {
        textures[type] = std::move(texture);
        return true;
    }
    std::cerr << "[SPRITES] Failed to load: " << path << std::endl;
    return false;
}

const sf::Texture& SpriteSystem::getTexture(SpriteType type) const {
    static sf::Texture empty;
    auto it = textures.find(type);
    return (it != textures.end()) ? it->second : empty;
}

sf::Sprite SpriteSystem::getSprite(SpriteType type, int frame) const {
    sf::Sprite sprite;
    auto it = textures.find(type);
    if (it != textures.end()) {
        sprite.setTexture(it->second);
        sf::IntRect rect(frame * entitySize, 0, entitySize, entitySize);
        sprite.setTextureRect(rect);
    }
    return sprite;
}

sf::IntRect SpriteSystem::getTextureRect(SpriteType type, int frame) const {
    return sf::IntRect(frame * entitySize, 0, entitySize, entitySize);
}

int SpriteSystem::getTileSize() const { return tileSize; }
int SpriteSystem::getEntitySize() const { return entitySize; }

void SpriteSystem::drawEntity(sf::RenderTarget& target, SpriteType type, int x, int y, int frame) const {
    sf::Sprite sprite = getSprite(type, frame);
    if (sprite.getTexture()) {
        sprite.setPosition(static_cast<float>(x), static_cast<float>(y));
        target.draw(sprite);
    } else {
        sf::RectangleShape rect(sf::Vector2f(static_cast<float>(entitySize), static_cast<float>(entitySize)));
        rect.setPosition(static_cast<float>(x), static_cast<float>(y));
        rect.setFillColor(getPlaceholderColor(type));
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(1.0f);
        target.draw(rect);
    }
}

void SpriteSystem::drawTile(sf::RenderTarget& target, SpriteType type, int x, int y) const {
    sf::Sprite sprite = getSprite(type, 0);
    if (sprite.getTexture()) {
        sprite.setPosition(static_cast<float>(x), static_cast<float>(y));
        sprite.setScale(
            static_cast<float>(tileSize) / entitySize,
            static_cast<float>(tileSize) / entitySize
        );
        target.draw(sprite);
    } else {
        sf::RectangleShape rect(sf::Vector2f(static_cast<float>(tileSize), static_cast<float>(tileSize)));
        rect.setPosition(static_cast<float>(x), static_cast<float>(y));
        rect.setFillColor(getPlaceholderColor(type));
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(1.0f);
        target.draw(rect);
    }
}
