// Entities/Boss.cpp
#include "Boss.h"
#include "Entities/Entity.h"
#include "Events/CombatEvent.h"
#include "Events/EventManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Boss::Boss(const std::string& name, const std::string& bossTitle,
           const std::string& enemyType, int health, int attack)
    : Enemy(name, enemyType, health, attack)
    , currentPhase(0)
    , enrageTimer(0.0f)
    , enrageThreshold(300.0f)
    , enraged(false)
    , bossTitle(bossTitle) {
    setLootExperience(100 + health / 5);
}

std::string Boss::getType() const {
    return "boss";
}

const std::string& Boss::getBossTitle() const {
    return bossTitle;
}

void Boss::addPhase(const BossPhase& phase) {
    phases.push_back(phase);
    std::sort(phases.begin(), phases.end(),
        [](const BossPhase& a, const BossPhase& b) {
            return a.healthThreshold > b.healthThreshold;
        });
}

void Boss::addAbility(const BossAbility& ability) {
    abilities.push_back(ability);
}

void Boss::update(float deltaTime) {
    Enemy::takeDamage(0); // tick base class logic

    // Обновляем кулдауны способностей
    for (auto& ability : abilities) {
        if (ability.currentCooldown > 0) {
            ability.currentCooldown -= deltaTime;
            if (ability.currentCooldown < 0) ability.currentCooldown = 0;
        }
    }

    // Обновляем кулдауны фаз
    for (auto& phase : phases) {
        if (phase.currentCooldown > 0) {
            phase.currentCooldown -= deltaTime;
            if (phase.currentCooldown < 0) phase.currentCooldown = 0;
        }
    }

    // Проверяем переходы фаз
    checkPhaseTransitions();

    // Ярость по времени
    if (!enraged) {
        enrageTimer += deltaTime;
        if (enrageTimer >= enrageThreshold) {
            triggerEnrage();
        }
    }
}

void Boss::takeDamage(int damage) {
    Enemy::takeDamage(damage);
    checkPhaseTransitions();
}

void Boss::checkPhaseTransitions() {
    float healthPercent = static_cast<float>(getHealth()) / static_cast<float>(getMaxHealth());

    for (auto& phase : phases) {
        if (!phase.triggered && healthPercent <= phase.healthThreshold && phase.currentCooldown <= 0) {
            phase.triggered = true;
            phase.currentCooldown = phase.cooldown;
            currentPhase++;

            std::ostringstream oss;
            oss << "[BOSS] " << bossTitle << " " << getName()
                << " переходит в фазу: " << phase.name << "!";
            std::cout << oss.str() << std::endl;

            if (!phase.ability.empty()) {
                std::cout << "[BOSS] Использует способность: " << phase.ability << std::endl;
            }
        }
    }
}

void Boss::useAbility(const std::string& abilityId, Entity* target) {
    for (auto& ability : abilities) {
        if (ability.id == abilityId && ability.currentCooldown <= 0) {
            std::ostringstream oss;
            oss << "[BOSS] " << getName() << " использует " << ability.name << "!";
            std::cout << oss.str() << std::endl;

            if (ability.areaRadius > 0) {
                std::ostringstream aoss;
                aoss << "[BOSS] Атака по площади (" << ability.areaRadius
                     << " клеток, " << ability.damage << " урона)";
                std::cout << aoss.str() << std::endl;
            }

            if (target) {
                target->takeDamage(ability.damage);
            }

            ability.currentCooldown = ability.cooldown;
            return;
        }
    }
}

void Boss::triggerEnrage() {
    if (enraged) return;
    enraged = true;

    setAttackPower(getAttackPower() * 2);

    std::ostringstream oss;
    oss << "[BOSS] " << getName() << " В ОЗЛЮЧЕНИИ! Урон удвоен!";
    std::cout << oss.str() << std::endl;
}

const std::vector<BossPhase>& Boss::getPhases() const {
    return phases;
}

