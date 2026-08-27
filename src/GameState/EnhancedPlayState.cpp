// src/GameState/EnhancedPlayState.cpp
#include "GameState/EnhancedPlayState.h"
#include "AI/AdvancedAI.h"
#include "AI/HeroAI.h"
#include "Core/Engine.h"
#include "Events/CombatEvent.h"
#include "Events/EventManager.h"
#include "Graphics/AdvancedGraphicsSystem.h"
#include "Graphics/UISystem.h"
#include "UI/HeroDiary.h"
#include "World/AdvancedWorldGenerator.h"
#include "World/TileBasedLocation.h"
#include "World/TileMap.h"
#include "Systems/ComboSystem.h"
#include "Systems/DivineSystem.h"
#include "Systems/LootSystem.h"
#include "Entities/Boss.h"
#include "Graphics/SpriteSystem.h"
#include "World/DayNightSystem.h"
#include "Entities/Wolf.h"
#include "Entities/Bear.h"
#include "Entities/Skeleton.h"
#include "Entities/WorldResource.h"
#include "Entities/Merchant.h"
#include "Entities/QuestGiver.h"
#include "Entities/Enemy.h"
#include "Items/Item.h"
#include <iostream>
#include <memory>
#include <cmath>
#include <sstream>

EnhancedPlayState::EnhancedPlayState() : GameState("enhanced_play") {
}

