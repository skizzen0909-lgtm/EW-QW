#include "Graphics/AdvancedGraphicsSystem.h"
#include "World/TileMap.h"
#include "World/TileBasedLocation.h"
#include "World/ChunkSystem.h"
#include "World/DayNightSystem.h"
#include "Entities/Entity.h"
#include "Entities/Enemy.h"
#include "Entities/WorldResource.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <memory>
#include <unordered_set>
#include "UISystem.h"

// Конструктор
AdvancedGraphicsSystem::AdvancedGraphicsSystem()
    : window(sf::VideoMode(1024, 768), "Eternal Wanderer", sf::Style::Default),
    uiSystem(std::make_unique<UISystem>()) {
    std::cout << "Window created with size: "
        << window.getSize().x << "x" << window.getSize().y << std::endl;
}

// Деструктор
AdvancedGraphicsSystem::~AdvancedGraphicsSystem() {
    shutdown();
}

// Инициализация системы
bool AdvancedGraphicsSystem::initialize() {
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    // Инициализация видов
    gameView.setSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
    uiView.setSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
    uiView.setCenter(static_cast<float>(window.getSize().x) / 2, static_cast<float>(window.getSize().y) / 2);
    // Создаем текстуры для каждого слоя
    createLayerTextures();
    // Загружаем текстуры, шейдеры и шрифты
    loadTextures();
    generatePlaceholderTextures();
    loadShaders();
    loadFonts();
    initialized = true;
    std::cout << "AdvancedGraphicsSystem initialized successfully." << std::endl;
    return true;
}

// Создание текстур для слоев
void AdvancedGraphicsSystem::createLayerTextures() {
    sf::Vector2u windowSize = window.getSize();
    for (int layer = static_cast<int>(RenderLayer::BACKGROUND);
        layer <= static_cast<int>(RenderLayer::UI); ++layer) {
        RenderLayer layerType = static_cast<RenderLayer>(layer);
        auto texture = std::make_unique<sf::RenderTexture>();
        if (texture->create(windowSize.x, windowSize.y)) {
            layerTextures[layerType] = std::move(texture);
            layerSprites[layerType] = sf::Sprite(layerTextures[layerType]->getTexture());
        }
        else {
            std::cerr << "Failed to create render texture for layer: " << layer << std::endl;
        }
    }
}

// Загрузка текстур
void AdvancedGraphicsSystem::loadTextures() {
    // Загрузка текстур для тайлов (БЕЗ сглаживания)
    std::vector<std::pair<std::string, std::string>> tilePaths = {
        {"ground_1", "assets/textures/tiles/base/ground_1.png"},
        {"ground_2", "assets/textures/tiles/base/ground_2.png"},
        {"ground_3", "assets/textures/tiles/base/ground_3.png"},
        {"ground_4", "assets/textures/tiles/base/ground_4.png"},
        {"water", "assets/textures/tiles/base/water.png"},
        {"desert", "assets/textures/tiles/base/desert.png"},
        {"road", "assets/textures/tiles/object/road_segment.png"},
        {"building", "assets/textures/tiles/base/building.png"},
        {"mountain", "assets/textures/tiles/base/mountain.png"},
        {"void", "assets/textures/tiles/base/void.png"},
        {"tree", "assets/textures/tiles/object/tree.png"},
        {"grass_patch", "assets/textures/tiles/decor/grass_patch.png"},
        {"house", "assets/textures/tiles/object/house.png"},
        {"stone", "assets/textures/tiles/decor/stone.png"},
        {"flower", "assets/textures/tiles/decor/flower.png"},
        {"ruin", "assets/textures/tiles/decor/ruin.png"},
        {"spawn", "assets/textures/tiles/base/spawn.png"},
        {"snow", "assets/textures/tiles/base/snow.png"},
        {"sand", "assets/textures/tiles/base/sand.png"},
        {"grass", "assets/textures/tiles/base/grass.png"},
        {"rock", "assets/textures/tiles/base/rock.png"}
    };
    for (const auto& path : tilePaths) {
        sf::Texture texture;
        if (texture.loadFromFile(path.second)) {
            texture.setSmooth(false); // КЛЮЧЕВАЯ СТРОКА ДЛЯ ТАЙЛОВ
            textureCache[path.first] = std::move(texture);
            std::cout << "Loaded texture: " << path.first << " from " << path.second << std::endl;
        }
        else {
            std::cerr << "Failed to load texture: " << path.first << " from " << path.second << std::endl;
        }
    }

    // Загрузка текстур для сущностей (БЕЗ сглаживания)
    std::vector<std::pair<std::string, std::string>> entityPaths = {
        {"player", "assets/textures/entities/player.png"},
        {"wolf", "assets/textures/entities/wolf.png"},
        {"bear", "assets/textures/entities/bear.png"},
        {"skeleton", "assets/textures/entities/skeleton.png"},
        {"enemy", "assets/textures/entities/enemy.png"},
        {"npc", "assets/textures/entities/npc.png"},
        {"villager", "assets/textures/entities/villager.png"}
    };
    for (const auto& path : entityPaths) {
        sf::Texture texture;
        if (texture.loadFromFile(path.second)) {
            texture.setSmooth(false); // 🔥 КЛЮЧЕВАЯ СТРОКА ДЛЯ СПРАЙТОВ
            textureCache[path.first] = std::move(texture);
            std::cout << "Loaded texture: " << path.first << " from " << path.second << std::endl;
        }
        else {
            std::cerr << "Failed to load texture: " << path.first << " from " << path.second << std::endl;
        }
    }

    // Загрузка текстур для частиц и эффектов (СО сглаживанием, если нужно)
    std::vector<std::string> effectPaths = {
        "assets/textures/effects/fire.png",
        "assets/textures/effects/smoke.png",
        "assets/textures/effects/magic.png"
    };
    for (size_t i = 0; i < effectPaths.size(); ++i) {
        sf::Texture texture;
        if (texture.loadFromFile(effectPaths[i])) {
            // Для эффектов можно оставить сглаживание (true) или отключить (false) — на ваше усмотрение
            texture.setSmooth(true); // Например, для плавных огненных частиц
            textureCache["effect_" + std::to_string(i)] = std::move(texture);
            std::cout << "Loaded effect texture: " << effectPaths[i] << std::endl;
        }
        else {
            std::cerr << "Failed to load effect texture: " << effectPaths[i] << std::endl;
        }
    }
}

