// GameState/EnhancedPlayState.h
#pragma once

#include "GameState.h"
#include "Entities/Player.h"
#include "World/GameWorld.h"
#include "Graphics/UISystem.h"
#include "Graphics/SpriteSystem.h"
#include "Utils/PerlinNoise.h"
#include "World/TileMap.h"
#include "World/ChunkSystem.h"
#include "World/SeasonSystem.h"
#include "World/DayNightSystem.h"
#include "Systems/CombatSystem.h"
#include "Systems/CraftingSystem.h"
#include "Systems/TradeSystem.h"
#include "Systems/DivineSystem.h"
#include "AI/HeroAI.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <SFML/Window/Event.hpp>

class AIBase;

class EnhancedPlayState : public GameState {
private:
    std::unordered_map<std::string, std::unique_ptr<AIBase>> aiBehaviors;
    std::shared_ptr<Player> player;
    std::unique_ptr<GameWorld> gameWorld;
    std::unique_ptr<UISystem> uiSystem;
    std::unique_ptr<SpriteSystem> spriteSystem;
    float timeAccumulator = 0.0f;
    bool paused = false;

    ChunkSystem chunkSystem;
    SeasonSystem seasonSystem;
    DayNightSystem dayNightSystem;
    CombatSystem combatSystem;
    CraftingSystem craftingSystem;
    TradeSystem tradeSystem;
    DivineSystem divineSystem;
    LootSystem lootSystem;
    HeroAI heroAI;

    std::vector<std::shared_ptr<Entity>> worldEntities;
    float respawnTimer = 0.0f;

    void initializeAI();
    void handlePlayerInput();
    void updateChunksAroundPlayer();
    void updateDivineAbilities();
    void updateWorldEntities(float deltaTime);
    void spawnEntitiesInChunk(int chunkX, int chunkY);
    void renderDivineUI();

    std::unique_ptr<PerlinNoise> baseNoise;
public:
    EnhancedPlayState();
    ~EnhancedPlayState() override;
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void handleInput() override;
    void render() override;
    void handleEvent(const sf::Event& event);
    void togglePause();
    void showInventory();
    void showSkills();
    void showMap();
    void showCrafting();
    void showTrade(const std::string& npcId);
    void showDivinePowers();

    CombatSystem& getCombatSystem();
    CraftingSystem& getCraftingSystem();
    TradeSystem& getTradeSystem();
    SeasonSystem& getSeasonSystem();
    DayNightSystem& getDayNightSystem();
    ChunkSystem& getChunkSystem();
    DivineSystem& getDivineSystem();
    HeroAI& getHeroAI();
};