void EnhancedPlayState::enter() {
    Engine* engineInstance = Engine::getInstance();
    if (!engineInstance) {
        std::cerr << "Graphics system not available!" << std::endl;
        return;
    }
    auto* graphicsSystem = engineInstance->getGraphicsSystem();
    if (!graphicsSystem) {
        std::cerr << "Graphics system is not available!" << std::endl;
        return;
    }
    if (!graphicsSystem->getWindow().isOpen()) {
        std::cerr << "Graphics window is not open!" << std::endl;
        return;
    }

    uiSystem = std::make_unique<UISystem>();
    if (!uiSystem->initialize(graphicsSystem->getWindow())) {
        std::cerr << "Failed to initialize UI system!" << std::endl;
        return;
    }

    graphicsSystem->setPostCompositeCallback([this](sf::RenderWindow& win) {
        if (uiSystem) {
            uiSystem->draw(win);
        }
    });

    spriteSystem = std::make_unique<SpriteSystem>();
    spriteSystem->initialize();

    uiSystem->setDivineSystem(&divineSystem);
    uiSystem->setDayNightSystem(&dayNightSystem);
    uiSystem->setHeroAI(&heroAI);

    AdvancedWorldGenerator generator;
    AdvancedWorldGenerator::GenerationParams params;
    params.worldSize = 512;
    params.regionSize = 64;
    params.mountainHeight = 0.8f;
    params.waterLevel = 0.4f;
    params.forestThreshold = 0.3f;
    generator.setParams(params);

    gameWorld = generator.generateWorld("Мир Вечного Странника");
    std::cout << "gameWorld created: " << (gameWorld ? "yes" : "no") << std::endl;
    if (gameWorld) {
        std::cout << "Locations count: " << gameWorld->getLocations().size() << std::endl;
        std::cout << "Current location index: " << gameWorld->getCurrentLocationIndex() << std::endl;
        auto* loc = gameWorld->getCurrentLocation();
        std::cout << "Current location ptr: " << loc << std::endl;
        if (loc) {
            std::cout << "Location name: " << loc->getName() << std::endl;
            auto* tileLoc = dynamic_cast<TileBasedLocation*>(loc);
            std::cout << "Is TileBasedLocation: " << (tileLoc ? "yes" : "no") << std::endl;
            if (tileLoc && tileLoc->getTileMap()) {
                std::cout << "TileMap size: " << tileLoc->getTileMap()->getWidth() << "x" << tileLoc->getTileMap()->getHeight() << std::endl;
            }
        }
    }
    if (!gameWorld) {
        uiSystem->showMessage("Ошибка генерации мира!");
        return;
    }
    std::cout << "Локаций в мире: " << gameWorld->getLocations().size() << std::endl;
    std::cout << "Текущая локация индекс: " << gameWorld->getCurrentLocationIndex() << std::endl;

    player = std::make_shared<Player>("Hero", 100);
    player->addGold(50);
    player->addItem(std::make_shared<Item>("rusty_sword", "Ржавый меч", "Старый, но ещё рабочий меч", ItemType::WEAPON, ItemRarity::COMMON, 5, 3));
    player->addItem(std::make_shared<Item>("bread", "Хлеб", "Кусок хлеба", ItemType::MISC, ItemRarity::COMMON, 3, 1));
    player->addItem(std::make_shared<Item>("health_potion", "Зелье здоровья", "Восстанавливает 25 HP", ItemType::CONSUMABLE, ItemRarity::COMMON, 25, 1));
    player->addItem(std::make_shared<Item>("water_flask", "Фляга воды", "Восстанавливает жажду", ItemType::CONSUMABLE, ItemRarity::COMMON, 3, 1));
    player->addItem(std::make_shared<Item>("water_flask", "Фляга воды", "Восстанавливает жажду", ItemType::CONSUMABLE, ItemRarity::COMMON, 3, 1));

    std::cout << "Player created: " << player->getName() << " HP:" << player->getHealth() << std::endl;

    if (auto* location = gameWorld->getCurrentLocation()) {
        if (auto tileLoc = dynamic_cast<TileBasedLocation*>(location)) {
            auto spawnPos = tileLoc->findSpawnPoint();
            if (spawnPos.first != -1 && spawnPos.second != -1) {
                player->setPosition(spawnPos.first, spawnPos.second);
                player->setCurrentLocation(location);
                location->addEntity(player);
                gameWorld->addEntity(player);
                std::cout << "Игрок 'Hero' создан и добавлен в локацию в (" << spawnPos.first << ", " << spawnPos.second << ")." << std::endl;
            }
            else {
                int centerX = tileLoc->getTileMap()->getWidth() / 2;
                int centerY = tileLoc->getTileMap()->getHeight() / 2;
                if (tileLoc->getTileMap()->isWalkable(centerX, centerY)) {
                    player->setPosition(centerX, centerY);
                    player->setCurrentLocation(location);
                    location->addEntity(player);
                    std::cerr << "FALLBACK: Игрок заспавнен в центре карты (" << centerX << ", " << centerY << ")." << std::endl;
                }
            }
        }
    }

    chunkSystem.setWorldSeed(42);
    lootSystem.initializeDefaultLootTables();
    heroAI.setDivineSystem(&divineSystem);
    heroAI.setTradeSystem(&tradeSystem);
    heroAI.setWorldEntities(&worldEntities);
    heroAI.setHomePosition(
        player->getPosition().first,
        player->getPosition().second);

    {
        auto [hx, hy] = player->getPosition();
        int npcOffsets[][2] = {{3,0},{-3,0},{0,3},{0,-3}};
        auto spawnNPC = [&](int dx, int dy, std::function<std::shared_ptr<NPC>()> factory) {
            int nx = hx + dx, ny = hy + dy;
            if (chunkSystem.isWalkable(nx, ny)) {
                auto npc = factory();
                npc->setPosition(nx, ny);
                worldEntities.push_back(npc);
            }
        };
        spawnNPC(3, 0, []() -> std::shared_ptr<NPC> {
            auto m = std::make_shared<Merchant>("Торговец", 80);
            m->addItem(MerchantItem("iron_sword", "Железный меч", 60, 2));
            m->addItem(MerchantItem("health_potion", "Зелье здоровья", 25, 10));
            m->addItem(MerchantItem("bread", "Хлеб", 5, 10));
            m->addItem(MerchantItem("water_flask", "Фляга воды", 3, 10));
            m->addItem(MerchantItem("leather_armor", "Кожаная броня", 50, 2));
            return m;
        });
        spawnNPC(-3, 0, []() -> std::shared_ptr<NPC> {
            auto s = std::make_shared<Merchant>("Кузнец", 100);
            s->addItem(MerchantItem("steel_sword", "Стальной меч", 250, 1));
            s->addItem(MerchantItem("iron_shield", "Железный щит", 120, 2));
            s->addItem(MerchantItem("repair_kit", "Ремкомплект", 30, 5));
            return s;
        });
        spawnNPC(0, 3, []() -> std::shared_ptr<NPC> {
            auto a = std::make_shared<Merchant>("Алхимик", 70);
            a->addItem(MerchantItem("greater_health_potion", "Большое зелье здоровья", 80, 5));
            a->addItem(MerchantItem("antidote", "Противоядие", 40, 10));
            a->addItem(MerchantItem("strength_elixir", "Эликсир силы", 150, 2));
            return a;
        });
        spawnNPC(0, -3, []() -> std::shared_ptr<NPC> {
            return std::make_shared<QuestGiver>("Староста");
        });
    }

    uiSystem->setChunkSystem(&chunkSystem);
    uiSystem->setWorldEntities(&worldEntities);
    uiSystem->setHeroEntity(player.get());
    uiSystem->showMinimap();
    if (auto* diary = uiSystem->getHeroDiary()) {
        diary->addEntry(1, "Утро", "Герой пробуждается в деревне. Начинается путешествие...", sf::Color(200, 220, 255));
    }

    auto setupAbility = [this](DivineAbilityType type, std::function<void()> fn) {
        const_cast<DivineAbility*>(divineSystem.getAbility(type))->execute = fn;
    };

    setupAbility(DivineAbilityType::BLESS_POWER, [this]() {
        if (player) {
            heroAI.getEmotions().addBravery(30.0f);
            heroAI.onDivineBlessing();
            if (uiSystem) uiSystem->showMessage("Бог благословил силу героя!");
        }
    });

    setupAbility(DivineAbilityType::BLESS_HEALTH, [this]() {
        if (player) {
            int heal = player->getMaxHealth() / 4;
            player->setHealth(std::min(player->getHealth() + heal, player->getMaxHealth()));
            heroAI.onDivineBlessing();
            if (uiSystem) uiSystem->showMessage("Бог исцеляет героя!");
        }
    });

    setupAbility(DivineAbilityType::LIGHTNING, [this]() {
        if (!player || !player->isAlive()) return;
        if (uiSystem) uiSystem->showMessage("Молния поражает врагов!");
        for (auto& entity : worldEntities) {
            if (entity && entity->isAlive()) {
                auto [ex, ey] = entity->getPosition();
                auto [px, py] = player->getPosition();
                if (std::abs(ex - px) + std::abs(ey - py) < 30) {
                    entity->takeDamage(20 + player->getLevel() * 2);
                    std::cout << "[DIVINE] Lightning strikes " << entity->getName() << "!" << std::endl;
                }
            }
        }
    });

    setupAbility(DivineAbilityType::WEATHER_CONTROL, [this]() {
        int roll = std::rand() % 5;
        WeatherType newWeather;
        const char* names[] = { "Ясно", "Дождь", "Шторм", "Снег", "Туман" };
        newWeather = static_cast<WeatherType>(roll);
        dayNightSystem.forceWeather(newWeather);
        if (uiSystem) {
            std::ostringstream ss;
            ss << "Бог меняет погоду на: " << names[roll];
            uiSystem->showMessage(ss.str());
        }
    });

    setupAbility(DivineAbilityType::EMOTION_CONTROL, [this]() {
        heroAI.getEmotions().triggerBravery(30.0f);
        if (uiSystem) uiSystem->showMessage("Бог дарует герою смелость!");
    });

    setupAbility(DivineAbilityType::ENEMY_DISTRACT, [this]() {
        if (!player || !player->isAlive()) return;
        int distracted = 0;
        for (auto& entity : worldEntities) {
            if (entity && entity->isAlive() && dynamic_cast<Enemy*>(entity.get())) {
                auto [ex, ey] = entity->getPosition();
                auto [px, py] = player->getPosition();
                if (std::abs(ex - px) + std::abs(ey - py) < 20) {
                    int pushX = ex + (ex - px) * 3;
                    int pushY = ey + (ey - py) * 3;
                    entity->setPosition(pushX, pushY);
                    distracted++;
                }
            }
        }
        if (uiSystem) {
            std::ostringstream ss;
            ss << "Бог отвлекает врагов! Отброшено: " << distracted;
            uiSystem->showMessage(ss.str());
        }
    });

    setupAbility(DivineAbilityType::TIME_MANIPULATION, [this]() {
        float currentSpeed = dayNightSystem.getDayLength();
        dayNightSystem.setTimeSpeed(currentSpeed * 3.0f);
        heroAI.getEmotions().addJoy(5.0f);
        if (uiSystem) {
            uiSystem->showMessage("Бог ускоряет время! День пролетает быстрее...");
        }
        std::cout << "[DIVINE] Time accelerated 3x" << std::endl;
    });

    setupAbility(DivineAbilityType::DIRECTION_GUIDE, [this]() {
        if (!player || !uiSystem) return;
        auto [px, py] = player->getPosition();
        std::string nearestEnemyName;
        int nearestEnemyDist = 9999;
        for (auto& entity : worldEntities) {
            if (!entity || !entity->isAlive()) continue;
            if (!dynamic_cast<Enemy*>(entity.get())) continue;
            auto [ex, ey] = entity->getPosition();
            int dist = std::abs(ex - px) + std::abs(ey - py);
            if (dist < nearestEnemyDist) {
                nearestEnemyDist = dist;
                nearestEnemyName = entity->getName();
            }
        }
        std::ostringstream ss;
        if (nearestEnemyDist < 9999) {
            int dx = 0, dy = 0;
            for (auto& entity : worldEntities) {
                if (!entity || !entity->isAlive()) continue;
                if (entity->getName() != nearestEnemyName) continue;
                auto [ex, ey] = entity->getPosition();
                dx = ex - px; dy = ey - py;
                break;
            }
            const char* dirX = (dx > 0) ? "восток" : (dx < 0) ? "запад" : "";
            const char* dirY = (dy > 0) ? "юг" : (dy < 0) ? "север" : "";
            ss << "Божий указ: ближайший враг " << nearestEnemyName
               << " на расстоянии " << nearestEnemyDist
               << " тайлов к " << dirX << (dx != 0 && dy != 0 ? "-" : "") << dirY;
        } else {
            ss << "Бог указывает: опасностей рядом нет, исследуй мир!";
        }
        uiSystem->showMessage(ss.str());
        heroAI.onPray();
    });

    if (player) {
        auto [px, py] = player->getPosition();
        int startChunkX = chunkSystem.worldToChunkCoord(px);
        int startChunkY = chunkSystem.worldToChunkCoord(py);
        chunkSystem.updateChunks(startChunkX, startChunkY);
    }

    // Spawn initial entities
    if (player) {
        auto [px, py] = player->getPosition();
        int chunkX = chunkSystem.worldToChunkCoord(px);
        int chunkY = chunkSystem.worldToChunkCoord(py);
        spawnEntitiesInChunk(chunkX, chunkY);
    }

    initializeAI();

    if (player && player->getAI()) {
        AdvancedAI* playerAI = dynamic_cast<AdvancedAI*>(player->getAI());
        if (playerAI && playerAI->getGoalSystem()) {
            auto* goalSystem = playerAI->getGoalSystem();

            Goal healGoal;
            healGoal.id = "heal_when_low";
            healGoal.priority = 1.0f;
            healGoal.condition = [](Entity* e) {
                return e && e->getHealth() < e->getMaxHealth() * 0.3f;
                };
            healGoal.action = [](Entity* e) {
                if (auto* ai = dynamic_cast<AdvancedAI*>(e->getAI())) {
                    ai->setState(AIState::SOCIALIZE);
                    std::cout << e->getName() << " ищет способ исцелиться!" << std::endl;
                }
                };
            goalSystem->addGoal(healGoal);

            Goal avoidStrongEnemies;
            avoidStrongEnemies.id = "avoid_strong_enemies";
            avoidStrongEnemies.priority = 0.9f;
            avoidStrongEnemies.condition = [this](Entity* e) {
                if (!e || !this->gameWorld) return false;
                auto* ai = e->getAI();
                if (!ai) return false;
                if (auto* advancedAI = dynamic_cast<AdvancedAI*>(ai)) {
                    const auto& memories = advancedAI->getMemorySystem().getMemories();
                    for (const auto& mem : memories) {
                        if ((mem.eventType == "seen" || mem.eventType == "attacked_by") && mem.entity.lock()) {
                            auto enemy = mem.entity.lock();
                            if (!enemy || !enemy->isAlive()) continue;
                            int playerLevel = e->getLevel();
                            int enemyLevel = enemy->getLevel();
                            if (enemyLevel > playerLevel * 1.5f) {
                                return true;
                            }
                        }
                    }
                }
                return false;
                };
            avoidStrongEnemies.action = [](Entity* e) {
                if (auto* ai = dynamic_cast<AdvancedAI*>(e->getAI())) {
                    ai->setState(AIState::FLEE);
                    std::cout << e->getName() << " убегает от сильного врага!" << std::endl;
                }
                };
            goalSystem->addGoal(avoidStrongEnemies);

            Goal seekLevelAppropriateEnemies;
            seekLevelAppropriateEnemies.id = "seek_training_targets";
            seekLevelAppropriateEnemies.priority = 0.7f;
            seekLevelAppropriateEnemies.condition = [](Entity* e) {
                if (auto* player = dynamic_cast<Player*>(e)) {
                    return player->getExperience() < player->getExperienceToNextLevel() * 0.9f;
                }
                return false;
                };
            seekLevelAppropriateEnemies.action = [](Entity* e) {
                if (auto* ai = dynamic_cast<AdvancedAI*>(e->getAI())) {
                    ai->setState(AIState::WANDER);
                    std::cout << e->getName() << " ищет врагов для прокачки!" << std::endl;
                }
                };
            goalSystem->addGoal(seekLevelAppropriateEnemies);

            Goal exploreWorld;
            exploreWorld.id = "explore_world";
            exploreWorld.priority = 0.5f;
            exploreWorld.condition = [this](Entity* e) {
                if (!e || !this->gameWorld) return false;
                auto* loc = this->gameWorld->getCurrentLocation();
                if (!loc) return false;
                auto* tileLoc = dynamic_cast<TileBasedLocation*>(loc);
                if (!tileLoc || !tileLoc->getTileMap()) return false;
                auto [x, y] = e->getPosition();
                int worldSize = tileLoc->getTileMap()->getWidth();
                int viewRadius = 200;
                return (x < viewRadius || x > worldSize - viewRadius ||
                    y < viewRadius || y > worldSize - viewRadius);
                };
            exploreWorld.action = [](Entity* e) {
                if (auto* ai = dynamic_cast<AdvancedAI*>(e->getAI())) {
                    ai->setState(AIState::WANDER);
                    std::cout << e->getName() << " исследует границы мира!" << std::endl;
                }
                };
            goalSystem->addGoal(exploreWorld);
        }
    }

    uiSystem->showDialog("greeting", player.get(), nullptr);
    uiSystem->showMessage("Добро пожаловать в Мир Вечного Странника!");
}