void AdvancedGraphicsSystem::generatePlaceholderTextures() {
    struct PlaceholderDef { std::string key; sf::Color color; int size; };
    std::vector<PlaceholderDef> placeholders = {
        {"ground_1", sf::Color(120, 160, 60), 64},
        {"ground_2", sf::Color(115, 155, 58), 64},
        {"ground_3", sf::Color(110, 150, 55), 64},
        {"ground_4", sf::Color(125, 165, 65), 64},
        {"water",    sf::Color(40, 100, 200), 64},
        {"desert",   sf::Color(210, 180, 100), 64},
        {"sand",     sf::Color(230, 210, 150), 64},
        {"grass",    sf::Color(80, 170, 60), 64},
        {"snow",     sf::Color(230, 235, 245), 64},
        {"rock",     sf::Color(130, 125, 120), 64},
        {"mountain", sf::Color(100, 95, 90), 64},
        {"road",     sf::Color(160, 150, 130), 64},
        {"building", sf::Color(120, 110, 100), 64},
        {"void",     sf::Color(30, 30, 30), 64},
        {"spawn",    sf::Color(100, 200, 100), 64},
        {"tree",     sf::Color(30, 100, 30), 64},
        {"house",    sf::Color(160, 120, 60), 64},
        {"stone",    sf::Color(160, 155, 150), 64},
        {"grass_patch", sf::Color(90, 180, 70), 64},
        {"flower",   sf::Color(220, 80, 120), 64},
        {"ruin",     sf::Color(150, 130, 100), 64},
        {"player",   sf::Color(0, 180, 255), 64},
        {"wolf",     sf::Color(120, 120, 130), 64},
        {"bear",     sf::Color(100, 70, 40), 64},
        {"skeleton", sf::Color(200, 200, 210), 64},
        {"enemy",    sf::Color(180, 50, 50), 64},
        {"npc",      sf::Color(255, 200, 50), 64},
        {"villager", sf::Color(180, 220, 80), 64},
        {"effect_0", sf::Color(255, 100, 0), 32},
        {"effect_1", sf::Color(120, 120, 120), 32},
        {"effect_2", sf::Color(100, 50, 200), 32},
    };

    int generated = 0;
    for (const auto& ph : placeholders) {
        if (textureCache.find(ph.key) != textureCache.end()) continue;

        sf::Texture texture;
        texture.create(ph.size, ph.size);
        sf::Uint8* pixels = new sf::Uint8[ph.size * ph.size * 4];

        for (int y = 0; y < ph.size; y++) {
            for (int x = 0; x < ph.size; x++) {
                int idx = (y * ph.size + x) * 4;
                int cx = ph.size / 2, cy = ph.size / 2;
                bool inside = false;

                if (ph.key == "wolf") {
                    int dx = std::abs(x - cx), dy = std::abs(y - cy);
                    inside = (dx + dy * 2) < (ph.size / 2 - 2);
                } else if (ph.key == "bear") {
                    int dx = x - cx, dy = y - cy;
                    inside = (dx * dx + dy * dy) < ((ph.size / 2 - 4) * (ph.size / 2 - 4));
                } else if (ph.key == "skeleton") {
                    bool body = (std::abs(x - cx) < 4 && std::abs(y - cy) < (ph.size / 2 - 2));
                    bool arms = (std::abs(y - (cy - 6)) < 3 && std::abs(x - cx) < (ph.size / 2 - 6));
                    inside = body || arms;
                } else if (ph.key == "player") {
                    int dx = std::abs(x - cx), dy = std::abs(y - cy);
                    inside = dx < 12 && dy < 18;
                } else {
                    bool isBorder = (x == 0 || x == ph.size - 1 || y == 0 || y == ph.size - 1);
                    bool isDiag1 = (x == y);
                    bool isDiag2 = (x == ph.size - 1 - y);
                    inside = !(isBorder || isDiag1 || isDiag2);
                }

                if (ph.key == "wolf" || ph.key == "bear" || ph.key == "skeleton" || ph.key == "player") {
                    if (inside) {
                        pixels[idx]     = ph.color.r;
                        pixels[idx + 1] = ph.color.g;
                        pixels[idx + 2] = ph.color.b;
                        pixels[idx + 3] = 255;
                    } else {
                        pixels[idx] = 0; pixels[idx+1] = 0; pixels[idx+2] = 0; pixels[idx+3] = 0;
                    }
                } else {
                    bool isBorder = (x == 0 || x == ph.size - 1 || y == 0 || y == ph.size - 1);
                    bool isDiag1 = (x == y);
                    bool isDiag2 = (x == ph.size - 1 - y);
                    if (isBorder) {
                        pixels[idx] = 0; pixels[idx+1] = 0; pixels[idx+2] = 0; pixels[idx+3] = 255;
                    } else if (isDiag1 || isDiag2) {
                        pixels[idx]     = static_cast<sf::Uint8>(std::max(0, ph.color.r - 40));
                        pixels[idx + 1] = static_cast<sf::Uint8>(std::max(0, ph.color.g - 40));
                        pixels[idx + 2] = static_cast<sf::Uint8>(std::max(0, ph.color.b - 40));
                        pixels[idx + 3] = 200;
                    } else {
                        pixels[idx]     = ph.color.r;
                        pixels[idx + 1] = ph.color.g;
                        pixels[idx + 2] = ph.color.b;
                        pixels[idx + 3] = 255;
                    }
                }
            }
        }

        texture.update(pixels);
        delete[] pixels;
        texture.setSmooth(false);
        textureCache[ph.key] = std::move(texture);
        generated++;
    }
    if (generated > 0) {
        std::cout << "[TEXTURES] Generated " << generated << " placeholder textures" << std::endl;
    }
}

