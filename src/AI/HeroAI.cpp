// AI/HeroAI.cpp
#include "HeroAI.h"
#include "Entities/Entity.h"
#include "Entities/Merchant.h"
#include "Entities/Enemy.h"
#include "Entities/WorldResource.h"
#include "Systems/TradeSystem.h"
#include "Items/Item.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

HeroAI::HeroAI()
    : divineSystem(nullptr), tradeSystem(nullptr), worldEntities(nullptr)
    , exploreTimer(0.0f)
    , needCheckTimer(0.0f)
    , emotionUpdateTimer(0.0f)
    , travelTimer(0.0f)
    , moveTimer(0.0f)
    , experienceLevel(1)
    , currentStrength(1.0f)
    , lastPosition(0, 0)
    , homePosition(50, 50)
    , weatherSpeedMod(1.0f) {
}

void HeroAI::update(float deltaTime, Entity* entity) {
    needs.update(deltaTime);
    emotions.update(deltaTime);

    needCheckTimer += deltaTime;
    emotionUpdateTimer += deltaTime;
    travelTimer += deltaTime;
    moveTimer += deltaTime;

    if (entity) {
        auto pos = entity->getPosition();
        if (pos != lastPosition) {
            recordVisit(pos.first, pos.second);
            lastPosition = pos;
        }
    }

    if (needCheckTimer >= 1.0f) {
        evaluateBasicNeeds();
        needCheckTimer = 0.0f;
    }

    if (emotionUpdateTimer >= 2.0f) {
        evaluateEmotionalState();
        evaluateDivineCommands();
        emotionUpdateTimer = 0.0f;
    }

    evaluateGoals(entity);
    selectBestGoal();
    executeCurrentGoal(deltaTime, entity);

    if (entity) {
        autoUseItems(entity);
    }

    if (entity) {
        currentStrength = static_cast<float>(entity->getHealth()) / entity->getMaxHealth()
                        + static_cast<float>(entity->getLevel()) * 0.1f;
    }
}

