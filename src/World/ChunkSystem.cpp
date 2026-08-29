// World/ChunkSystem.cpp
#include "ChunkSystem.h"
#include "World/BiomeSystem.h"
#include <iostream>
#include <cmath>
#include <algorithm>

ChunkSystem::ChunkSystem(int chunkSize, int loadRadius, int unloadRadius)
    : chunkSize(chunkSize)
    , loadRadius(loadRadius)
    , unloadRadius(unloadRadius)
    , worldSeed(0)
    , baseNoise(std::make_unique<PerlinNoise>(0))
    , mountainNoise(std::make_unique<PerlinNoise>(1))
    , forestNoise(std::make_unique<PerlinNoise>(2)) {
}

void ChunkSystem::setWorldSeed(unsigned int seed) {
    worldSeed = seed;
    baseNoise = std::make_unique<PerlinNoise>(seed);
    mountainNoise = std::make_unique<PerlinNoise>(seed + 1);
    forestNoise = std::make_unique<PerlinNoise>(seed + 2);
}

void ChunkSystem::updateChunks(int playerChunkX, int playerChunkY) {
    // Генерация и загрузка чанков в радиусе
    for (int dy = -loadRadius; dy <= loadRadius; dy++) {
        for (int dx = -loadRadius; dx <= loadRadius; dx++) {
            int cx = playerChunkX + dx;
            int cy = playerChunkY + dy;

            ChunkCoord coord{cx, cy};
            auto it = chunks.find(coord);

            if (it == chunks.end()) {
                auto chunk = std::make_unique<Chunk>(cx, cy);
                generateChunk(chunk.get());
                chunks[coord] = std::move(chunk);
            }
            else if (!it->second->loaded) {
                it->second->loaded = true;
            }
        }
    }

    // Выгрузка дальних чанков
    std::vector<ChunkCoord> toUnload;
    for (auto& [coord, chunk] : chunks) {
        int dx = coord.x - playerChunkX;
        int dy = coord.y - playerChunkY;
        int dist = std::max(std::abs(dx), std::abs(dy));

        if (dist > unloadRadius) {
            toUnload.push_back(coord);
        }
    }

    for (const auto& coord : toUnload) {
        chunks.erase(coord);
    }
}

void ChunkSystem::generateChunk(Chunk* chunk) {
    if (chunk->generated) return;

    chunk->tileMap = std::make_unique<TileMap>(chunkSize, chunkSize);
    chunk->loaded = true;

    int offsetX = chunk->coord.x * chunkSize;
    int offsetY = chunk->coord.y * chunkSize;

    BiomeSystem biomeSystem;

    for (int y = 0; y < chunkSize; y++) {
        for (int x = 0; x < chunkSize; x++) {
            int worldX = offsetX + x;
            int worldY = offsetY + y;

            float nx = static_cast<float>(worldX) * 0.005f;
            float ny = static_cast<float>(worldY) * 0.005f;

            float height = baseNoise->fractalNoise(nx, ny, 5, 0.5f);
            height = height * 0.5f + 0.5f;
            float moisture = forestNoise->fractalNoise(nx * 2.0f, ny * 2.0f, 3, 0.6f);
            moisture = moisture * 0.5f + 0.5f;
            float temperature = 0.5f + 0.15f * std::sin(static_cast<float>(worldY) * 0.002f)
                + 0.05f * mountainNoise->fractalNoise(nx * 1.5f, ny * 1.5f, 2, 0.4f);

            TileType baseType = getTerrainType(height, moisture, temperature);
            chunk->tileMap->setTile(x, y, baseType);

            TileType objectType = getObjectType(height, moisture, temperature, worldX, worldY);
            if (objectType != TileType::VOID) {
                chunk->tileMap->addObjectToTile(x, y, objectType);
            }

            TileType decorType = getDecorationType(height, moisture, temperature, worldX, worldY);
            if (decorType != TileType::VOID) {
                chunk->tileMap->addDecorationToTile(x, y, decorType);
            }

            int variant = static_cast<int>(baseNoise->fractalNoise(
                static_cast<float>(worldX) * 0.1f,
                static_cast<float>(worldY) * 0.1f, 2, 0.3f) * 4.0f);
            chunk->tileMap->getLayer(x, y).groundVariant = std::clamp(variant, 0, 3);
        }
    }

    chunk->generated = true;
}

TileType ChunkSystem::getTerrainType(float height, float moisture, float temperature) const {
    if (height < 0.3f) return TileType::WATER;
    if (height < 0.38f) return TileType::SAND;
    if (height > 0.78f) return TileType::ROCK;
    if (height > 0.68f) {
        return (temperature < -0.1f) ? TileType::SNOW : TileType::ROCK;
    }
    if (temperature < -0.2f) return TileType::SNOW;
    if (temperature < -0.05f && height > 0.5f) return TileType::SNOW;
    if (moisture > 0.5f) return TileType::GRASS;
    if (moisture < 0.2f && temperature > 0.3f) return TileType::DESERT;
    if (moisture < 0.3f && temperature > 0.2f) return TileType::SAND;
    return TileType::GROUND;
}