// Загрузка шейдеров
void AdvancedGraphicsSystem::loadShaders() {
    // Загрузка шейдера освещения
    auto lightShader = std::make_unique<sf::Shader>();
    if (lightShader->loadFromFile("assets/shaders/lighting.vert", "assets/shaders/lighting.frag")) {
        shaderCache["lighting"] = std::move(lightShader);
        std::cout << "Lighting shader loaded successfully." << std::endl;
    }
    else {
        std::cerr << "Warning: Failed to load lighting shader!" << std::endl;
    }
    // Загрузка шейдера пост-обработки
    auto postProcessShader = std::make_unique<sf::Shader>();
    if (postProcessShader->loadFromFile("assets/shaders/postprocess.vert", "assets/shaders/postprocess.frag")) {
        shaderCache["postprocess"] = std::move(postProcessShader);
        std::cout << "Post-process shader loaded successfully." << std::endl;
    }
    else {
        std::cerr << "Warning: Failed to load post-process shader!" << std::endl;
    }
}

// Загрузка шрифтов
void AdvancedGraphicsSystem::loadFonts() {
    // Загрузка шрифтов
    std::vector<std::pair<std::string, std::string>> fontPaths = {
        {"arial", "assets/fonts/arial.ttf"},
        {"consolas", "assets/fonts/consolas.ttf"},
        {"times", "assets/fonts/times.ttf"}
    };
    for (const auto& path : fontPaths) {
        sf::Font font;
        if (font.loadFromFile(path.second)) {
            fontCache[path.first] = std::move(font);
            std::cout << "Loaded font: " << path.first << " from " << path.second << std::endl;
        }
        else {
            std::cerr << "Failed to load font: " << path.first << " from " << path.second << std::endl;
        }
    }
}

// Завершение работы системы
void AdvancedGraphicsSystem::shutdown() {
    if (initialized) {
        window.close();
        textureCache.clear();
        shaderCache.clear();
        fontCache.clear();
        layerTextures.clear();
        layerSprites.clear();
        initialized = false;
        std::cout << "AdvancedGraphicsSystem shut down." << std::endl;
    }
}

// Начало кадра
void AdvancedGraphicsSystem::beginFrame() {
    if (!initialized) {
        std::cerr << "AdvancedGraphicsSystem not initialized!" << std::endl;
        return;
    }
    // Очищаем все слои
    for (auto& layer : layerTextures) {
        layer.second->clear(sf::Color::Transparent); // Используем -> вместо .
    }
}

// Рендеринг мира
void AdvancedGraphicsSystem::renderWorld(const std::shared_ptr<Entity>& player,
    const std::vector<std::shared_ptr<Entity>>& entities,
    const TileBasedLocation* location) {
    if (!location || !initialized) {
        std::cerr << "renderWorld: location is null or system not initialized" << std::endl;
        return;
    }
    const TileMap* tileMap = location->getTileMap();
    if (!tileMap) {
        std::cerr << "renderWorld: tileMap is null" << std::endl;
        return;
    }
    // Обновляем камеру
    if (player) {
        auto pos = player->getPosition();
        float pixelX = static_cast<float>(pos.first) * 64.0f + 32.0f;
        float pixelY = static_cast<float>(pos.second) * 64.0f + 32.0f;
        gameView.setCenter(pixelX, pixelY);
    }
    // Рендерим каждый слой
    renderBackgroundLayer();
    renderTerrainLayer(tileMap);
    renderObjectsLayer(tileMap);
    renderEntitiesLayer(entities);
    renderEffectsLayer();
}

void AdvancedGraphicsSystem::renderWorld(const std::shared_ptr<Entity>& player,
    const std::vector<std::shared_ptr<Entity>>& entities,
    const TileMap* tileMap) {
    if (!tileMap || !initialized) return;
    if (player) {
        auto pos = player->getPosition();
        float pixelX = static_cast<float>(pos.first) * 64.0f + 32.0f;
        float pixelY = static_cast<float>(pos.second) * 64.0f + 32.0f;
        gameView.setCenter(pixelX, pixelY);
    }
    renderBackgroundLayer();
    renderTerrainLayer(tileMap);
    renderObjectsLayer(tileMap);
    renderEntitiesLayer(entities);
    renderEffectsLayer();
}