void EnhancedPlayState::exit() {
    if (uiSystem) {
        uiSystem->shutdown();
    }
    gameWorld.reset();
    player.reset();
}

void EnhancedPlayState::update(float deltaTime) {
    if (paused) return;
    timeAccumulator += deltaTime;

    if (gameWorld) {
        gameWorld->update(deltaTime);
    }

    seasonSystem.update(deltaTime);
    divineSystem.update(deltaTime);
    dayNightSystem.update(deltaTime);

    float weatherMod = 1.0f;
    switch (dayNightSystem.getWeather()) {
        case WeatherType::RAIN:  weatherMod = 0.85f; break;
        case WeatherType::STORM: weatherMod = 0.65f; break;
        case WeatherType::SNOW:  weatherMod = 0.75f; break;
        case WeatherType::FOG:   weatherMod = 0.90f; break;
        default: break;
    }
    heroAI.setWeatherSpeedMod(weatherMod);

    lootSystem.update(deltaTime);
    craftingSystem.update(deltaTime);
    if (player && player->isAlive()) {
        lootSystem.collectNearbyLoot(player.get(), 3.0f);

        for (auto& entity : worldEntities) {
            if (!entity || !entity->isAlive()) continue;
            auto* res = dynamic_cast<WorldResource*>(entity.get());
            if (!res || !res->canGather()) continue;
            auto [ex, ey] = entity->getPosition();
            auto [px, py] = player->getPosition();
            int dist = std::abs(px - ex) + std::abs(py - ey);
            if (dist <= 2) {
                if (res->getResourceType() == ResourceType::BUSH || res->getResourceType() == ResourceType::MUSHROOM) {
                    int gathered = res->gather(1);
                    if (gathered > 0) {
                        std::string itemId, itemName;
                        if (res->getResourceType() == ResourceType::BUSH) {
                            itemId = "berry"; itemName = "Ягоды";
                        } else {
                            itemId = "mushroom"; itemName = "Гриб";
                        }
                        auto item = std::make_shared<Item>(itemId, itemName, "Собрано вручную", ItemType::CONSUMABLE);
                        player->addItem(item);
                        if (heroAI.getNeeds().needsFood()) {
                            heroAI.getNeeds().eat(25.0f);
                        }
                        heroAI.onItemFound();
                        if (uiSystem) uiSystem->showMessage("Герой собрал " + itemName + "!");
                        if (uiSystem && uiSystem->getHeroDiary()) {
                            int day = dayNightSystem.getDay();
                            const char* tn[] = { "Рассвет", "День", "Закат", "Ночь" };
                            uiSystem->getHeroDiary()->addEntry(day, tn[static_cast<int>(dayNightSystem.getTimeOfDay())],
                                "Собрал " + itemName, sf::Color(150, 220, 150));
                        }
                    }
                } else if (res->getResourceType() == ResourceType::ROCK) {
                    int gathered = res->gather(1);
                    if (gathered > 0) {
                        auto item = std::make_shared<Item>("stone", "Камень", "Добыт из скалы", ItemType::MISC);
                        player->addItem(item);
                        player->addGold(1);
                        if (uiSystem) uiSystem->showMessage("Герой добыл камень!");
                        if (uiSystem && uiSystem->getHeroDiary()) {
                            int day = dayNightSystem.getDay();
                            const char* tn[] = { "Рассвет", "День", "Закат", "Ночь" };
                            uiSystem->getHeroDiary()->addEntry(day, tn[static_cast<int>(dayNightSystem.getTimeOfDay())],
                                "Добыл камень", sf::Color(170, 170, 170));
                        }
                    }
                } else if (res->getResourceType() == ResourceType::TREE) {
                    int gathered = res->gather(1);
                    if (gathered > 0) {
                        auto item = std::make_shared<Item>("wood", "Дерево", "Срублено дерево", ItemType::MISC);
                        player->addItem(item);
                        if (uiSystem) uiSystem->showMessage("Герой срубил дерево!");
                        if (uiSystem && uiSystem->getHeroDiary()) {
                            int day = dayNightSystem.getDay();
                            const char* tn[] = { "Рассвет", "День", "Закат", "Ночь" };
                            uiSystem->getHeroDiary()->addEntry(day, tn[static_cast<int>(dayNightSystem.getTimeOfDay())],
                                "Срубил дерево", sf::Color(139, 90, 43));
                        }
                    }
                } else if (res->getResourceType() == ResourceType::CRYSTAL) {
                    int gathered = res->gather(1);
                    if (gathered > 0) {
                        auto item = std::make_shared<Item>("crystal", "Кристалл", "Редкий кристалл", ItemType::MISC, ItemRarity::RARE);
                        player->addItem(item);
                        player->addGold(5);
                        if (uiSystem) uiSystem->showMessage("Герой нашёл кристалл!");
                        if (uiSystem && uiSystem->getHeroDiary()) {
                            int day = dayNightSystem.getDay();
                            const char* tn[] = { "Рассвет", "День", "Закат", "Ночь" };
                            uiSystem->getHeroDiary()->addEntry(day, tn[static_cast<int>(dayNightSystem.getTimeOfDay())],
                                "Нашёл кристалл! +5 золота", sf::Color(200, 150, 255));
                        }
                    }
                }
            }
        }
    }
    updateWorldEntities(deltaTime);
    heroAI.update(deltaTime, player.get());

    if (player && player->isAlive()) {
        heroAI.getNeeds().setHealth(static_cast<float>(player->getHealth()));
        if (heroAI.getNeeds().getHealthState() == NeedState::CRITICAL && player->getHealth() > 1) {
            player->setHealth(1);
        }

        WeatherType weather = dayNightSystem.getWeather();
        if (weather == WeatherType::STORM) {
            int stormDmg = 1;
            player->takeDamage(stormDmg);
            if (uiSystem && std::rand() % 300 == 0) uiSystem->showMessage("Шторм наносит урон!");
        }
        if (weather == WeatherType::RAIN) {
            heroAI.getNeeds().addThirst(0.5f * (1.0f / 60.0f));
        }
    }

    auto heroEvents = heroAI.drainEventLog();
    if (!heroEvents.empty() && uiSystem && uiSystem->getHeroDiary()) {
        auto& diary = *uiSystem->getHeroDiary();
        int day = dayNightSystem.getDay();
        const char* timeNames[] = { "Рассвет", "День", "Закат", "Ночь" };
        int tIdx = static_cast<int>(dayNightSystem.getTimeOfDay());
        for (const auto& [text, color] : heroEvents) {
            diary.addEntry(day, timeNames[tIdx], text, color);
        }
    }

    if (uiSystem) {
        uiSystem->update(deltaTime);
    }

    updateChunksAroundPlayer();
}