TileType ChunkSystem::getObjectType(float height, float moisture, float temperature, int worldX, int worldY) const {
    if (height < 0.38f || height > 0.75f) return TileType::VOID;

    float localNoise = forestNoise->fractalNoise(
        static_cast<float>(worldX) * 0.08f,
        static_cast<float>(worldY) * 0.08f, 2, 0.5f);

    float treeDensity = moisture * 0.7f;

    if (temperature < -0.15f) {
        treeDensity *= 0.3f;
    }
    if (temperature > 0.5f && moisture < 0.3f) {
        treeDensity *= 0.1f;
    }

    if (localNoise > (1.0f - treeDensity) && height > 0.42f) {
        return TileType::TREE;
    }

    if (height > 0.6f && height < 0.68f && localNoise > 0.85f) {
        return TileType::RUIN;
    }

    if (moisture < 0.2f && temperature > 0.3f && localNoise > 0.9f) {
        return TileType::STONE;
    }

    return TileType::VOID;
}

TileType ChunkSystem::getDecorationType(float height, float moisture, float temperature, int worldX, int worldY) const {
    if (height < 0.4f || height > 0.72f) return TileType::VOID;

    float decorNoise = baseNoise->fractalNoise(
        static_cast<float>(worldX) * 0.2f + 500.0f,
        static_cast<float>(worldY) * 0.2f + 500.0f, 2, 0.4f);

    if (moisture > 0.5f && temperature > 0.0f && decorNoise > 0.82f) {
        return TileType::FLOWER;
    }

    if (decorNoise > 0.88f) {
        return TileType::GRASS_PATCH;
    }

    if (moisture < 0.3f && decorNoise > 0.85f) {
        return TileType::STONE;
    }

    return TileType::VOID;
}

const TileLayer* ChunkSystem::getLayerAt(int worldX, int worldY) const {
    int cx = worldToChunkCoord(worldX);
    int cy = worldToChunkCoord(worldY);
    ChunkCoord coord{cx, cy};
    auto it = chunks.find(coord);
    if (it == chunks.end() || !it->second->generated) return nullptr;
    int localX = worldX - cx * chunkSize;
    int localY = worldY - cy * chunkSize;
    if (localX < 0 || localX >= chunkSize || localY < 0 || localY >= chunkSize) return nullptr;
    return &it->second->tileMap->getLayer(localX, localY);
}

TileType ChunkSystem::getTileAt(int worldX, int worldY) const {
    int cx = worldToChunkCoord(worldX);
    int cy = worldToChunkCoord(worldY);

    ChunkCoord coord{cx, cy};
    auto it = chunks.find(coord);
    if (it == chunks.end() || !it->second->generated) {
        return TileType::VOID;
    }

    int localX = worldX - cx * chunkSize;
    int localY = worldY - cy * chunkSize;

    if (localX < 0 || localX >= chunkSize || localY < 0 || localY >= chunkSize) {
        return TileType::VOID;
    }

    return it->second->tileMap->getTile(localX, localY);
}

bool ChunkSystem::isWalkable(int worldX, int worldY) const {
    TileType type = getTileAt(worldX, worldY);
    return type != TileType::WATER && type != TileType::MOUNTAIN && type != TileType::VOID;
}

bool ChunkSystem::isChunkLoaded(int chunkX, int chunkY) const {
    ChunkCoord coord{chunkX, chunkY};
    auto it = chunks.find(coord);
    return it != chunks.end() && it->second->generated;
}

int ChunkSystem::worldToChunkCoord(int worldCoord) const {
    if (worldCoord >= 0) return worldCoord / chunkSize;
    return (worldCoord + 1) / chunkSize - 1;
}

int ChunkSystem::chunkToWorldCoord(int chunkCoord) const {
    return chunkCoord * chunkSize;
}

int ChunkSystem::getChunkSize() const {
    return chunkSize;
}

const Chunk* ChunkSystem::getChunk(int chunkX, int chunkY) const {
    ChunkCoord coord{chunkX, chunkY};
    auto it = chunks.find(coord);
    return (it != chunks.end()) ? it->second.get() : nullptr;
}

int ChunkSystem::getLoadedChunkCount() const {
    int count = 0;
    for (const auto& [coord, chunk] : chunks) {
        if (chunk->loaded) count++;
    }
    return count;
}

int ChunkSystem::getGeneratedChunkCount() const {
    int count = 0;
    for (const auto& [coord, chunk] : chunks) {
        if (chunk->generated) count++;
    }
    return count;
}