void AdvancedGraphicsSystem::renderFromChunks(const std::shared_ptr<Entity>& player,
    const std::vector<std::shared_ptr<Entity>>& entities,
    const ChunkSystem& chunks) {
    if (!initialized) return;
    if (player) {
        auto pos = player->getPosition();
        float pixelX = static_cast<float>(pos.first) * 64.0f + 32.0f;
        float pixelY = static_cast<float>(pos.second) * 64.0f + 32.0f;
        gameView.setCenter(pixelX, pixelY);
    }
    renderBackgroundLayer();

    sf::RenderTexture& terrainTex = *layerTextures[RenderLayer::TERRAIN];
    sf::RenderTexture& objectsTex = *layerTextures[RenderLayer::OBJECTS];
    terrainTex.setView(gameView);
    objectsTex.setView(gameView);

    sf::Vector2f viewCenter = gameView.getCenter();
    sf::Vector2f viewSize = gameView.getSize();
    int startX = static_cast<int>((viewCenter.x - viewSize.x / 2.0f) / 64.0f) - 1;
    int startY = static_cast<int>((viewCenter.y - viewSize.y / 2.0f) / 64.0f) - 1;
    int endX = static_cast<int>((viewCenter.x + viewSize.x / 2.0f) / 64.0f) + 2;
    int endY = static_cast<int>((viewCenter.y + viewSize.y / 2.0f) / 64.0f) + 2;

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            const TileLayer* layer = chunks.getLayerAt(x, y);
            if (!layer) continue;
            float fx = static_cast<float>(x) * 64.0f;
            float fy = static_cast<float>(y) * 64.0f;

            std::string textureName;
            TileType baseType = layer->baseType;
            if (baseType == TileType::GROUND) {
                int variant = std::clamp(layer->groundVariant, 0, 3);
                textureName = "ground_" + std::to_string(variant + 1);
            } else {
                switch (baseType) {
                case TileType::WATER:    textureName = "water";    break;
                case TileType::DESERT:   textureName = "desert";   break;
                case TileType::GRASS:    textureName = "grass";    break;
                case TileType::SAND:     textureName = "sand";     break;
                case TileType::SNOW:     textureName = "snow";     break;
                case TileType::ROCK:     textureName = "rock";     break;
                case TileType::MOUNTAIN: textureName = "mountain"; break;
                case TileType::VOID:     textureName = "void";     break;
                case TileType::SPAWN:    textureName = "spawn";    break;
                default:                 textureName = "ground_1";  break;
                }
            }

            auto it = textureCache.find(textureName);
            if (it != textureCache.end()) {
                sf::Sprite sprite(it->second);
                sprite.setPosition(fx, fy);
                terrainTex.draw(sprite);
            } else {
                sf::RectangleShape rect(sf::Vector2f(64.0f, 64.0f));
                rect.setPosition(fx, fy);
                sf::Color tileColor;
                switch (baseType) {
                case TileType::GROUND:    tileColor = sf::Color(120, 160, 60); break;
                case TileType::WATER:     tileColor = sf::Color(40, 100, 200); break;
                case TileType::DESERT:    tileColor = sf::Color(210, 180, 100); break;
                case TileType::GRASS:     tileColor = sf::Color(80, 170, 60); break;
                case TileType::SAND:      tileColor = sf::Color(230, 210, 150); break;
                case TileType::SNOW:      tileColor = sf::Color(230, 235, 245); break;
                case TileType::ROCK:      tileColor = sf::Color(130, 125, 120); break;
                case TileType::MOUNTAIN:  tileColor = sf::Color(100, 95, 90); break;
                case TileType::VOID:      tileColor = sf::Color(30, 30, 30); break;
                case TileType::SPAWN:     tileColor = sf::Color(100, 200, 100); break;
                default:                  tileColor = sf::Color(120, 160, 60); break;
                }
                rect.setFillColor(tileColor);
                terrainTex.draw(rect);
            }

            if (layer->objectType != TileType::VOID) {
                std::string objName;
                switch (layer->objectType) {
                case TileType::TREE:         objName = "tree"; break;
                case TileType::HOUSE:        objName = "house"; break;
                case TileType::ROAD_SEGMENT: objName = "road"; break;
                case TileType::MOUNTAIN:     objName = "mountain"; break;
                case TileType::BUILDING:     objName = "building"; break;
                case TileType::RUIN:         objName = "ruin"; break;
                case TileType::STONE:        objName = "stone"; break;
                default: objName = ""; break;
                }
                auto oit = textureCache.find(objName);
                if (oit != textureCache.end()) {
                    sf::Sprite sprite(oit->second);
                    sprite.setPosition(fx, fy);
                    objectsTex.draw(sprite);
                } else if (!objName.empty()) {
                    sf::RectangleShape rect(sf::Vector2f(48.0f, 56.0f));
                    rect.setPosition(fx + 8.0f, fy + 4.0f);
                    sf::Color objColor;
                    switch (layer->objectType) {
                    case TileType::TREE:         objColor = sf::Color(30, 100, 30); break;
                    case TileType::HOUSE:        objColor = sf::Color(160, 120, 60); break;
                    case TileType::ROAD_SEGMENT: objColor = sf::Color(160, 150, 130); rect.setSize(sf::Vector2f(56.0f, 16.0f)); rect.setPosition(fx + 4.0f, fy + 24.0f); break;
                    case TileType::MOUNTAIN:     objColor = sf::Color(110, 105, 100); rect.setSize(sf::Vector2f(60.0f, 52.0f)); rect.setPosition(fx + 2.0f, fy + 8.0f); break;
                    case TileType::RUIN:         objColor = sf::Color(150, 130, 100); rect.setSize(sf::Vector2f(40.0f, 40.0f)); rect.setPosition(fx + 12.0f, fy + 12.0f); break;
                    case TileType::STONE:        objColor = sf::Color(160, 155, 150); rect.setSize(sf::Vector2f(32.0f, 28.0f)); rect.setPosition(fx + 16.0f, fy + 20.0f); break;
                    default:                     objColor = sf::Color(120, 120, 120); break;
                    }
                    rect.setFillColor(objColor);
                    rect.setOutlineColor(sf::Color(0, 0, 0, 80));
                    rect.setOutlineThickness(1.0f);
                    objectsTex.draw(rect);
                }
            }

            for (const auto& decType : layer->decorationLayers) {
                std::string decName;
                switch (decType) {
                case TileType::GRASS_PATCH: decName = "grass_patch"; break;
                case TileType::STONE:       decName = "stone"; break;
                case TileType::FLOWER:      decName = "flower"; break;
                default: continue;
                }
                auto dit = textureCache.find(decName);
                if (dit != textureCache.end()) {
                    sf::Sprite sprite(dit->second);
                    sprite.setPosition(fx, fy);
                    objectsTex.draw(sprite);
                } else {
                    sf::RectangleShape rect(sf::Vector2f(16.0f, 16.0f));
                    rect.setPosition(fx + 24.0f, fy + 40.0f);
                    sf::Color decColor;
                    switch (decType) {
                    case TileType::GRASS_PATCH: decColor = sf::Color(90, 180, 70); break;
                    case TileType::STONE:       decColor = sf::Color(160, 155, 150); break;
                    case TileType::FLOWER:      decColor = sf::Color(220, 80, 120); break;
                    default:                    decColor = sf::Color(150, 150, 150); break;
                    }
                    rect.setFillColor(decColor);
                    objectsTex.draw(rect);
                }
            }
        }
    }
    terrainTex.display();
    objectsTex.display();

    renderEntitiesLayer(entities);
    renderEffectsLayer();
}

// Рендеринг фона
void AdvancedGraphicsSystem::renderBackgroundLayer() {
    sf::RenderTexture& texture = *layerTextures[RenderLayer::BACKGROUND]; 
    texture.setView(gameView);
    sf::RectangleShape background(sf::Vector2f(40000, 40000)); 
    background.setFillColor(sf::Color(135, 206, 235)); 
    texture.draw(background);
    texture.display();
}

