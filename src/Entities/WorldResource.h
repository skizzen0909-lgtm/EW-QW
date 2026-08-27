#ifndef WORLDRESOURCE_H
#define WORLDRESOURCE_H

#include "Entity.h"
#include <string>

enum class ResourceType {
    TREE,
    ROCK,
    BUSH,
    MUSHROOM,
    CRYSTAL
};

class WorldResource : public Entity {
private:
    ResourceType resourceType;
    int gatherAmount;
    int maxGatherAmount;
    float regrowTimer;
    float regrowRate;

public:
    WorldResource(ResourceType type, int x, int y);
    ~WorldResource() override = default;

    ResourceType getResourceType() const { return resourceType; }
    int getGatherAmount() const { return gatherAmount; }
    bool canGather() const { return gatherAmount > 0 && isAlive(); }

    int gather(int maxAmount);
    void regrow(float deltaTime);
    bool isDepleted() const { return gatherAmount <= 0; }

    std::string getType() const override { return "WorldResource"; }
    void update(float deltaTime) override;
};

#endif