void EnhancedPlayState::updateChunksAroundPlayer() {
    if (!player) return;
    auto [px, py] = player->getPosition();
    int chunkX = chunkSystem.worldToChunkCoord(px);
    int chunkY = chunkSystem.worldToChunkCoord(py);
    chunkSystem.updateChunks(chunkX, chunkY);

    respawnTimer += 1.0f / 60.0f;
    if (respawnTimer >= 10.0f) {
        respawnTimer = 0.0f;
        int range = 3;
        for (int cx = chunkX - range; cx <= chunkX + range; cx++) {
            for (int cy = chunkY - range; cy <= chunkY + range; cy++) {
                int chunkSize = chunkSystem.getChunkSize();
                int baseX = cx * chunkSize;
                int baseY = cy * chunkSize;
                int entityCount = 0;
                for (auto& e : worldEntities) {
                    if (!e || !e->isAlive()) continue;
                    auto [ex, ey] = e->getPosition();
                    if (ex >= baseX && ex < baseX + chunkSize && ey >= baseY && ey < baseY + chunkSize) {
                        entityCount++;
                    }
                }
                if (entityCount < 2) {
                    spawnEntitiesInChunk(cx, cy);
                }
            }
        }
    }
}

void EnhancedPlayState::updateWorldEntities(float deltaTime) {
    for (auto& entity : worldEntities) {
        if (entity && entity->isAlive()) {
            entity->update(deltaTime);
        }
    }

    if (player && player->isAlive()) {
        auto [px, py] = player->getPosition();
        float enemyAggro = dayNightSystem.getEnemyAggression();

        for (auto& entity : worldEntities) {
            if (!entity || !entity->isAlive()) continue;
            auto [ex, ey] = entity->getPosition();
            int dist = std::abs(px - ex) + std::abs(py - ey);

            Enemy* enemy = dynamic_cast<Enemy*>(entity.get());
            if (enemy && dist < 3) {
                int enemyDamage = static_cast<int>(enemy->getAttackPower() * enemyAggro);
                int armorBonus = 0;
                for (const auto& item : player->getInventory()) {
                    if (item && (item->getId() == "leather_armor" || item->getId() == "leather_scrap")) armorBonus += 1;
                    if (item && (item->getId() == "steel_armor" || item->getId() == "chainmail")) armorBonus += 3;
                    if (item && item->getId() == "iron_shield") armorBonus += 2;
                }
                enemyDamage = std::max(1, enemyDamage - armorBonus);
                player->takeDamage(enemyDamage);
                heroAI.onDamageTaken(static_cast<float>(enemyDamage));
                std::cout << "[COMBAT] " << entity->getName() << " атакует " << player->getName()
                          << " на " << enemyDamage << " урона!" << std::endl;
            }

            if (heroAI.hasActiveGoal() && heroAI.getCurrentGoal().type == HeroGoalType::FIGHT && dist < 5) {
                int heroDamage = player->getLevel() * 3 + 5;
                int weaponBonus = 0;
                for (const auto& item : player->getInventory()) {
                    if (item && item->getId() == "rusty_sword") weaponBonus += 3;
                    if (item && item->getId() == "iron_sword") weaponBonus += 8;
                    if (item && item->getId() == "steel_sword") weaponBonus += 15;
                    if (item && item->getId() == "war_hammer") weaponBonus += 12;
                }
                heroDamage += weaponBonus;
                float dmgMod = heroAI.getEmotions().getAttackDamageModifier() * heroAI.getNeeds().getAttackDamageModifier();
                heroDamage = std::max(1, static_cast<int>(heroDamage * dmgMod));
                entity->takeDamage(heroDamage);
                std::cout << "[COMBAT] " << player->getName() << " атакует " << entity->getName()
                          << " на " << heroDamage << " урона!" << std::endl;

                if (!entity->isAlive()) {
                    heroAI.onEnemyKilled();
                    int xp = 10 + entity->getLevel() * 3;
                    int oldLevel = player->getLevel();
                    player->gainExperience(xp);

                    int goldReward = 2 + entity->getLevel() * 2;
                    player->addGold(goldReward);

                    lootSystem.spawnLootOnGround(
                        enemy ? enemy->getEnemyType() : "unknown", player->getLevel(),
                        static_cast<float>(ex), static_cast<float>(ey));
                    std::cout << "[COMBAT] " << entity->getName() << " побеждён! +" << xp << " XP, +" << goldReward << " золота" << std::endl;

                    if (uiSystem && uiSystem->getHeroDiary()) {
                        auto& diary = *uiSystem->getHeroDiary();
                        int day = dayNightSystem.getDay();
                        const char* timeNames[] = { "Рассвет", "День", "Закат", "Ночь" };
                        int tIdx = static_cast<int>(dayNightSystem.getTimeOfDay());
                        std::ostringstream entry;
                        entry << "Победил " << entity->getName() << "! +" << xp << " XP, +" << goldReward << " золота";
                        diary.addEntry(day, timeNames[tIdx], entry.str(), sf::Color(120, 255, 120));
                    }

                    if (player->getLevel() > oldLevel) {
                        heroAI.setExperienceLevel(player->getLevel());
                        if (uiSystem) {
                            std::ostringstream lvlSS;
                            lvlSS << "УРОВЕНЬ " << player->getLevel() << "! Здоровье восстановлено!";
                            uiSystem->showMessage(lvlSS.str());
                        }
                        if (uiSystem && uiSystem->getHeroDiary()) {
                            int day = dayNightSystem.getDay();
                            const char* timeNames2[] = { "Рассвет", "День", "Закат", "Ночь" };
                            int tIdx2 = static_cast<int>(dayNightSystem.getTimeOfDay());
                            std::ostringstream lvlEntry;
                            lvlEntry << "Уровень " << player->getLevel() << "! Стал сильнее!";
                            uiSystem->getHeroDiary()->addEntry(day, timeNames2[tIdx2], lvlEntry.str(), sf::Color(255, 220, 50));
                        }
                        std::cout << "[LEVELUP] " << player->getName() << " reached level "
                                  << player->getLevel() << "!" << std::endl;
                    }
                }
            }
        }
    }

    // Despawn dead entities and entities far from player
    int despawnDist = 200;
    worldEntities.erase(
        std::remove_if(worldEntities.begin(), worldEntities.end(),
            [&](const std::shared_ptr<Entity>& e) {
                if (!e || !e->isAlive()) return true;
                if (!player) return false;
                auto [px, py] = player->getPosition();
                auto [ex, ey] = e->getPosition();
                int dist = std::abs(px - ex) + std::abs(py - ey);
                return dist > despawnDist;
            }),
        worldEntities.end());

    if (player && !player->isAlive()) {
        std::cout << "[DEATH] " << player->getName() << " погиб!" << std::endl;
        int xpLoss = player->getExperience() / 4;
        int goldLoss = player->getGold() / 4;
        player->setExperience(std::max(0, player->getExperience() - xpLoss));
        player->setGold(std::max(0, player->getGold() - goldLoss));
        player->setHealth(player->getMaxHealth());
        player->setPosition(50, 50);
        heroAI.getEmotions().triggerFear(50.0f);
        heroAI.setExperienceLevel(player->getLevel());
        if (uiSystem) {
            std::ostringstream deathSS;
            deathSS << "Герой погиб! -" << xpLoss << " XP, -" << goldLoss << " золота. Респавн...";
            uiSystem->showMessage(deathSS.str());
        }
        if (uiSystem && uiSystem->getHeroDiary()) {
            int day = dayNightSystem.getDay();
            const char* timeNames3[] = { "Рассвет", "День", "Закат", "Ночь" };
            int tIdx3 = static_cast<int>(dayNightSystem.getTimeOfDay());
            uiSystem->getHeroDiary()->addEntry(day, timeNames3[tIdx3],
                "Погиб! Потерял золото и опыт. Очнулся у дома...", sf::Color(255, 80, 80));
        }
    }

    worldEntities.erase(
        std::remove_if(worldEntities.begin(), worldEntities.end(),
            [this](const std::shared_ptr<Entity>& e) {
                if (!e || !e->isAlive()) return true;
                if (!player) return false;
                std::string etype = e->getType();
                if (etype == "npc" || etype == "villager") return false;
                auto [ex, ey] = e->getPosition();
                auto [px, py] = player->getPosition();
                return std::abs(ex - px) > 600 || std::abs(ey - py) > 600;
            }),
        worldEntities.end());
}