// Рендеринг terrain слоя
void AdvancedGraphicsSystem::renderTerrainLayer(const TileMap* tileMap) {
    // Исправлено: используем * для разыменования unique_ptr
    sf::RenderTexture& texture = *layerTextures[RenderLayer::TERRAIN];
    texture.setView(gameView);
    int width = tileMap->getWidth();
    int height = tileMap->getHeight();
    // Получаем границы видимой области
    sf::Vector2f viewCenter = gameView.getCenter();
    sf::Vector2f viewSize = gameView.getSize();
    int startX = static_cast<int>((viewCenter.x - viewSize.x / 2) / 64.0f);
    int startY = static_cast<int>((viewCenter.y - viewSize.y / 2) / 64.0f);
    int endX = static_cast<int>((viewCenter.x + viewSize.x / 2) / 64.0f) + 1;
    int endY = static_cast<int>((viewCenter.y + viewSize.y / 2) / 64.0f) + 1;
    // Ограничиваем границы
    startX = std::max(0, startX);
    startY = std::max(0, startY);
    endX = std::min(width, endX);
    endY = std::min(height, endY);

    // Рендерим тайлы
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            const TileLayer& layer = tileMap->getLayer(x, y);
            TileType baseType = layer.baseType;

            std::string textureName;
            if (baseType == TileType::GROUND) {
                int variant = tileMap->getLayer(x, y).groundVariant;
                variant = std::clamp(variant, 0, 3);
                textureName = "ground_" + std::to_string(variant + 1);
            }
            else {
                switch (baseType) {
                case TileType::WATER:    textureName = "water";    break;
                case TileType::DESERT:   textureName = "desert";   break;
                case TileType::GRASS:    textureName = "grass";    break;
                case TileType::SAND:     textureName = "sand";     break;
                case TileType::SNOW:     textureName = "snow";     break;
                case TileType::ROCK:     textureName = "rock";     break;
                case TileType::MOUNTAIN: textureName = "mountain"; break;
                case TileType::VOID:     textureName = "void";     break;
                case TileType::SPAWN:    textureName = "spawn";    break;
                default:                 textureName = "ground_1";  break;
                }
            }

            auto it = textureCache.find(textureName);
            if (it != textureCache.end()) {
                sf::Sprite sprite(it->second);
                sprite.setPosition(static_cast<float>(x) * 64.0f, static_cast<float>(y) * 64.0f);
                texture.draw(sprite);
            } else {
                sf::RectangleShape rect(sf::Vector2f(64.0f, 64.0f));
                rect.setPosition(static_cast<float>(x) * 64.0f, static_cast<float>(y) * 64.0f);
                sf::Color tileColor;
                switch (baseType) {
                case TileType::GROUND:    tileColor = sf::Color(120, 160, 60); break;
                case TileType::WATER:     tileColor = sf::Color(40, 100, 200); break;
                case TileType::DESERT:    tileColor = sf::Color(210, 180, 100); break;
                case TileType::GRASS:     tileColor = sf::Color(80, 170, 60); break;
                case TileType::SAND:      tileColor = sf::Color(230, 210, 150); break;
                case TileType::SNOW:      tileColor = sf::Color(230, 235, 245); break;
                case TileType::ROCK:      tileColor = sf::Color(130, 125, 120); break;
                case TileType::MOUNTAIN:  tileColor = sf::Color(100, 95, 90); break;
                case TileType::VOID:      tileColor = sf::Color(30, 30, 30); break;
                case TileType::SPAWN:     tileColor = sf::Color(100, 200, 100); break;
                default:                  tileColor = sf::Color(120, 160, 60); break;
                }
                rect.setFillColor(tileColor);
                texture.draw(rect);
            }
        }
    }
    texture.display();
}

