// Graphics/SpriteSystem.h
#ifndef SPRITESYSTEM_H
#define SPRITESYSTEM_H

#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <memory>

enum class SpriteType {
    PLAYER_IDLE,
    PLAYER_WALK,
    PLAYER_ATTACK,
    PLAYER_HURT,
    WOLF_IDLE,
    WOLF_WALK,
    WOLF_ATTACK,
    BEAR_IDLE,
    BEAR_WALK,
    BEAR_ATTACK,
    SKELETON_IDLE,
    SKELETON_WALK,
    SKELETON_ATTACK,
    NPC_MERCHANT,
    NPC_QUEST_GIVER,
    TILE_GRASS,
    TILE_TREE,
    TILE_WATER,
    TILE_STONE,
    TILE_DIRT
};

struct SpriteFrame {
    sf::Texture texture;
    sf::IntRect rect;
    float duration;
};

class SpriteSystem {
private:
    std::unordered_map<SpriteType, std::vector<SpriteFrame>> animations;
    std::unordered_map<SpriteType, sf::Texture> textures;

    int tileSize;
    int entitySize;

    bool loadTexture(const std::string& path, SpriteType type);
    void generatePlaceholderTextures();
    sf::Color getPlaceholderColor(SpriteType type) const;

public:
    SpriteSystem();
    ~SpriteSystem() = default;

    bool initialize();
    void loadAllSprites();

    const sf::Texture& getTexture(SpriteType type) const;
    sf::Sprite getSprite(SpriteType type, int frame = 0) const;
    sf::IntRect getTextureRect(SpriteType type, int frame = 0) const;

    int getTileSize() const;
    int getEntitySize() const;

    void drawEntity(sf::RenderTarget& target, SpriteType type, int x, int y, int frame = 0) const;
    void drawTile(sf::RenderTarget& target, SpriteType type, int x, int y) const;

    SpriteSystem(const SpriteSystem&) = delete;
    SpriteSystem& operator=(const SpriteSystem&) = delete;
};

#endif // SPRITESYSTEM_H