void HeroAI::evaluateGoals(Entity* entity) {
    goals.clear();

    // EXPLORE — always available, priority varies
    float explorePriority = 0.3f;
    if (emotions.isHappy()) explorePriority += 0.2f;
    if (emotions.isBrave()) explorePriority += 0.15f;
    if (emotions.isAfraid()) explorePriority -= 0.2f;
    {
        HeroGoal exploreGoal;
        exploreGoal.type = HeroGoalType::EXPLORE;
        exploreGoal.priority = explorePriority;
        exploreGoal.active = true;
        goals.push_back(exploreGoal);
    }

    // TRAVEL_TO_UNKNOWN — explore unvisited areas, higher priority for curious hero
    if (exploreTimer > 3.0f) {
        TravelDestination dest = chooseTravelDestination();
        if (dest.attraction > 0.0f) {
            HeroGoal travelGoal;
            travelGoal.type = HeroGoalType::TRAVEL_TO_UNKNOWN;
            travelGoal.priority = 0.35f + dest.attraction * 0.3f;
            travelGoal.target = dest.position;
            travelGoal.active = true;
            travelGoal.reason = dest.reason;
            goals.push_back(travelGoal);
        }
    }

    // FIND_FOOD
    if (needs.needsFood()) {
        HeroGoal foodGoal;
        foodGoal.type = HeroGoalType::FIND_FOOD;
        foodGoal.priority = 0.8f;
        foodGoal.target = findNearestFood();
        foodGoal.active = true;
        goals.push_back(foodGoal);
    }

    // FIND_WATER
    if (needs.needsWater()) {
        HeroGoal waterGoal;
        waterGoal.type = HeroGoalType::FIND_WATER;
        waterGoal.priority = 0.85f;
        waterGoal.target = findNearestWater();
        waterGoal.active = true;
        goals.push_back(waterGoal);
    }

    // REST
    if (needs.needsRest()) {
        HeroGoal restGoal;
        restGoal.type = HeroGoalType::REST;
        restGoal.priority = 0.7f;
        restGoal.target = findRestSpot();
        restGoal.active = true;
        goals.push_back(restGoal);
    }

    // FLEE — fear overrides most things
    if (emotions.isAfraid()) {
        HeroGoal fleeGoal;
        fleeGoal.type = HeroGoalType::FLEE;
        fleeGoal.priority = 0.9f;
        fleeGoal.target = findSafeSpot();
        fleeGoal.active = true;
        goals.push_back(fleeGoal);
    }

    // FIGHT — brave hero seeks combat when healthy
    if (emotions.isBrave() && needs.getHealth() > needs.getMaxHealth() * 0.5f) {
        HeroGoal fightGoal;
        fightGoal.type = HeroGoalType::FIGHT;
        fightGoal.priority = 0.55f + currentStrength * 0.1f;
        fightGoal.active = true;
        goals.push_back(fightGoal);
    }

    // TRAVEL_TO_DANGER — aggressive hero seeks dangerous areas for XP
    if (emotions.isEnraged() || (emotions.isBrave() && experienceLevel >= 3)) {
        HeroGoal dangerGoal;
        dangerGoal.type = HeroGoalType::TRAVEL_TO_DANGER;
        dangerGoal.priority = 0.5f;
        TravelDestination dangerDest = chooseTravelDestination();
        dangerGoal.target = dangerDest.position;
        dangerGoal.active = true;
        dangerGoal.reason = "Сила и ярость толкают в опасность";
        goals.push_back(dangerGoal);
    }

    // FIND_NPC — sad hero seeks social contact
    if (emotions.isSad()) {
        HeroGoal npcGoal;
        npcGoal.type = HeroGoalType::FIND_NPC;
        npcGoal.priority = 0.6f;
        npcGoal.target = findNPC();
        npcGoal.active = true;
        goals.push_back(npcGoal);
    }

    // TRADE_WITH_MERCHANT — buy items based on needs/emotions
    if (tradeSystem && worldEntities) {
        float tradePriority = 0.0f;
        std::string tradeReason;

        if (needs.needsFood() || needs.needsWater()) {
            tradePriority = 0.75f;
            tradeReason = "Голоден/жажден — нужно купить еду";
        } else if (needs.getHealth() < needs.getMaxHealth() * 0.4f) {
            tradePriority = 0.82f;
            tradeReason = "Мало здоровья — нужно купить зелье";
        } else if (emotions.isAfraid() && entity && entity->getGold() >= 25) {
            tradePriority = 0.78f;
            tradeReason = "Страх — покупаю зелья лечения";
        } else if (emotions.isEnraged() && entity && entity->getGold() >= 100) {
            tradePriority = 0.65f;
            tradeReason = "Ярость — хочу купить оружие";
        } else if (entity && entity->getGold() > 150 && experienceLevel >= 3) {
            tradePriority = 0.5f;
            tradeReason = "Есть золото — хочу улучшить снаряжение";
        }

        if (tradePriority > 0.0f) {
            HeroGoal tradeGoal;
            tradeGoal.type = HeroGoalType::TRADE_WITH_MERCHANT;
            tradeGoal.priority = tradePriority;
            tradeGoal.target = findNearestMerchant();
            tradeGoal.active = true;
            tradeGoal.reason = tradeReason;
            goals.push_back(tradeGoal);
        }
    }

    // SELL_LOOT — sell excess items near merchant
    if (tradeSystem && worldEntities && entity) {
        int inventorySize = static_cast<int>(entity->getInventory().size());
        if (inventorySize > 8 && entity->getGold() < 50) {
            HeroGoal sellGoal;
            sellGoal.type = HeroGoalType::SELL_LOOT;
            sellGoal.priority = 0.6f;
            sellGoal.target = findNearestMerchantForSelling();
            sellGoal.active = true;
            sellGoal.reason = "Инвентарь полон — нужно продать лишнее";
            goals.push_back(sellGoal);
        }
    }

    // PRAY — happy hero or when faith is low
    if (emotions.isHappy() || (divineSystem && divineSystem->getFaith() < divineSystem->getMaxFaith() * 0.3f)) {
        HeroGoal prayGoal;
        prayGoal.type = HeroGoalType::PRAY;
        prayGoal.priority = 0.4f;
        prayGoal.active = true;
        goals.push_back(prayGoal);
    }

    // TRAVEL_TO_SAFE — wounded hero retreats to home
    if (needs.getHealthState() == NeedState::CRITICAL || needs.getHungerState() == NeedState::CRITICAL) {
        HeroGoal retreatGoal;
        retreatGoal.type = HeroGoalType::TRAVEL_TO_SAFE;
        retreatGoal.priority = 0.88f;
        retreatGoal.target = homePosition;
        retreatGoal.active = true;
        retreatGoal.reason = "Критическое состояние — возвращаюсь домой";
        goals.push_back(retreatGoal);
    }
}