// Рендеринг объектов слоя
void AdvancedGraphicsSystem::renderObjectsLayer(const TileMap* tileMap) {
    // Исправлено: используем * для разыменования unique_ptr
    sf::RenderTexture& texture = *layerTextures[RenderLayer::OBJECTS];
    texture.setView(gameView);
    int width = tileMap->getWidth();
    int height = tileMap->getHeight();
    // Получаем границы видимой области
    sf::Vector2f viewCenter = gameView.getCenter();
    sf::Vector2f viewSize = gameView.getSize();
    int startX = static_cast<int>((viewCenter.x - viewSize.x / 2) / 64.0f);
    int startY = static_cast<int>((viewCenter.y - viewSize.y / 2) / 64.0f);
    int endX = static_cast<int>((viewCenter.x + viewSize.x / 2) / 64.0f) + 1;
    int endY = static_cast<int>((viewCenter.y + viewSize.y / 2) / 64.0f) + 1;
    // Ограничиваем границы
    startX = std::max(0, startX);
    startY = std::max(0, startY);
    endX = std::min(width, endX);
    endY = std::min(height, endY);
    // Рендерим объекты
    for (int y = startY; y < endY; ++y) {
        for (int x = startX; x < endX; ++x) {
            const TileLayer& layer = tileMap->getLayer(x, y);
            // Рендерим объект
            if (layer.objectType != TileType::VOID) {
                std::string textureName;
                switch (layer.objectType) {
                case TileType::TREE: textureName = "tree"; break;
                case TileType::HOUSE: textureName = "house"; break;
                case TileType::ROAD_SEGMENT: textureName = "road"; break;
                case TileType::MOUNTAIN: textureName = "mountain"; break;
                case TileType::BUILDING: textureName = "building"; break;
                case TileType::RUIN: textureName = "ruin"; break;
                default: textureName = "ground_1"; break;
                }
                auto it = textureCache.find(textureName);
                if (it != textureCache.end()) {
                    sf::Sprite sprite(it->second);
                    sprite.setPosition(static_cast<float>(x) * 64.0f, static_cast<float>(y) * 64.0f);
                    texture.draw(sprite);
                } else {
                    sf::RectangleShape rect(sf::Vector2f(48.0f, 56.0f));
                    rect.setPosition(static_cast<float>(x) * 64.0f + 8.0f, static_cast<float>(y) * 64.0f + 4.0f);
                    sf::Color objColor;
                    switch (layer.objectType) {
                    case TileType::TREE:         objColor = sf::Color(30, 100, 30); break;
                    case TileType::HOUSE:        objColor = sf::Color(160, 120, 60); break;
                    case TileType::ROAD_SEGMENT: objColor = sf::Color(160, 150, 130); rect.setSize(sf::Vector2f(56.0f, 16.0f)); rect.setPosition(static_cast<float>(x)*64.0f+4.0f, static_cast<float>(y)*64.0f+24.0f); break;
                    case TileType::MOUNTAIN:     objColor = sf::Color(110, 105, 100); rect.setSize(sf::Vector2f(60.0f, 52.0f)); rect.setPosition(static_cast<float>(x)*64.0f+2.0f, static_cast<float>(y)*64.0f+8.0f); break;
                    case TileType::BUILDING:     objColor = sf::Color(120, 110, 100); break;
                    case TileType::RUIN:         objColor = sf::Color(150, 130, 100); rect.setSize(sf::Vector2f(40.0f, 40.0f)); rect.setPosition(static_cast<float>(x)*64.0f+12.0f, static_cast<float>(y)*64.0f+12.0f); break;
                    default:                     objColor = sf::Color(120, 120, 120); break;
                    }
                    rect.setFillColor(objColor);
                    rect.setOutlineColor(sf::Color(0, 0, 0, 80));
                    rect.setOutlineThickness(1.0f);
                    texture.draw(rect);
                }
            }
            // Рендерим декорации
            for (const auto& decType : layer.decorationLayers) {
                std::string textureName;
                switch (decType) {
                case TileType::GRASS_PATCH: textureName = "grass_patch"; break;
                case TileType::STONE: textureName = "stone"; break;
                case TileType::FLOWER: textureName = "flower"; break;
                default: continue;
                }
                auto it = textureCache.find(textureName);
                if (it != textureCache.end()) {
                    sf::Sprite sprite(it->second);
                    sprite.setPosition(static_cast<float>(x) * 64.0f, static_cast<float>(y) * 64.0f);
                    texture.draw(sprite);
                } else {
                    sf::RectangleShape rect(sf::Vector2f(16.0f, 16.0f));
                    rect.setPosition(static_cast<float>(x) * 64.0f + 24.0f, static_cast<float>(y) * 64.0f + 40.0f);
                    sf::Color decColor;
                    switch (decType) {
                    case TileType::GRASS_PATCH: decColor = sf::Color(90, 180, 70); break;
                    case TileType::STONE:       decColor = sf::Color(160, 155, 150); break;
                    case TileType::FLOWER:      decColor = sf::Color(220, 80, 120); break;
                    default:                    decColor = sf::Color(150, 150, 150); break;
                    }
                    rect.setFillColor(decColor);
                    texture.draw(rect);
                }
            }
        }
    }
    texture.display();
}

// Рендеринг сущностей
void AdvancedGraphicsSystem::renderEntitiesLayer(const std::vector<std::shared_ptr<Entity>>& entities) {
    sf::RenderTexture& texture = *layerTextures[RenderLayer::ENTITIES];
    texture.setView(gameView);

    sf::Vector2f viewCenter = gameView.getCenter();
    sf::Vector2f viewSize = gameView.getSize();
    float left = viewCenter.x - viewSize.x / 2.0f - 128.0f;
    float right = viewCenter.x + viewSize.x / 2.0f + 128.0f;
    float top = viewCenter.y - viewSize.y / 2.0f - 128.0f;
    float bottom = viewCenter.y + viewSize.y / 2.0f + 128.0f;

    for (const auto& entity : entities) {
        if (!entity || !entity->isAlive()) continue;
        auto pos = entity->getPosition();
        float entityX = static_cast<float>(pos.first) * 64.0f;
        float entityY = static_cast<float>(pos.second) * 64.0f;

        if (entityX < left || entityX > right || entityY < top || entityY > bottom) continue;

        std::string rk = entity->getRenderKey();
        std::string textureName;
        sf::Color fallbackColor;

        if (!rk.empty()) {
            if (rk == "player") { textureName = "player"; fallbackColor = sf::Color(0, 180, 255); }
            else if (rk == "wolf") { textureName = "wolf"; fallbackColor = sf::Color(120, 120, 130); }
            else if (rk == "bear") { textureName = "bear"; fallbackColor = sf::Color(100, 70, 40); }
            else if (rk == "skeleton") { textureName = "skeleton"; fallbackColor = sf::Color(200, 200, 210); }
            else if (rk == "npc") { textureName = "npc"; fallbackColor = sf::Color(255, 200, 50); }
            else if (rk == "villager") { textureName = "villager"; fallbackColor = sf::Color(180, 220, 80); }
            else { textureName = rk; fallbackColor = sf::Color(200, 200, 200); }
        } else {
            std::string type = entity->getType();
            if (type == "enemy") {
                textureName = "enemy";
                fallbackColor = sf::Color(180, 50, 50);
            } else if (type == "WorldResource") {
                textureName = "";
                fallbackColor = sf::Color(80, 160, 60);
            } else {
                textureName = "npc";
                fallbackColor = sf::Color(200, 200, 200);
            }
        }

        bool drawn = false;
        if (!textureName.empty()) {
            auto it = textureCache.find(textureName);
            if (it != textureCache.end()) {
                sf::Sprite sprite(it->second);
                sprite.setPosition(entityX, entityY);
                texture.draw(sprite);
                drawn = true;
            }
        }

        if (!drawn) {
            sf::RectangleShape rect(sf::Vector2f(48.0f, 48.0f));
            rect.setPosition(entityX + 8.0f, entityY + 8.0f);
            rect.setFillColor(fallbackColor);
            rect.setOutlineColor(sf::Color(0, 0, 0));
            rect.setOutlineThickness(1.5f);
            texture.draw(rect);

            if (entity->getType() == "WorldResource") {
                auto* wr = dynamic_cast<WorldResource*>(entity.get());
                if (wr) {
                    switch (wr->getResourceType()) {
                    case ResourceType::TREE:     fallbackColor = sf::Color(30, 120, 30); break;
                    case ResourceType::ROCK:     fallbackColor = sf::Color(140, 140, 140); break;
                    case ResourceType::BUSH:     fallbackColor = sf::Color(60, 180, 60); break;
                    case ResourceType::MUSHROOM: fallbackColor = sf::Color(200, 100, 50); break;
                    case ResourceType::CRYSTAL:  fallbackColor = sf::Color(120, 180, 255); break;
                    }
                    rect.setFillColor(fallbackColor);
                    texture.draw(rect);
                }
            }
        }

        if (entity->getType() == "enemy") {
            float hpRatio = static_cast<float>(entity->getHealth()) / entity->getMaxHealth();
            if (hpRatio < 1.0f) {
                sf::RectangleShape bgBar(sf::Vector2f(40.0f, 4.0f));
                bgBar.setPosition(entityX + 12.0f, entityY - 4.0f);
                bgBar.setFillColor(sf::Color(60, 60, 60));
                texture.draw(bgBar);

                sf::RectangleShape hpBar(sf::Vector2f(40.0f * hpRatio, 4.0f));
                hpBar.setPosition(entityX + 12.0f, entityY - 4.0f);
                hpBar.setFillColor(hpRatio > 0.5f ? sf::Color::Green :
                                   hpRatio > 0.25f ? sf::Color::Yellow : sf::Color::Red);
                texture.draw(hpBar);
            }
        }

        if (entity->getType() == "player") {
            float hpRatio = static_cast<float>(entity->getHealth()) / entity->getMaxHealth();
            sf::RectangleShape bgBar(sf::Vector2f(50.0f, 5.0f));
            bgBar.setPosition(entityX + 7.0f, entityY - 8.0f);
            bgBar.setFillColor(sf::Color(60, 60, 60));
            texture.draw(bgBar);

            sf::RectangleShape hpBar(sf::Vector2f(50.0f * hpRatio, 5.0f));
            hpBar.setPosition(entityX + 7.0f, entityY - 8.0f);
            hpBar.setFillColor(sf::Color(0, 200, 80));
            texture.draw(hpBar);
        }
    }
    texture.display();
}

