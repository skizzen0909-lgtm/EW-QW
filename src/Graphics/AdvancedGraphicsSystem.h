// Graphics/AdvancedGraphicsSystem.h
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include "Types/Types.h"
#include "World/DayNightSystem.h"
#include "UISystem.h"

// Forward declarations
class Entity;
class TileBasedLocation;
class TileMap;
class ChunkSystem;

// ����������� ����� ����������
enum class RenderLayer {
    BACKGROUND,
    TERRAIN,
    OBJECTS,
    ENTITIES,
    EFFECTS,
    UI
};

// ����� ��� ���������� �����������
class AdvancedGraphicsSystem {
private:
    sf::RenderWindow window;
    std::unordered_map<RenderLayer, std::unique_ptr<sf::RenderTexture>> layerTextures;
    std::unordered_map<RenderLayer, sf::Sprite> layerSprites;
    std::unordered_map<std::string, sf::Texture> textureCache;
    std::unordered_map<std::string, std::unique_ptr<sf::Shader>> shaderCache;
    std::unordered_map<std::string, sf::Font> fontCache;
    sf::View gameView;
    sf::View uiView;
    bool initialized = false;
    float time = 0.0f;
    std::unique_ptr<UISystem> uiSystem;
    std::function<void(sf::RenderWindow&)> postCompositeCallback;
    // --- ������ ��� �������� �������� ---
    void loadTextures();
    void generatePlaceholderTextures();
    void loadShaders();
    void loadFonts();
    void createLayerTextures();
    // ������ ��� ���������� ������� ����
    void renderBackgroundLayer();
    void renderTerrainLayer(const TileMap* tileMap);
    void renderObjectsLayer(const TileMap* tileMap);
    void renderEntitiesLayer(const std::vector<std::shared_ptr<Entity>>& entities);
    void renderEffectsLayer();
    
public:
    AdvancedGraphicsSystem();
    ~AdvancedGraphicsSystem();
    bool initialize();
    void shutdown();
    void beginFrame();
    void renderWorld(const std::shared_ptr<Entity>& player,
        const std::vector<std::shared_ptr<Entity>>& entities,
        const TileBasedLocation* location);
    void renderWorld(const std::shared_ptr<Entity>& player,
        const std::vector<std::shared_ptr<Entity>>& entities,
        const TileMap* tileMap);
    void renderFromChunks(const std::shared_ptr<Entity>& player,
        const std::vector<std::shared_ptr<Entity>>& entities,
        const ChunkSystem& chunks);
    void renderDayNightOverlay(TimeOfDay time, WeatherType weather);
    void renderLootDrops(const std::vector<std::pair<float, float>>& lootPositions);
    void endFrame();
    void handleEvents();
    bool isWindowOpen() const;
    sf::RenderWindow& getWindow();
    sf::View& getGameView();
    // ������ ��� ������ � �������
    void setCameraPosition(float x, float y);
    void setCameraZoom(float zoom);
    void setCameraRotation(float rotation);
    // ������ ��� ������ � ����������
    sf::Texture* getTexture(const std::string& name);
    bool loadTexture(const std::string& name, const std::string& path);
    // ������ ��� ������ � ���������
    sf::Shader* getShader(const std::string& name);
    bool loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    // ������ ��� ������ � ��������
    sf::Font* getFont(const std::string& name);
    bool loadFont(const std::string& name, const std::string& path);
    // ������ ��� ������ � ���������
    void addParticleEffect(const sf::Vector2f& position, const std::string& effectType, int count);
    void updateParticleEffects(float deltaTime);
    void setPostCompositeCallback(std::function<void(sf::RenderWindow&)> cb) { postCompositeCallback = std::move(cb); }
    // ���������� ��������
    void applyLighting();
    void applyPostProcessing();
};