void HeroAI::selectBestGoal() {
    if (goals.empty()) return;

    auto bestIt = std::max_element(goals.begin(), goals.end(),
        [](const HeroGoal& a, const HeroGoal& b) {
            return a.priority < b.priority;
        });

    if (bestIt != goals.end() && bestIt->priority > currentGoal.priority) {
        currentGoal = *bestIt;
        travelTimer = 0.0f;
    } else if (bestIt != goals.end() && travelTimer > 15.0f) {
        currentGoal = *bestIt;
        travelTimer = 0.0f;
    }
}

void HeroAI::moveToward(Entity* entity, int targetX, int targetY, float deltaTime) {
    if (!entity) return;
    auto [cx, cy] = entity->getPosition();
    int dx = targetX - cx;
    int dy = targetY - cy;
    float speed = emotions.getMovementSpeedModifier() * needs.getMovementSpeedModifier() * weatherSpeedMod;
    if (speed < 0.3f) speed = 0.3f;
    int step = std::max(1, static_cast<int>(3.0f * speed * deltaTime * 60.0f));
    if (dx != 0) cx += (dx > 0 ? std::min(step, dx) : std::max(-step, dx));
    if (dy != 0) cy += (dy > 0 ? std::min(step, dy) : std::max(-step, dy));
    entity->setPosition(cx, cy);
    needs.addEnergy(-0.5f * speed * deltaTime);
}

bool HeroAI::isNearTarget(Entity* entity, const std::pair<int, int>& target) const {
    if (!entity) return false;
    auto [cx, cy] = entity->getPosition();
    return std::abs(cx - target.first) <= 5 && std::abs(cy - target.second) <= 5;
}