// Рендеринг эффектов
void AdvancedGraphicsSystem::renderEffectsLayer() {
    // Исправлено: используем * для разыменования unique_ptr
    sf::RenderTexture& texture = *layerTextures[RenderLayer::EFFECTS];
    texture.setView(gameView);
    // Здесь будет рендер частиц и других эффектов
    // Для примера рисуем несколько частиц
    for (int i = 0; i < 10; ++i) {
        float x = gameView.getCenter().x + (rand() % 200 - 100);
        float y = gameView.getCenter().y + (rand() % 200 - 100);
        auto it = textureCache.find("effect_0");
        if (it != textureCache.end()) {
            sf::Sprite sprite(it->second);
            sprite.setPosition(x, y);
            sprite.setColor(sf::Color(255, 100, 0, 150));
            texture.draw(sprite);
        } else {
            sf::CircleShape circle(4.0f);
            circle.setPosition(x, y);
            circle.setFillColor(sf::Color(255, 100, 0, 150));
            texture.draw(circle);
        }
    }
    texture.display();
}

// Конец кадра
void AdvancedGraphicsSystem::endFrame() {
    if (!initialized) {
        std::cerr << "AdvancedGraphicsSystem not initialized!" << std::endl;
        return;
    }
    window.clear(sf::Color(30, 30, 40));
    sf::View defaultView = window.getDefaultView();
    window.setView(defaultView);
    for (int layer = static_cast<int>(RenderLayer::BACKGROUND);
        layer <= static_cast<int>(RenderLayer::EFFECTS); ++layer) {
        RenderLayer layerType = static_cast<RenderLayer>(layer);
        auto it = layerTextures.find(layerType);
        if (it != layerTextures.end()) {
            sf::Sprite sprite(it->second->getTexture());
            window.draw(sprite);
        }
    }
    if (postCompositeCallback) {
        postCompositeCallback(window);
    }
    window.setView(defaultView);
    window.display();
}

// Обработка событий
void AdvancedGraphicsSystem::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
        // Добавить обработку других событий
    }
}

// Проверка, открыто ли окно
bool AdvancedGraphicsSystem::isWindowOpen() const {
    return window.isOpen();
}

// Получение окна
sf::RenderWindow& AdvancedGraphicsSystem::getWindow() {
    return window;
}

// Получение игрового вида
sf::View& AdvancedGraphicsSystem::getGameView() {
    return gameView;
}

// Установка позиции камеры
void AdvancedGraphicsSystem::setCameraPosition(float x, float y) {
    gameView.setCenter(x, y);
}

// Установка зума камеры
void AdvancedGraphicsSystem::setCameraZoom(float zoom) {
    gameView.zoom(zoom);
}

// Установка вращения камеры
void AdvancedGraphicsSystem::setCameraRotation(float rotation) {
    gameView.setRotation(rotation);
}

// Получение текстуры
sf::Texture* AdvancedGraphicsSystem::getTexture(const std::string& name) {
    auto it = textureCache.find(name);
    if (it != textureCache.end()) {
        return &it->second;
    }
    return nullptr;
}

// Загрузка текстуры
bool AdvancedGraphicsSystem::loadTexture(const std::string& name, const std::string& path) {
    sf::Texture texture;
    if (texture.loadFromFile(path)) {
        texture.setSmooth(false); 
        textureCache[name] = std::move(texture);
        return true;
    }
    return false;
}

// Получение шейдера
sf::Shader* AdvancedGraphicsSystem::getShader(const std::string& name) {
    auto it = shaderCache.find(name);
    if (it != shaderCache.end() && it->second) {
        return it->second.get();
    }
    return nullptr;
}

// Загрузка шейдера
bool AdvancedGraphicsSystem::loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
    auto shader = std::make_unique<sf::Shader>();
    if (shader->loadFromFile(vertexPath, fragmentPath)) {
        shaderCache[name] = std::move(shader);
        return true;
    }
    return false;
}

