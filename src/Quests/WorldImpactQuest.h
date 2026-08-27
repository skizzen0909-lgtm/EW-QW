// Quests/WorldImpactQuest.h
#ifndef WORLDIMPACTQUEST_H
#define WORLDIMPACTQUEST_H

#include "Quest.h"
#include "Types/Types.h"
#include <string>
#include <vector>

enum class WorldImpactType {
    PLACE_TILE,
    REMOVE_TILE,
    CHANGE_TILE,
    SPAWN_ENTITY,
    REMOVE_ENTITY,
    UNLOCK_LOCATION,
    CHANGE_WEATHER,
    MODIFY_NPC_RELATION
};

struct WorldImpact {
    WorldImpactType type;
    int targetX, targetY;
    TileType tileFrom;
    TileType tileTo;
    std::string entityId;
    std::string locationId;
    int intValue;
    float floatValue;

    WorldImpact()
        : type(WorldImpactType::PLACE_TILE)
        , targetX(0), targetY(0)
        , tileFrom(TileType::VOID), tileTo(TileType::GROUND)
        , intValue(0), floatValue(0.0f) {}
};

class WorldImpactQuest : public Quest {
private:
    std::vector<WorldImpact> impacts;
    bool impactApplied;

public:
    WorldImpactQuest(const std::string& questId, const std::string& title,
                     const std::string& description, QuestType type);

    void addImpact(const WorldImpact& impact);
    const std::vector<WorldImpact>& getImpacts() const;
    bool isImpactApplied() const;

    bool checkCompletionConditions(Entity* player) override;
    void complete(Entity* player) override;
    std::string getProgressString() const override;

    void applyWorldImpacts(class TileMap* tileMap, class Location* location);
};

#endif // WORLDIMPACTQUEST_H