const std::vector<BossAbility>& Boss::getAbilities() const {
    return abilities;
}

int Boss::getCurrentPhase() const {
    return currentPhase;
}

bool Boss::isEnraged() const {
    return enraged;
}

// === Фабрики боссов ===

std::shared_ptr<Boss> Boss::createGoblinKing(int level) {
    int health = 300 + level * 50;
    int attack = 15 + level * 3;

    auto boss = std::make_shared<Boss>("Король Гоблинов", "Повелитель клана", "goblin", health, attack);

    boss->addPhase({"Злобный", 0.7f, "goblin_swarm", 15, 10.0f, 0.0f, false});
    boss->addPhase({"Бешеный", 0.3f, "rage_strike", 30, 8.0f, 0.0f, false});

    boss->addAbility(BossAbility("goblin_swarm", "Гоблинская волна", "Призывает волну гоблинов", 15, 5.0f, 12.0f, 3));
    boss->addAbility(BossAbility("rage_strike", "Удар ярости", "Мощный удар с отбрасыванием", 30, 2.0f, 8.0f));
    boss->addAbility(BossAbility("cunning_strike", "Хитрый удар", "Удар в спину с критическим уроном", 25, 1.0f, 6.0f));

    return boss;
}

std::shared_ptr<Boss> Boss::createDragonLord(int level) {
    int health = 800 + level * 100;
    int attack = 40 + level * 5;

    auto boss = std::make_shared<Boss>("Дракон Лорд", "Пламенный Владыка", "dragon", health, attack);

    boss->addPhase({"Полёт", 0.6f, "fire_breath", 40, 15.0f, 0.0f, false});
    boss->addPhase({"Ярость", 0.25f, "meteor_strike", 60, 12.0f, 0.0f, false});
    boss->addPhase({"Отчаяние", 0.1f, "dragon_roar", 80, 20.0f, 0.0f, false});

    boss->addAbility(BossAbility("fire_breath", "Огненное дыхание", "Конус огня", 40, 8.0f, 15.0f, 4));
    boss->addAbility(BossAbility("meteor_strike", "Удар метеоритом", "Массивный урон по площади", 60, 10.0f, 12.0f, 5));
    boss->addAbility(BossAbility("dragon_roar", "Рёв дракона", "Оглушение и урон", 80, 12.0f, 20.0f, 6));
    boss->addAbility(BossAbility("tail_swipe", "Хвостовой удар", "Быстрый удар хвостом", 25, 3.0f, 4.0f, 2));

    return boss;
}

std::shared_ptr<Boss> Boss::createShadowLich(int level) {
    int health = 500 + level * 70;
    int attack = 25 + level * 4;

    auto boss = std::make_shared<Boss>("Теневой Лич", "Повелитель нежити", "undead", health, attack);

    boss->addPhase({"Некромантия", 0.65f, "summon_skeletons", 0, 18.0f, 0.0f, false});
    boss->addPhase({"Тёмная магия", 0.35f, "shadow_bolt", 35, 10.0f, 0.0f, false});
    boss->addPhase({"Ритуал", 0.15f, "death_grip", 50, 25.0f, 0.0f, false});

    boss->addAbility(BossAbility("summon_skeletons", "Призыв скелетов", "Вызывает скелетов-воинов", 0, 0.0f, 18.0f));
    boss->addAbility(BossAbility("shadow_bolt", "Теневая стрела", "Магический снаряд", 35, 10.0f, 6.0f));
    boss->addAbility(BossAbility("death_grip", "Хватка смерти", "Притягивает и наносит урон", 50, 8.0f, 25.0f, 3));
    boss->addAbility(BossAbility("life_drain", "Вытягивание жизни", "Восстанавливает здоровье", -30, 5.0f, 10.0f));
    boss->addAbility(BossAbility("frost_nova", "Ледянаяnova", "Замораживает врагов", 20, 6.0f, 8.0f, 4));

    return boss;
}