void HeroAI::executeCurrentGoal(float deltaTime, Entity* entity) {
    if (!currentGoal.active) return;

    switch (currentGoal.type) {
    case HeroGoalType::FIND_FOOD:
        if (entity) moveToward(entity, currentGoal.target.first, currentGoal.target.second, deltaTime);
        exploreTimer += deltaTime;
        if (exploreTimer > 5.0f || (entity && isNearTarget(entity, currentGoal.target))) {
            needs.eat(30.0f);
            emotions.addJoy(10.0f);
            if (divineSystem) divineSystem->onGoodDeed();
            exploreTimer = 0.0f;
            currentGoal.active = false;
        }
        break;

    case HeroGoalType::FIND_WATER:
        if (entity) moveToward(entity, currentGoal.target.first, currentGoal.target.second, deltaTime);
        exploreTimer += deltaTime;
        if (exploreTimer > 5.0f || (entity && isNearTarget(entity, currentGoal.target))) {
            needs.drink(30.0f);
            emotions.addJoy(8.0f);
            exploreTimer = 0.0f;
            currentGoal.active = false;
        }
        break;

    case HeroGoalType::REST:
        needs.rest(deltaTime * 2.0f);
        emotions.addJoy(deltaTime);
        if (needs.getEnergy() > needs.getMaxEnergy() * 0.8f) {
            currentGoal.active = false;
        }
        break;

    case HeroGoalType::FLEE:
        if (entity) moveToward(entity, currentGoal.target.first, currentGoal.target.second, deltaTime);
        emotions.addFear(-deltaTime * 5.0f);
        if (!emotions.isAfraid()) {
            currentGoal.active = false;
        }
        break;

    case HeroGoalType::FIGHT:
        emotions.addAnger(deltaTime * 3.0f);
        break;

    case HeroGoalType::PRAY:
        if (divineSystem) {
            divineSystem->onHeroPrays();
        }
        emotions.addBravery(3.0f);
        currentGoal.active = false;
        break;

    case HeroGoalType::EXPLORE:
        if (entity) {
            int wanderX = lastPosition.first + static_cast<int>(std::sin(exploreTimer * 0.5f) * 10.0f);
            int wanderY = lastPosition.second + static_cast<int>(std::cos(exploreTimer * 0.7f) * 10.0f);
            moveToward(entity, wanderX, wanderY, deltaTime);
        }
        exploreTimer += deltaTime;
        emotions.addJoy(deltaTime * 0.5f);
        if (exploreTimer > 10.0f) {
            emotions.addJoy(5.0f);
            exploreTimer = 0.0f;
        }
        break;

    case HeroGoalType::TRAVEL_TO_UNKNOWN:
    case HeroGoalType::TRAVEL_TO_DANGER:
    case HeroGoalType::TRAVEL_TO_SAFE:
        if (entity) moveToward(entity, currentGoal.target.first, currentGoal.target.second, deltaTime);
        exploreTimer += deltaTime;
        if (exploreTimer > 8.0f || (entity && isNearTarget(entity, currentGoal.target))) {
            emotions.addJoy(3.0f);
            exploreTimer = 0.0f;
            currentGoal.active = false;
        }
        break;

    case HeroGoalType::FIND_NPC:
        if (entity) moveToward(entity, currentGoal.target.first, currentGoal.target.second, deltaTime);
        exploreTimer += deltaTime;
        if (exploreTimer > 6.0f || (entity && isNearTarget(entity, currentGoal.target))) {
            emotions.onFriendNearby();
            if (divineSystem) divineSystem->onHeroHelpsNPC();
            exploreTimer = 0.0f;
            currentGoal.active = false;
        }
        break;

    case HeroGoalType::TRADE_WITH_MERCHANT:
        if (entity) moveToward(entity, currentGoal.target.first, currentGoal.target.second, deltaTime);
        exploreTimer += deltaTime;
        if (exploreTimer > 3.0f || (entity && isNearTarget(entity, currentGoal.target))) {
            autoTrade(entity);
            exploreTimer = 0.0f;
            currentGoal.active = false;
        }
        break;

    case HeroGoalType::SELL_LOOT:
        if (entity) moveToward(entity, currentGoal.target.first, currentGoal.target.second, deltaTime);
        exploreTimer += deltaTime;
        if (exploreTimer > 3.0f || (entity && isNearTarget(entity, currentGoal.target))) {
            autoSellLoot(entity);
            exploreTimer = 0.0f;
            currentGoal.active = false;
        }
        break;

    case HeroGoalType::FOLLOW_DIVINE_COMMAND:
        break;
    }
}

void HeroAI::evaluateBasicNeeds() {
    if (needs.getHealthState() == NeedState::CRITICAL) {
        emotions.triggerFear(20.0f);
    }
    if (needs.getHungerState() == NeedState::LOW) {
        emotions.addSadness(5.0f);
    }
    if (needs.getThirstState() == NeedState::LOW) {
        emotions.addSadness(5.0f);
    }
    if (needs.getEnergyState() == NeedState::LOW) {
        emotions.addSadness(3.0f);
    }
}

void HeroAI::evaluateEmotionalState() {
    if (emotions.isAfraid()) {
        needs.addEnergy(-2.0f);
    }
    if (emotions.isEnraged()) {
        needs.addEnergy(-5.0f);
    }
    if (emotions.isHappy()) {
        needs.addEnergy(3.0f);
    }
}

void HeroAI::evaluateDivineCommands() {
    if (divineSystem && divineSystem->getFaith() > 50.0f) {
        emotions.addBravery(5.0f);
    }
}

float HeroAI::calculateDangerScore(int x, int y) const {
    float distHome = std::sqrt(
        static_cast<float>((x - homePosition.first) * (x - homePosition.first) +
                           (y - homePosition.second) * (y - homePosition.second)));
    float danger = distHome * 0.005f;
    danger += static_cast<float>(std::abs(x % 7 + y % 11)) * 0.02f;
    return std::min(danger, 1.0f);
}

