// World/ChunkSystem.h
#ifndef CHUNKSYSTEM_H
#define CHUNKSYSTEM_H

#include "World/TileMap.h"
#include "World/TileBasedLocation.h"
#include "Utils/PerlinNoise.h"
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

struct ChunkCoord {
    int x, y;
    bool operator==(const ChunkCoord& other) const { return x == other.x && y == other.y; }
};

struct ChunkCoordHash {
    size_t operator()(const ChunkCoord& c) const {
        return std::hash<int>()(c.x) ^ (std::hash<int>()(c.y) << 16);
    }
};

struct Chunk {
    ChunkCoord coord;
    std::unique_ptr<TileMap> tileMap;
    bool generated;
    bool loaded;

    Chunk() : generated(false), loaded(false) {}
    Chunk(int cx, int cy) : coord({cx, cy}), generated(false), loaded(false) {}
};

class ChunkSystem {
private:
    int chunkSize;
    int loadRadius;
    int unloadRadius;

    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> chunks;

    std::unique_ptr<PerlinNoise> baseNoise;
    std::unique_ptr<PerlinNoise> mountainNoise;
    std::unique_ptr<PerlinNoise> forestNoise;

    unsigned int worldSeed;

    void generateChunk(Chunk* chunk);

    TileType getTerrainType(float height, float moisture, float temperature) const;
    TileType getObjectType(float height, float moisture, float temperature, int worldX, int worldY) const;
    TileType getDecorationType(float height, float moisture, float temperature, int worldX, int worldY) const;

public:
    ChunkSystem(int chunkSize = 64, int loadRadius = 3, int unloadRadius = 5);
    ~ChunkSystem() = default;

    void setWorldSeed(unsigned int seed);
    void updateChunks(int playerChunkX, int playerChunkY);

    TileType getTileAt(int worldX, int worldY) const;
    const TileLayer* getLayerAt(int worldX, int worldY) const;
    bool isWalkable(int worldX, int worldY) const;
    bool isChunkLoaded(int chunkX, int chunkY) const;

    int worldToChunkCoord(int worldCoord) const;
    int chunkToWorldCoord(int chunkCoord) const;
    int getChunkSize() const;

    const Chunk* getChunk(int chunkX, int chunkY) const;

    int getLoadedChunkCount() const;
    int getGeneratedChunkCount() const;

    ChunkSystem(const ChunkSystem&) = delete;
    ChunkSystem& operator=(const ChunkSystem&) = delete;
};

#endif // CHUNKSYSTEM_H