void EnhancedPlayState::spawnEntitiesInChunk(int chunkX, int chunkY) {
    int chunkSize = chunkSystem.getChunkSize();
    int baseX = chunkX * chunkSize;
    int baseY = chunkY * chunkSize;
    int playerLevel = player ? player->getLevel() : 1;
    int enemyLevel = std::max(1, playerLevel + (std::rand() % 3) - 1);

    Location* sharedLoc = player ? player->getCurrentLocation() : nullptr;

    auto spawnNearPlayer = [&](int radius, auto creator) {
        if (!player) return;
        auto [px, py] = player->getPosition();
        for (int attempt = 0; attempt < 20; attempt++) {
            int x = px + (std::rand() % (radius * 2 + 1)) - radius;
            int y = py + (std::rand() % (radius * 2 + 1)) - radius;
            if (x == px && y == py) continue;
            if (chunkSystem.isWalkable(x, y)) {
                auto ent = creator(enemyLevel);
                ent->setPosition(x, y);
                if (sharedLoc) ent->setCurrentLocation(sharedLoc);
                worldEntities.push_back(ent);
                return;
            }
        }
    };

    int closeRadius = 8;

    spawnNearPlayer(closeRadius, [](int lvl) -> std::shared_ptr<Entity> {
        return std::make_shared<Wolf>(lvl);
    });

    if (std::rand() % 100 < 10) {
        spawnNearPlayer(closeRadius, [](int lvl) -> std::shared_ptr<Entity> {
            return std::make_shared<Bear>(lvl);
        });
    }

    if (dayNightSystem.shouldSpawnNightEnemies() && std::rand() % 100 < 15) {
        spawnNearPlayer(closeRadius, [](int lvl) -> std::shared_ptr<Entity> {
            auto s = std::make_shared<Skeleton>(lvl);
            s->setNightSpawned(true);
            return s;
        });
    }

    for (int i = 0; i < 4; i++) {
        int x = baseX + (std::rand() % chunkSize);
        int y = baseY + (std::rand() % chunkSize);
        if (chunkSystem.isWalkable(x, y)) {
            int typeRoll = std::rand() % 100;
            ResourceType rt = (typeRoll < 40) ? ResourceType::TREE :
                              (typeRoll < 65) ? ResourceType::ROCK :
                              (typeRoll < 80) ? ResourceType::BUSH :
                              (typeRoll < 95) ? ResourceType::MUSHROOM :
                              ResourceType::CRYSTAL;
            auto res = std::make_shared<WorldResource>(rt, x, y);
            worldEntities.push_back(res);
        }
    }
}

