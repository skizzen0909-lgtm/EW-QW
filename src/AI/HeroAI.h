// AI/HeroAI.h
#ifndef HEROAI_H
#define HEROAI_H

#include "AI/AIBase.h"
#include "Entities/HeroNeeds.h"
#include "Entities/HeroEmotions.h"
#include "Systems/DivineSystem.h"
#include <memory>
#include <vector>
#include <utility>
#include <string>
#include <SFML/Graphics.hpp>

class Entity;
class TradeSystem;

enum class HeroGoalType {
    EXPLORE,
    FIND_FOOD,
    FIND_WATER,
    REST,
    FIGHT,
    FLEE,
    FIND_NPC,
    PRAY,
    FOLLOW_DIVINE_COMMAND,
    TRAVEL_TO_DANGER,
    TRAVEL_TO_SAFE,
    TRAVEL_TO_UNKNOWN,
    TRADE_WITH_MERCHANT,
    SELL_LOOT,
    USE_ITEM
};

enum class DangerLevel {
    SAFE,
    MILD,
    MODERATE,
    DANGEROUS,
    DEADLY
};

struct TravelDestination {
    std::pair<int, int> position;
    DangerLevel danger;
    float attraction;
    std::string reason;

    TravelDestination() : position(0, 0), danger(DangerLevel::SAFE), attraction(0.0f) {}
};

struct HeroGoal {
    HeroGoalType type;
    float priority;
    std::pair<int, int> target;
    bool active;
    std::string reason;

    HeroGoal() : type(HeroGoalType::EXPLORE), priority(0.0f), target(0, 0), active(false) {}
};

class HeroAI : public AIBase {
private:
    HeroNeeds needs;
    HeroEmotions emotions;
    DivineSystem* divineSystem;
    TradeSystem* tradeSystem;
    const std::vector<std::shared_ptr<Entity>>* worldEntities;

    std::vector<HeroGoal> goals;
    HeroGoal currentGoal;

    float exploreTimer;
    float needCheckTimer;
    float emotionUpdateTimer;
    float travelTimer;
    float moveTimer;

    int experienceLevel;
    float currentStrength;
    std::pair<int, int> lastPosition;
    std::vector<std::pair<int, int>> visitedPositions;
    std::pair<int, int> homePosition;
    float weatherSpeedMod;
    std::vector<std::pair<std::string, sf::Color>> eventLog;

    void moveToward(Entity* entity, int targetX, int targetY, float deltaTime);
    bool isNearTarget(Entity* entity, const std::pair<int, int>& target) const;

    void evaluateGoals(Entity* entity = nullptr);
    void selectBestGoal();
    void executeCurrentGoal(float deltaTime, Entity* entity);

    void evaluateBasicNeeds();
    void evaluateEmotionalState();
    void evaluateDivineCommands();

    float calculateDangerScore(int x, int y) const;
    float calculateExplorationValue(int x, int y) const;
    TravelDestination chooseTravelDestination() const;

    std::pair<int, int> findNearestFood() const;
    std::pair<int, int> findNearestWater() const;
    std::pair<int, int> findSafeSpot() const;
    std::pair<int, int> findNPC() const;
    std::pair<int, int> findRestSpot() const;
    std::pair<int, int> findNearestMerchant() const;
    std::pair<int, int> findNearestMerchantForSelling() const;
    void autoUseItems(Entity* entity);
    void autoTrade(Entity* entity);
    void autoSellLoot(Entity* entity);

    bool hasVisited(int x, int y) const;
    void recordVisit(int x, int y);

public:
    HeroAI();
    ~HeroAI() override = default;

    void update(float deltaTime, Entity* entity) override;

    HeroNeeds& getNeeds();
    HeroEmotions& getEmotions();
    void setDivineSystem(DivineSystem* divine);
    void setTradeSystem(TradeSystem* trade);
    void setWorldEntities(const std::vector<std::shared_ptr<Entity>>* entities);
    void setWeatherSpeedMod(float mod);

    const HeroGoal& getCurrentGoal() const;
    bool hasActiveGoal() const;

    void setExperienceLevel(int level);
    void setStrength(float strength);
    void setHomePosition(int x, int y);

    std::vector<std::pair<std::string, sf::Color>> drainEventLog();
    void addEventLog(const std::string& text, sf::Color color = sf::Color::White);

    void onDivineBlessing();
    void onDivineCurse();
    void onDamageTaken(float damage);
    void onEnemyKilled();
    void onItemFound();
    void onNPCInteraction();
    void onPray();

    HeroAI(const HeroAI&) = delete;
    HeroAI& operator=(const HeroAI&) = delete;
};

#endif // HEROAI_H
