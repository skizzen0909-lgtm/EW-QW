#include "WorldResource.h"
#include <algorithm>

WorldResource::WorldResource(ResourceType type, int x, int y)
    : Entity("Resource", 1), resourceType(type)
    , gatherAmount(3), maxGatherAmount(3)
    , regrowTimer(0.0f), regrowRate(30.0f) {
    setPosition(x, y);
    switch (type) {
    case ResourceType::TREE:    setName("Дерево"); gatherAmount = 4; maxGatherAmount = 4; break;
    case ResourceType::ROCK:    setName("Камень"); gatherAmount = 3; maxGatherAmount = 3; break;
    case ResourceType::BUSH:    setName("Куст"); gatherAmount = 2; maxGatherAmount = 2; break;
    case ResourceType::MUSHROOM: setName("Гриб"); gatherAmount = 1; maxGatherAmount = 1; break;
    case ResourceType::CRYSTAL: setName("Кристалл"); gatherAmount = 1; maxGatherAmount = 1; break;
    }
    health = 9999;
    maxHealth = 9999;
}

int WorldResource::gather(int maxAmount) {
    if (gatherAmount <= 0) return 0;
    int gathered = std::min(maxAmount, gatherAmount);
    gatherAmount -= gathered;
    if (gatherAmount <= 0) {
        health = 0;
    }
    return gathered;
}

void WorldResource::regrow(float deltaTime) {
    if (gatherAmount >= maxGatherAmount) return;
    regrowTimer += deltaTime;
    if (regrowTimer >= regrowRate) {
        regrowTimer = 0.0f;
        gatherAmount = std::min(gatherAmount + 1, maxGatherAmount);
        if (gatherAmount > 0) health = 9999;
    }
}

void WorldResource::update(float deltaTime) {
    if (isDepleted()) {
        regrow(deltaTime);
    }
}