void EnhancedPlayState::initializeAI() {
    AIPersonality enemyPersonality;
    enemyPersonality.aggression = 0.9f;
    enemyPersonality.curiosity = 0.1f;
    enemyPersonality.sociability = 0.1f;
    enemyPersonality.caution = 0.7f;

    auto enemyAI = std::make_unique<AdvancedAI>(enemyPersonality);
    aiBehaviors["enemy"] = std::move(enemyAI);
}

void EnhancedPlayState::render() {
    auto* engine = Engine::getInstance();
    if (!engine) return;
    auto* graphics = engine->getGraphicsSystem();
    if (!graphics) return;

    if (player && player->isAlive()) {
        std::vector<std::shared_ptr<Entity>> renderEntities;
        renderEntities.push_back(player);
        for (const auto& ent : worldEntities) {
            if (ent && ent->isAlive()) {
                renderEntities.push_back(ent);
            }
        }
        graphics->renderFromChunks(player, renderEntities, chunkSystem);

        graphics->renderDayNightOverlay(dayNightSystem.getTimeOfDay(), dayNightSystem.getWeather());

        std::vector<std::pair<float, float>> lootPositions;
        for (const auto& item : lootSystem.getGroundItems()) {
            if (!item.collected) {
                lootPositions.emplace_back(item.worldX, item.worldY);
            }
        }
        graphics->renderLootDrops(lootPositions);
    }
}

