// Entities/Boss.h
#ifndef BOSS_H
#define BOSS_H

#include "Enemy.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

struct BossPhase {
    std::string name;
    float healthThreshold;
    std::string ability;
    int damage;
    float cooldown;
    float currentCooldown;
    bool triggered;
};

struct BossAbility {
    std::string id;
    std::string name;
    std::string description;
    int damage;
    float range;
    float cooldown;
    float currentCooldown;
    int areaRadius;

    BossAbility()
        : damage(0), range(1.0f), cooldown(5.0f),
          currentCooldown(0.0f), areaRadius(0) {}
    BossAbility(const std::string& id, const std::string& name, const std::string& desc,
                int dmg, float rng, float cd, int area = 0)
        : id(id), name(name), description(desc), damage(dmg),
          range(rng), cooldown(cd), currentCooldown(0.0f), areaRadius(area) {}
};

class Boss : public Enemy {
private:
    std::vector<BossPhase> phases;
    std::vector<BossAbility> abilities;
    int currentPhase;
    float enrageTimer;
    float enrageThreshold;
    bool enraged;
    std::string bossTitle;

public:
    Boss(const std::string& name, const std::string& bossTitle,
         const std::string& enemyType, int health = 500, int attack = 30);
    ~Boss() override = default;

    std::string getType() const override;
    const std::string& getBossTitle() const;

    void addPhase(const BossPhase& phase);
    void addAbility(const BossAbility& ability);
    void update(float deltaTime);

    void takeDamage(int damage) override;
    void checkPhaseTransitions();
    void useAbility(const std::string& abilityId, Entity* target);
    void triggerEnrage();

    const std::vector<BossPhase>& getPhases() const;
    const std::vector<BossAbility>& getAbilities() const;
    int getCurrentPhase() const;
    bool isEnraged() const;

    static std::shared_ptr<Boss> createGoblinKing(int level = 1);
    static std::shared_ptr<Boss> createDragonLord(int level = 15);
    static std::shared_ptr<Boss> createShadowLich(int level = 10);
};

#endif // BOSS_H