float HeroAI::calculateExplorationValue(int x, int y) const {
    if (hasVisited(x, y)) return 0.0f;
    float value = 1.0f;
    float distHome = std::sqrt(
        static_cast<float>((x - homePosition.first) * (x - homePosition.first) +
                           (y - homePosition.second) * (y - homePosition.second)));
    if (distHome < 30.0f) value *= 0.5f;
    if (distHome > 200.0f) value *= 0.7f;
    return value;
}

TravelDestination HeroAI::chooseTravelDestination() const {
    TravelDestination best;
    best.attraction = 0.0f;

    for (int attempt = 0; attempt < 8; attempt++) {
        int tx = homePosition.first + (std::rand() % 400) - 200;
        int ty = homePosition.second + (std::rand() % 400) - 200;

        float danger = calculateDangerScore(tx, ty);
        float exploration = calculateExplorationValue(tx, ty);

        float attraction = 0.0f;
        DangerLevel dangerLevel;

        if (danger < 0.2f) dangerLevel = DangerLevel::SAFE;
        else if (danger < 0.4f) dangerLevel = DangerLevel::MILD;
        else if (danger < 0.6f) dangerLevel = DangerLevel::MODERATE;
        else if (danger < 0.8f) dangerLevel = DangerLevel::DANGEROUS;
        else dangerLevel = DangerLevel::DEADLY;

        if (emotions.isAfraid()) {
            attraction = (1.0f - danger) * 0.8f + exploration * 0.2f;
        } else if (emotions.isEnraged() || emotions.isBrave()) {
            attraction = danger * 0.6f + exploration * 0.4f;
        } else if (emotions.isSad()) {
            float distHome = std::sqrt(
                static_cast<float>((tx - homePosition.first) * (tx - homePosition.first) +
                                   (ty - homePosition.second) * (ty - homePosition.second)));
            attraction = (distHome < 50.0f ? 0.8f : 0.3f) + exploration * 0.3f;
        } else {
            attraction = exploration * 0.5f + 0.3f;
        }

        if (needs.getHealthState() == NeedState::CRITICAL) {
            attraction = (1.0f - danger) * 0.9f;
        }

        attraction *= (1.0f + experienceLevel * 0.05f);

        if (attraction > best.attraction) {
            best.position = {tx, ty};
            best.danger = dangerLevel;
            best.attraction = attraction;

            if (emotions.isAfraid()) best.reason = "Ищу укрытие подальше от опасности";
            else if (emotions.isEnraged()) best.reason = "Ищу кого-нибудь для боя";
            else if (emotions.isSad()) best.reason = "Возвращаюсь к знакомым местам";
            else best.reason = "Исследую новые территории";
        }
    }

    return best;
}

bool HeroAI::hasVisited(int x, int y) const {
    for (const auto& v : visitedPositions) {
        if (std::abs(v.first - x) < 5 && std::abs(v.second - y) < 5) return true;
    }
    return false;
}

void HeroAI::recordVisit(int x, int y) {
    visitedPositions.push_back({x, y});
    if (visitedPositions.size() > 200) {
        visitedPositions.erase(visitedPositions.begin());
    }
}

std::pair<int, int> HeroAI::findNearestFood() const {
    if (worldEntities) {
        auto [hx, hy] = homePosition;
        float bestDist = 999999.0f;
        auto result = homePosition;
        for (const auto& e : *worldEntities) {
            if (!e || !e->isAlive()) continue;
            auto* res = dynamic_cast<WorldResource*>(e.get());
            if (res && (res->getResourceType() == ResourceType::BUSH || res->getResourceType() == ResourceType::MUSHROOM)
                && res->canGather()) {
                auto [rx, ry] = e->getPosition();
                float d = static_cast<float>(std::abs(rx - hx) + std::abs(ry - hy));
                if (d < bestDist) { bestDist = d; result = {rx, ry}; }
            }
        }
        if (bestDist < 999999.0f) return result;
    }
    return homePosition;
}

std::pair<int, int> HeroAI::findNearestWater() const {
    auto [hx, hy] = homePosition;
    return {hx + 10, hy};
}