void EnhancedPlayState::handleInput() {
    handlePlayerInput();
}

void EnhancedPlayState::handleEvent(const sf::Event& event) {
    if (uiSystem) {
        uiSystem->handleEvent(event);
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::G) {
            showDivinePowers();
        } else if (event.key.code == sf::Keyboard::I) {
            showInventory();
        } else if (event.key.code == sf::Keyboard::C) {
            if (uiSystem && uiSystem->isCraftingMenuVisible()) {
                uiSystem->hideCraftingMenu();
            } else {
                showCrafting();
            }
        } else         if (event.key.code == sf::Keyboard::Escape) {
            if (uiSystem) {
                if (uiSystem->isGodPanelVisible()) uiSystem->hideGodPanel();
                else if (uiSystem->isCraftingMenuVisible()) uiSystem->hideCraftingMenu();
                else if (uiSystem->isInventoryVisible()) uiSystem->hideInventory();
                else if (uiSystem->isTradeMenuVisible()) uiSystem->hideTradeMenu();
                else if (uiSystem->isDiaryVisible()) uiSystem->hideDiary();
                else togglePause();
            }
        } else if (event.key.code == sf::Keyboard::M) {
            if (uiSystem) uiSystem->toggleMinimap();
        } else if (event.key.code == sf::Keyboard::J) {
            if (uiSystem) uiSystem->toggleDiary();
        } else if (event.key.code == sf::Keyboard::E) {
            if (player && player->isAlive()) {
                auto [px, py] = player->getPosition();
                for (auto& entity : worldEntities) {
                    if (!entity || !entity->isAlive()) continue;
                    std::string type = entity->getType();
                    if (type != "npc" && type != "villager") continue;
                    auto [ex, ey] = entity->getPosition();
                    int dist = std::abs(px - ex) + std::abs(py - ey);
                    if (dist <= 3) {
                        if (auto* merchant = dynamic_cast<Merchant*>(entity.get())) {
                            std::string tradeId = "village_merchant";
                            if (merchant->getName() == "Кузнец") tradeId = "blacksmith";
                            else if (merchant->getName() == "Алхимик") tradeId = "alchemist";
                            showTrade(tradeId);
                            if (uiSystem) uiSystem->showMessage(merchant->getDialogue());
                            heroAI.onNPCInteraction();
                            if (uiSystem && uiSystem->getHeroDiary()) {
                                int day = dayNightSystem.getDay();
                                const char* tn[] = { "Рассвет", "День", "Закат", "Ночь" };
                                uiSystem->getHeroDiary()->addEntry(day, tn[static_cast<int>(dayNightSystem.getTimeOfDay())],
                                    "Встретил торговца " + merchant->getName(), sf::Color(220, 200, 100));
                            }
                        } else if (auto* questGiver = dynamic_cast<QuestGiver*>(entity.get())) {
                            if (uiSystem) uiSystem->showMessage(questGiver->getDialogue());
                            heroAI.onNPCInteraction();
                        } else {
                            if (uiSystem) {
                                auto* npc = dynamic_cast<NPC*>(entity.get());
                                if (npc) uiSystem->showMessage(npc->getDialogue());
                            }
                            heroAI.onNPCInteraction();
                        }
                        std::cout << "[INTERACT] " << player->getName() << " interacted with " << entity->getName() << std::endl;
                        break;
                    }
                }
            }
        }
    }
}