// Получение шрифта
sf::Font* AdvancedGraphicsSystem::getFont(const std::string& name) {
    auto it = fontCache.find(name);
    if (it != fontCache.end()) {
        return &it->second;
    }
    return nullptr;
}

// Загрузка шрифта
bool AdvancedGraphicsSystem::loadFont(const std::string& name, const std::string& path) {
    sf::Font font;
    if (font.loadFromFile(path)) {
        fontCache[name] = std::move(font);
        return true;
    }
    return false;
}

// Добавление частиц
void AdvancedGraphicsSystem::addParticleEffect(const sf::Vector2f& position, const std::string& effectType, int count) {
    // Здесь будет реализация добавления частиц
    // Для примера просто выводим сообщение
    std::cout << "Added particle effect at " << position.x << ", " << position.y
        << " of type " << effectType << " with count " << count << std::endl;
}

// Обновление частиц
void AdvancedGraphicsSystem::updateParticleEffects(float deltaTime) {
    // Здесь будет реализация обновления частиц
}

// Применение освещения
void AdvancedGraphicsSystem::applyLighting() {
    if (shaderCache.find("lighting") == shaderCache.end() || !shaderCache["lighting"]) return;
    sf::RenderTexture tempTexture;
    if (!tempTexture.create(window.getSize().x, window.getSize().y)) {
        std::cerr << "Failed to create temp texture for lighting!" << std::endl;
        return;
    }
    tempTexture.clear(sf::Color::Transparent);
    tempTexture.setView(gameView);
    // Исправлено: используем -> для доступа к уникальному указателю
    sf::Sprite worldSprite(layerTextures[RenderLayer::TERRAIN]->getTexture());
    worldSprite.setTextureRect(sf::IntRect(0, 0, window.getSize().x, window.getSize().y));
    tempTexture.draw(worldSprite, shaderCache["lighting"].get());
    tempTexture.display();
    // Копируем результат обратно в worldTexture
    layerTextures[RenderLayer::TERRAIN]->clear(sf::Color::Transparent);
    sf::Sprite tempSprite(tempTexture.getTexture());
    layerTextures[RenderLayer::TERRAIN]->draw(tempSprite);
    layerTextures[RenderLayer::TERRAIN]->display();
}

// Применение пост-обработки
void AdvancedGraphicsSystem::applyPostProcessing() {
    if (shaderCache.find("postprocess") == shaderCache.end() || !shaderCache["postprocess"]) return;
    sf::RenderTexture tempTexture;
    if (!tempTexture.create(window.getSize().x, window.getSize().y)) {
        std::cerr << "Failed to create temp texture for post-processing!" << std::endl;
        return;
    }
    tempTexture.clear(sf::Color::Transparent);
    tempTexture.setView(uiView);
    // Исправлено: используем -> для доступа к уникальному указателю
    for (int layer = static_cast<int>(RenderLayer::TERRAIN
        );
        layer <= static_cast<int>(RenderLayer::UI); ++layer) {
        RenderLayer layerType = static_cast<RenderLayer>(layer);
        if (layerTextures.find(layerType) != layerTextures.end()) {
            sf::Sprite sprite(layerTextures[layerType]->getTexture());
            sprite.setTextureRect(sf::IntRect(0, 0, window.getSize().x, window.getSize().y));
            tempTexture.draw(sprite);
        }
    }
    tempTexture.display();
    // Копируем результат обратно в финальную текстуру
    layerTextures[RenderLayer::UI]->clear(sf::Color::Transparent);
    sf::Sprite tempSprite(tempTexture.getTexture());
    layerTextures[RenderLayer::UI]->draw(tempSprite);
    layerTextures[RenderLayer::UI]->display();
}

void AdvancedGraphicsSystem::renderDayNightOverlay(TimeOfDay time, WeatherType weather) {
    sf::RenderTexture& fxTex = *layerTextures[RenderLayer::EFFECTS];
    fxTex.setView(gameView);

    sf::Vector2f center = gameView.getCenter();
    sf::Vector2f size = gameView.getSize();
    sf::RectangleShape overlay(size);
    overlay.setPosition(center.x - size.x / 2.0f, center.y - size.y / 2.0f);

    sf::Color overlayColor = sf::Color::Transparent;
    float alpha = 0.0f;

    switch (time) {
    case TimeOfDay::NIGHT: alpha = 140.0f; overlayColor = sf::Color(10, 10, 50, static_cast<sf::Uint8>(alpha)); break;
    case TimeOfDay::DAWN:  alpha = 50.0f;  overlayColor = sf::Color(200, 120, 40, static_cast<sf::Uint8>(alpha)); break;
    case TimeOfDay::DUSK:  alpha = 60.0f;  overlayColor = sf::Color(180, 80, 30, static_cast<sf::Uint8>(alpha)); break;
    case TimeOfDay::DAY:   alpha = 0.0f;   break;
    }

    switch (weather) {
    case WeatherType::FOG:  overlayColor = sf::Color(180, 180, 190, 60); break;
    case WeatherType::RAIN: overlayColor = sf::Color(80, 80, 120, 30); break;
    case WeatherType::STORM: overlayColor = sf::Color(50, 50, 70, 70); break;
    case WeatherType::SNOW: overlayColor = sf::Color(200, 210, 230, 30); break;
    default: break;
    }

    if (alpha > 0.0f || weather != WeatherType::CLEAR) {
        overlay.setFillColor(overlayColor);
        fxTex.draw(overlay);
    }

    fxTex.display();
}

void AdvancedGraphicsSystem::renderLootDrops(const std::vector<std::pair<float, float>>& lootPositions) {
    sf::RenderTexture& fxTex = *layerTextures[RenderLayer::EFFECTS];
    fxTex.setView(gameView);

    for (const auto& [x, y] : lootPositions) {
        sf::CircleShape drop(6.0f);
        drop.setFillColor(sf::Color(255, 215, 0));
        drop.setOutlineColor(sf::Color(200, 150, 0));
        drop.setOutlineThickness(1.5f);
        drop.setPosition(x * 64.0f + 26.0f, y * 64.0f + 26.0f);
        fxTex.draw(drop);
    }

    fxTex.display();
}