std::pair<int, int> HeroAI::findSafeSpot() const {
    return homePosition;
}

std::pair<int, int> HeroAI::findNPC() const {
    if (worldEntities) {
        auto [hx, hy] = homePosition;
        float bestDist = 999999.0f;
        auto result = homePosition;
        for (const auto& e : *worldEntities) {
            if (!e || !e->isAlive()) continue;
            std::string etype = e->getType();
            if (etype == "npc" || etype == "villager") {
                auto [nx, ny] = e->getPosition();
                float d = static_cast<float>(std::abs(nx - hx) + std::abs(ny - hy));
                if (d < bestDist) { bestDist = d; result = {nx, ny}; }
            }
        }
        if (bestDist < 999999.0f) return result;
    }
    return homePosition;
}

std::pair<int, int> HeroAI::findRestSpot() const {
    return homePosition;
}

std::pair<int, int> HeroAI::findNearestMerchant() const {
    if (!worldEntities) return homePosition;
    auto result = homePosition;
    float bestDist = 999999.0f;
    for (const auto& e : *worldEntities) {
        if (!e || !e->isAlive()) continue;
        if (dynamic_cast<Merchant*>(e.get())) {
            auto [mx, my] = e->getPosition();
            float d = static_cast<float>(std::abs(mx - homePosition.first) + std::abs(my - homePosition.second));
            if (d < bestDist) { bestDist = d; result = {mx, my}; }
        }
    }
    return result;
}

std::pair<int, int> HeroAI::findNearestMerchantForSelling() const {
    return findNearestMerchant();
}

void HeroAI::autoUseItems(Entity* entity) {
    if (!entity) return;
    const auto& inv = entity->getInventory();

    if (needs.getHealth() < needs.getMaxHealth() * 0.5f) {
        for (const auto& item : inv) {
            if (item && (item->getId() == "health_potion" || item->getId() == "greater_health_potion"
                       || item->getId() == "minor_health_potion")) {
                int heal = item->getId() == "greater_health_potion" ? 50 : 25;
                entity->heal(heal);
                needs.heal(static_cast<float>(heal));
                std::cout << "[AI] " << entity->getName() << " использует " << item->getName()
                          << " (+" << heal << " HP)" << std::endl;
                entity->removeItem(item);
                emotions.addJoy(5.0f);
                addEventLog("Использовал " + item->getName() + " (+" + std::to_string(heal) + " HP)", sf::Color(100, 255, 150));
                return;
            }
        }
    }

    if (needs.needsFood()) {
        for (const auto& item : inv) {
            if (item && (item->getId() == "bread" || item->getId() == "cooked_meat"
                       || item->getId() == "berry" || item->getId() == "raw_meat")) {
                float hunger = item->getId() == "bread" ? 30.0f : 20.0f;
                needs.eat(hunger);
                std::cout << "[AI] " << entity->getName() << " ест " << item->getName() << std::endl;
                entity->removeItem(item);
                addEventLog("Ест " + item->getName(), sf::Color(200, 180, 100));
                return;
            }
        }
    }

    if (needs.needsWater()) {
        for (const auto& item : inv) {
            if (item && item->getId() == "water_flask") {
                needs.drink(40.0f);
                std::cout << "[AI] " << entity->getName() << " пьёт " << item->getName() << std::endl;
                entity->removeItem(item);
                addEventLog("Пьёт " + item->getName(), sf::Color(100, 180, 255));
                return;
            }
        }
    }
}