void EnhancedPlayState::handlePlayerInput() {
}

void EnhancedPlayState::togglePause() {
    paused = !paused;
}

void EnhancedPlayState::showInventory() {
    if (uiSystem) {
        uiSystem->showInventory();
    }
}

void EnhancedPlayState::showSkills() {
    if (uiSystem) {
        uiSystem->showMessage("Система навыков в разработке...");
    }
}

void EnhancedPlayState::showMap() {
    if (uiSystem) {
        uiSystem->showMessage("Карта мира в разработке...");
    }
}

void EnhancedPlayState::showCrafting() {
    if (uiSystem && player) {
        uiSystem->showCraftingMenu(player.get(), craftingSystem);
    }
}

void EnhancedPlayState::showTrade(const std::string& npcId) {
    if (uiSystem && player) {
        uiSystem->showTradeMenu(player.get(), tradeSystem, npcId);
    }
}

CombatSystem& EnhancedPlayState::getCombatSystem() {
    return combatSystem;
}

CraftingSystem& EnhancedPlayState::getCraftingSystem() {
    return craftingSystem;
}

TradeSystem& EnhancedPlayState::getTradeSystem() {
    return tradeSystem;
}

ChunkSystem& EnhancedPlayState::getChunkSystem() {
    return chunkSystem;
}

SeasonSystem& EnhancedPlayState::getSeasonSystem() {
    return seasonSystem;
}

void EnhancedPlayState::showDivinePowers() {
    if (uiSystem) {
        uiSystem->toggleGodPanel(divineSystem, player.get());
    }
}

DivineSystem& EnhancedPlayState::getDivineSystem() {
    return divineSystem;
}

HeroAI& EnhancedPlayState::getHeroAI() {
    return heroAI;
}

DayNightSystem& EnhancedPlayState::getDayNightSystem() { return dayNightSystem; }

void EnhancedPlayState::renderDivineUI() {
}

void EnhancedPlayState::updateDivineAbilities() {
}

EnhancedPlayState::~EnhancedPlayState() = default;