void HeroAI::autoTrade(Entity* entity) {
    if (!entity || !tradeSystem) return;

    auto buyItem = [&](const std::string& itemId) {
        if (tradeSystem->buyItem(itemId, entity)) {
            std::cout << "[AI] " << entity->getName() << " купил " << itemId << " (золото: " << entity->getGold() << ")" << std::endl;
            emotions.addJoy(8.0f);
            addEventLog("Купил " + itemId + " у торговца", sf::Color(180, 220, 100));
            return true;
        }
        return false;
    };

    if (needs.getHealth() < needs.getMaxHealth() * 0.4f && entity->getGold() >= 25) {
        if (buyItem("health_potion")) return;
    }

    if (emotions.isAfraid() && entity->getGold() >= 25) {
        buyItem("health_potion");
        buyItem("health_potion");
        buyItem("health_potion");
        return;
    }

    if (needs.needsFood() && entity->getGold() >= 5) {
        if (buyItem("bread")) { needs.eat(30.0f); return; }
    }

    if (needs.needsWater() && entity->getGold() >= 3) {
        if (buyItem("water_flask")) { needs.drink(30.0f); return; }
    }

    if (emotions.isEnraged() && entity->getGold() >= 100) {
        buyItem("iron_sword");
        return;
    }

    if (entity->getGold() >= 50 && experienceLevel >= 3) {
        if (buyItem("leather_armor")) {
            return;
        }
    }
}

void HeroAI::autoSellLoot(Entity* entity) {
    if (!entity || !tradeSystem) return;

    std::vector<std::string> junkIds = {"wolf_fang", "wolf_pelt", "leather_scrap", "bone_fragment",
                                         "raw_meat", "bear_claw", "bear_pelt", "ancient_coin",
                                         "rusty_sword", "stone"};

    for (const auto& junkId : junkIds) {
        int count = 0;
        for (const auto& item : entity->getInventory()) {
            if (item && item->getId() == junkId) count++;
        }
        for (int i = 0; i < count; ++i) {
            if (tradeSystem->sellItem(junkId, entity, 1)) {
                std::cout << "[AI] " << entity->getName() << " продал " << junkId
                          << " (золото: " << entity->getGold() << ")" << std::endl;
                emotions.addJoy(3.0f);
                addEventLog("Продал " + junkId + " торговцу", sf::Color(200, 180, 80));
            }
        }
    }
}

HeroNeeds& HeroAI::getNeeds() { return needs; }
HeroEmotions& HeroAI::getEmotions() { return emotions; }

void HeroAI::setDivineSystem(DivineSystem* divine) { divineSystem = divine; }
void HeroAI::setTradeSystem(TradeSystem* trade) { tradeSystem = trade; }
void HeroAI::setWorldEntities(const std::vector<std::shared_ptr<Entity>>* entities) { worldEntities = entities; }
void HeroAI::setWeatherSpeedMod(float mod) { weatherSpeedMod = mod; }

std::vector<std::pair<std::string, sf::Color>> HeroAI::drainEventLog() {
    auto result = std::move(eventLog);
    eventLog.clear();
    return result;
}

void HeroAI::addEventLog(const std::string& text, sf::Color color) {
    eventLog.emplace_back(text, color);
}

const HeroGoal& HeroAI::getCurrentGoal() const { return currentGoal; }
bool HeroAI::hasActiveGoal() const { return currentGoal.active; }

void HeroAI::setExperienceLevel(int level) { experienceLevel = level; }
void HeroAI::setStrength(float strength) { currentStrength = strength; }
void HeroAI::setHomePosition(int x, int y) { homePosition = {x, y}; }

void HeroAI::onDivineBlessing() {
    emotions.addJoy(20.0f);
    emotions.addBravery(15.0f);
    if (divineSystem) {
        divineSystem->onHeroFaithGrows(5.0f);
    }
}

void HeroAI::onDivineCurse() {
    emotions.addFear(30.0f);
    emotions.addSadness(20.0f);
    if (divineSystem) {
        divineSystem->drainFaith(10.0f);
    }
}

void HeroAI::onDamageTaken(float damage) {
    emotions.onDamageTaken(damage);
    needs.takeDamage(damage);
}

void HeroAI::onEnemyKilled() {
    emotions.onEnemyKilled();
    experienceLevel++;
    if (divineSystem) {
        divineSystem->onHeroKillsEnemy();
    }
}

void HeroAI::onItemFound() {
    emotions.addJoy(15.0f);
}

void HeroAI::onNPCInteraction() {
    emotions.onFriendNearby();
    if (divineSystem) {
        divineSystem->onHeroHelpsNPC();
    }
}

void HeroAI::onPray() {
    if (divineSystem) {
        divineSystem->onHeroPrays();
    }
    emotions.addBravery(5.0f);
}
