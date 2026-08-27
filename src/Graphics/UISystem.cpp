// src/UI/UISystem.cpp
#include "UISystem.h"
#include "Core/Engine.h"
#include "Graphics/AdvancedGraphicsSystem.h"
#include "UI/InventoryUI.h"
#include "UI/QuestLogUI.h"
#include "UI/SkillTreeUI.h"
#include "UI/DialogSystem.h"
#include "UI/GodPanelUI.h"
#include "UI/MinimapSystem.h"
#include "UI/HeroDiary.h"
#include "Entities/Player.h"
#include "Entities/Entity.h"
#include "Systems/CraftingSystem.h"
#include "Systems/TradeSystem.h"
#include "Systems/DivineSystem.h"
#include "Items/Item.h"
#include "World/DayNightSystem.h"
#include "AI/HeroAI.h"
#include <iostream>
#include <sstream> 
#include <iomanip>  
#include <memory>
UISystem::UISystem()
    : initialized(false), window(nullptr), playerEntity(nullptr),
      divineSystemPtr(nullptr), dayNightSystemPtr(nullptr), heroAIPtr(nullptr),
      craftingMenuVisible(false), tradeMenuVisible(false), godPanelVisible(false),
      tradePlayer(nullptr), craftPlayer(nullptr), tradeSystemPtr(nullptr),
      craftingSystemPtr(nullptr), tradeScrollOffset(0), craftScrollOffset(0),
      minimapVisible(false), diaryVisible(false),
      chunkSystemPtr(nullptr), worldEntitiesPtr(nullptr), heroEntityPtr(nullptr) {
}

UISystem::~UISystem() {
    shutdown();
}

bool UISystem::initialize(sf::RenderWindow& targetWindow) {
    if (initialized) {
        std::cerr << "UISystem уже инициализирована!" << std::endl;
        return true;
    }
    window = &targetWindow; // Используем переданное окно
    loadFont();
    setupHUD();
    // Инициализация UI панелей
    inventoryUI = std::make_unique<InventoryUI>(font);
    questLogUI = std::make_unique<QuestLogUI>(font);
    // ИСПРАВЛЕНО: передаем оба аргумента - font и window
    skillTreeUI = std::make_unique<SkillTreeUI>(font, window);
    // Получаем окно через Engine
    auto* engine = Engine::getInstance();
    if (!engine || !engine->getGraphicsSystem() || !engine->getGraphicsSystem()->isWindowOpen()) {
        std::cerr << "Ошибка: Engine или GraphicsSystem не инициализированы!" << std::endl;
        shutdown();
        return false;
    }
    // Передаем window, которое уже является graphicsWindow
    dialogSystem = std::make_unique<DialogSystem>(*window, font);

    godPanelUI = std::make_unique<GodPanelUI>();
    godPanelUI->initialize(*window, font);

    minimapSystem = std::make_unique<MinimapSystem>();
    minimapSystem->initialize(font);
    minimapSystem->setPosition(static_cast<float>(window->getSize().x) - 170.0f, 10.0f);

    heroDiary = std::make_unique<HeroDiary>();
    heroDiary->initialize(font);

    initialized = true;
    std::cout << "UISystem initialized successfully." << std::endl;
    return true;
}

void UISystem::shutdown() {
    if (initialized) {
        messages.clear();
        inventoryUI.reset();
        questLogUI.reset();
        skillTreeUI.reset();
        dialogSystem.reset();
        godPanelUI.reset();
        minimapSystem.reset();
        heroDiary.reset();
        window = nullptr;
        initialized = false;
        std::cout << "UISystem выключена." << std::endl;
    }
}

void UISystem::loadFont() {
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "Предупреждение: Не удалось загрузить шрифт 'assets/fonts/arial.ttf'. Попробуем системный..." << std::endl;
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "Ошибка: Не удалось загрузить ни один шрифт. UI может отображаться некорректно." << std::endl;
        }
    }
}

void UISystem::setupHUD() {
    hudText.setFont(font);
    hudText.setCharacterSize(16);
    hudText.setFillColor(sf::Color::White);
    hudText.setPosition(10, 10);

    hudBackground.setSize(sf::Vector2f(300, 100));
    hudBackground.setPosition(5, 5);
    hudBackground.setFillColor(sf::Color(0, 0, 0, 150));
    hudBackground.setOutlineThickness(1);
    hudBackground.setOutlineColor(sf::Color::White);

    faithBarBg.setSize(sf::Vector2f(180, 12));
    faithBarBg.setPosition(10, 115);
    faithBarBg.setFillColor(sf::Color(30, 30, 30));
    faithBarBg.setOutlineThickness(1);
    faithBarBg.setOutlineColor(sf::Color(80, 80, 80));

    faithBarFill.setSize(sf::Vector2f(0, 12));
    faithBarFill.setPosition(10, 115);
    faithBarFill.setFillColor(sf::Color(160, 120, 255));

    faithLabel.setFont(font);
    faithLabel.setCharacterSize(10);
    faithLabel.setFillColor(sf::Color(200, 170, 255));
    faithLabel.setPosition(195, 115);

    xpLabel.setFont(font);
    xpLabel.setCharacterSize(10);
    xpLabel.setFillColor(sf::Color(255, 200, 50));
    xpLabel.setPosition(195, 149);

    healthBarBg.setSize(sf::Vector2f(180, 12));
    healthBarBg.setPosition(10, 132);
    healthBarBg.setFillColor(sf::Color(30, 30, 30));
    healthBarBg.setOutlineThickness(1);
    healthBarBg.setOutlineColor(sf::Color(80, 80, 80));

    healthBarFill.setSize(sf::Vector2f(0, 12));
    healthBarFill.setPosition(10, 132);
    healthBarFill.setFillColor(sf::Color(80, 220, 80));

    xpBarBg.setSize(sf::Vector2f(180, 12));
    xpBarBg.setPosition(10, 149);
    xpBarBg.setFillColor(sf::Color(30, 30, 30));
    xpBarBg.setOutlineThickness(1);
    xpBarBg.setOutlineColor(sf::Color(80, 80, 80));

    xpBarFill.setSize(sf::Vector2f(0, 12));
    xpBarFill.setPosition(10, 149);
    xpBarFill.setFillColor(sf::Color(255, 200, 50));

    heroInfoText.setFont(font);
    heroInfoText.setCharacterSize(10);
    heroInfoText.setFillColor(sf::Color(180, 180, 180));
    heroInfoText.setPosition(10, 166);

    timeOfDayText.setFont(font);
    timeOfDayText.setCharacterSize(10);
    timeOfDayText.setFillColor(sf::Color(200, 200, 100));
    timeOfDayText.setPosition(10, 165);
}

void UISystem::updateHUD() {
    if (!playerEntity) {
        hudText.setString("Игрок: Не найден");
        return;
    }
    std::ostringstream oss;
    oss << "Игрок: " << playerEntity->getName() << "\n"
        << "Здоровье: " << playerEntity->getHealth() << "/" << playerEntity->getMaxHealth() << "\n"
        << "Уровень: " << playerEntity->getLevel() << " Опыт: " << playerEntity->getExperience() << "\n"
        << "Золото: " << playerEntity->getGold() << " | Инвентарь: " << playerEntity->getInventory().size();
    hudText.setString(oss.str());

    if (divineSystemPtr) {
        float maxFaith = divineSystemPtr->getMaxFaith();
        float faithRatio = (maxFaith > 0) ? divineSystemPtr->getFaith() / maxFaith : 0.0f;
        faithBarFill.setSize(sf::Vector2f(180.0f * faithRatio, 12));
        std::ostringstream fss;
        fss << "Bepa: " << static_cast<int>(divineSystemPtr->getFaith())
            << "/" << static_cast<int>(divineSystemPtr->getMaxFaith());
        faithLabel.setString(fss.str());
    }

    if (playerEntity) {
        float maxHP = static_cast<float>(playerEntity->getMaxHealth());
        float hpRatio = (maxHP > 0) ? static_cast<float>(playerEntity->getHealth()) / maxHP : 0.0f;
        healthBarFill.setSize(sf::Vector2f(180.0f * hpRatio, 12));

        int xpForNext = playerEntity->getLevel() * 50;
        float xpRatio = (xpForNext > 0) ? static_cast<float>(playerEntity->getExperience()) / xpForNext : 0.0f;
        if (xpRatio > 1.0f) xpRatio = 1.0f;
        xpBarFill.setSize(sf::Vector2f(180.0f * xpRatio, 12));
        std::ostringstream xss;
        xss << "Опыт: " << playerEntity->getExperience() << "/" << xpForNext;
        xpLabel.setString(xss.str());
    }

    if (heroAIPtr && heroAIPtr->hasActiveGoal()) {
        const auto& goal = heroAIPtr->getCurrentGoal();
        const char* goalNames[] = {
            "Исследование", "Поиск еды", "Поиск воды", "Отдых",
            "Бой", "Побег", "Поиск NPC", "Молитва",
            "Божья воля", "В опасность", "В безопасность", "В неизвестность",
            "Торговля", "Продажа лута", "Использование предмета"
        };
        int idx = static_cast<int>(goal.type);
        if (idx >= 0 && idx <= 14) {
            std::ostringstream gss;
            gss << "Цель: " << goalNames[idx] << "\n";
            gss << "Еда: " << static_cast<int>(heroAIPtr->getNeeds().getHunger())
                << " | Вода: " << static_cast<int>(heroAIPtr->getNeeds().getThirst())
                << " | Энергия: " << static_cast<int>(heroAIPtr->getNeeds().getEnergy());
            heroInfoText.setString(gss.str());
        }
    }

    if (dayNightSystemPtr) {
        const char* timeNames[] = { "Рассвет", "День", "Закат", "Ночь" };
        int timeIdx = static_cast<int>(dayNightSystemPtr->getTimeOfDay());
        if (timeIdx >= 0 && timeIdx <= 3) {
            std::ostringstream tss;
            tss << "Время: " << timeNames[timeIdx];
            WeatherType w = dayNightSystemPtr->getWeather();
            if (w != WeatherType::CLEAR) {
                const char* weatherNames[] = { "", "Дождь", "Шторм", "Снег", "Туман" };
                int wIdx = static_cast<int>(w);
                if (wIdx >= 1 && wIdx <= 4) tss << " | " << weatherNames[wIdx];
            }
            timeOfDayText.setString(tss.str());
        }
    }
}

void UISystem::showGameHUD() {
    if (!initialized) {
        std::cerr << "UISystem не инициализирована!" << std::endl;
        return;
    }
    // Отображение фона HUD
    window->draw(hudBackground);
    // Обновление и отображение текста статистики
    updateHUD();
    window->draw(hudText);
}

void UISystem::hideGameHUD() {
    hideInventory();
    hideQuestLog();
    endDialog();
}

bool UISystem::isGameHUDVisible() const {
    return initialized;
}

void UISystem::showMessage(const std::string& message) {
    if (!initialized || !window) return;
    TimedMessage msg;
    msg.text.setFont(font);
    msg.text.setCharacterSize(14);
    msg.text.setFillColor(sf::Color::Yellow);
    msg.text.setString(message);

    float baseY = window->getSize().y - 50.0f;
    float offsetY = messages.size() * 20.0f;
    msg.text.setPosition(10, baseY - offsetY);
    msg.timer.restart();
    messages.push_back(std::move(msg));

    if (messages.size() > 5) {
        messages.erase(messages.begin());
    }
}

void UISystem::clearMessages() {
    messages.clear();
}

void UISystem::showDialog(const std::string& dialogueId, Player* player, Entity* npc) {
    if (dialogSystem) {
        dialogSystem->startDialogue(dialogueId, player, npc);
    }
}

void UISystem::endDialog() {
    if (dialogSystem) {
        dialogSystem->endDialogue();
    }
}

bool UISystem::isInDialog() const {
    return dialogSystem && dialogSystem->isInDialogue();
}

DialogSystem* UISystem::getDialogSystem() const {
    return dialogSystem.get();
}

void UISystem::setPlayerEntity(Entity* player) {
    playerEntity = player;
}

void UISystem::setDivineSystem(DivineSystem* ds) { divineSystemPtr = ds; }
void UISystem::setDayNightSystem(DayNightSystem* dns) { dayNightSystemPtr = dns; }
void UISystem::setHeroAI(HeroAI* hai) { heroAIPtr = hai; }
void UISystem::setChunkSystem(ChunkSystem* cs) { chunkSystemPtr = cs; }
void UISystem::setWorldEntities(const std::vector<std::shared_ptr<Entity>>* entities) { worldEntitiesPtr = entities; }
void UISystem::setHeroEntity(Entity* hero) { heroEntityPtr = hero; }

void UISystem::showInventory() {
    if (inventoryUI) inventoryUI->setVisible(true);
}

void UISystem::hideInventory() {
    if (inventoryUI) inventoryUI->setVisible(false);
}

bool UISystem::isInventoryVisible() const {
    return inventoryUI && inventoryUI->isVisible();
}

void UISystem::showQuestLog() {
    if (questLogUI) questLogUI->setVisible(true);
}

void UISystem::hideQuestLog() {
    if (questLogUI) questLogUI->setVisible(false);
}

bool UISystem::isQuestLogVisible() const {
    return questLogUI && questLogUI->isVisible();
}

void UISystem::showSkillTree() {
    if (skillTreeUI) skillTreeUI->setVisible(true);
}

void UISystem::hideSkillTree() {
    if (skillTreeUI) skillTreeUI->setVisible(false);
}

bool UISystem::isSkillTreeVisible() const {
    return skillTreeUI && skillTreeUI->isVisible();
}

void UISystem::update(float deltaTime) {
    if (!initialized || !window) return;

    messages.erase(
        std::remove_if(messages.begin(), messages.end(),
            [](const TimedMessage& msg) { return msg.timer.getElapsedTime().asSeconds() > 3.0f; }),
        messages.end());

    updateHUD();

    if (inventoryUI && inventoryUI->isVisible()) {
        inventoryUI->update(deltaTime, playerEntity);
    }
    if (questLogUI && questLogUI->isVisible()) {
        questLogUI->update(deltaTime, playerEntity);
    }
    if (skillTreeUI && skillTreeUI->isVisible()) {
        skillTreeUI->update(deltaTime, playerEntity);
    }
    if (dialogSystem && dialogSystem->isInDialogue()) {
        dialogSystem->update(deltaTime);
    }
    if (godPanelUI && godPanelUI->isVisible()) {
        godPanelUI->update(deltaTime);
    }

    if (minimapSystem && minimapSystem->isVisible() && playerEntity) {
        minimapSystem->update(deltaTime, playerEntity, heroEntityPtr,
                              worldEntitiesPtr ? *worldEntitiesPtr : std::vector<std::shared_ptr<Entity>>(),
                              chunkSystemPtr);
    }

    if (heroDiary && heroDiary->isVisible()) {
        heroDiary->update(deltaTime);
    }
}

void UISystem::draw(sf::RenderTarget& target) {
    if (!initialized) return;
    target.draw(hudBackground);
    target.draw(hudText);
    target.draw(faithBarBg);
    target.draw(faithBarFill);
    target.draw(faithLabel);
    target.draw(healthBarBg);
    target.draw(healthBarFill);
    target.draw(xpBarBg);
    target.draw(xpBarFill);
    target.draw(xpLabel);
    target.draw(heroInfoText);
    target.draw(timeOfDayText);
    for (auto& msg : messages) {
        target.draw(msg.text);
    }
    // Рисуем инвентарь, журнал квестов и дерево навыков
    if (inventoryUI && inventoryUI->isVisible()) {
        inventoryUI->draw(target);
    }
    if (questLogUI && questLogUI->isVisible()) {
        questLogUI->draw(target);
    }
    if (skillTreeUI && skillTreeUI->isVisible()) {
        skillTreeUI->draw(target);
    }
    // Крафт и торговля
    renderCraftingMenu(target);
    renderTradeMenu(target);
    if (godPanelUI && godPanelUI->isVisible()) {
        godPanelUI->draw(target);
    }
    if (dialogSystem && dialogSystem->isInDialogue()) {
        dialogSystem->draw(target);
    }

    if (minimapSystem) {
        minimapSystem->draw(target);
        minimapSystem->drawLegend(target);
    }

    if (heroDiary) {
        heroDiary->draw(target);
    }
}

bool UISystem::isActive() const {
    return (inventoryUI && inventoryUI->isVisible()) ||
        (questLogUI && questLogUI->isVisible()) ||
        (skillTreeUI && skillTreeUI->isVisible()) ||
        (dialogSystem && dialogSystem->isInDialogue()) ||
        craftingMenuVisible || tradeMenuVisible ||
        (godPanelUI && godPanelUI->isVisible()) ||
        (heroDiary && heroDiary->isVisible());
}

void UISystem::handleEvent(const sf::Event& event) {
    if (!initialized) return;

    if (dialogSystem && dialogSystem->isInDialogue()) {
        dialogSystem->handleEvent(event);
        return;
    }

    if (heroDiary && heroDiary->isVisible()) {
        heroDiary->handleEvent(event);
        return;
    }

    if (inventoryUI && inventoryUI->isVisible()) {
        inventoryUI->handleEvent(event);
        if (!inventoryUI->isVisible()) return;
    }
    if (questLogUI && questLogUI->isVisible()) {
        questLogUI->handleEvent(event);
        if (!questLogUI->isVisible()) return;
    }
    if (skillTreeUI && skillTreeUI->isVisible()) {
        skillTreeUI->handleEvent(event);
    }
    if (godPanelUI && godPanelUI->isVisible()) {
        godPanelUI->handleEvent(event);
    }

    if (event.type == sf::Event::KeyPressed) {
        if (tradeMenuVisible && tradeSystemPtr && tradePlayer) {
            if (event.key.code == sf::Keyboard::Up) {
                if (tradeScrollOffset > 0) tradeScrollOffset--;
            } else if (event.key.code == sf::Keyboard::Down) {
                tradeScrollOffset++;
            } else if (event.key.code == sf::Keyboard::Enter) {
                const TradeOffer* offer = tradeSystemPtr->findTradeOffer(currentTradeNPC);
                if (offer) {
                    int idx = tradeScrollOffset;
                    if (idx >= 0 && idx < static_cast<int>(offer->buyItems.size())) {
                        tradeSystemPtr->buyItem(offer->buyItems[idx].itemId, tradePlayer);
                    }
                }
            } else if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num5) {
                int sellIdx = static_cast<int>(event.key.code) - static_cast<int>(sf::Keyboard::Num0);
                const TradeOffer* offer = tradeSystemPtr->findTradeOffer(currentTradeNPC);
                if (offer && sellIdx >= 0 && sellIdx < static_cast<int>(offer->sellItems.size())) {
                    tradeSystemPtr->sellItem(offer->sellItems[sellIdx].itemId, tradePlayer, 1);
                }
            }
        }
        if (craftingMenuVisible && craftingSystemPtr && craftPlayer) {
            if (event.key.code == sf::Keyboard::Up) {
                if (craftScrollOffset > 0) craftScrollOffset--;
            } else if (event.key.code == sf::Keyboard::Down) {
                craftScrollOffset++;
            } else if (event.key.code == sf::Keyboard::Enter) {
                auto recipes = craftingSystemPtr->getAvailableRecipes(craftPlayer);
                int idx = craftScrollOffset;
                if (idx >= 0 && idx < static_cast<int>(recipes.size())) {
                    craftingSystemPtr->startCrafting(recipes[idx]->id, craftPlayer);
                }
            }
        }
    }
}

void UISystem::hideAll() {
    hideGameHUD();
    hideInventory();
    hideQuestLog();
    hideSkillTree();
    hideCraftingMenu();
    hideTradeMenu();
    hideGodPanel();
    hideDiary();
}

void UISystem::showCraftingMenu(Player* player, CraftingSystem& craftingSystem) {
    craftingMenuVisible = true;
    craftPlayer = player;
    craftingSystemPtr = &craftingSystem;
    craftScrollOffset = 0;
    if (window) {
        window->setView(window->getDefaultView());
    }
}

void UISystem::hideCraftingMenu() {
    craftingMenuVisible = false;
    craftPlayer = nullptr;
    craftingSystemPtr = nullptr;
}

bool UISystem::isCraftingMenuVisible() const {
    return craftingMenuVisible;
}

void UISystem::showTradeMenu(Player* player, TradeSystem& tradeSystem, const std::string& npcId) {
    tradeMenuVisible = true;
    tradePlayer = player;
    tradeSystemPtr = &tradeSystem;
    currentTradeNPC = npcId;
    tradeScrollOffset = 0;
    tradeSystemPtr->startTrading(npcId, player);
    if (window) {
        window->setView(window->getDefaultView());
    }
}

void UISystem::hideTradeMenu() {
    if (tradeSystemPtr) tradeSystemPtr->stopTrading();
    tradeMenuVisible = false;
    tradePlayer = nullptr;
    tradeSystemPtr = nullptr;
    currentTradeNPC.clear();
}

bool UISystem::isTradeMenuVisible() const {
    return tradeMenuVisible;
}

void UISystem::renderCraftingMenu(sf::RenderTarget& target) {
    if (!craftingMenuVisible || !craftPlayer || !craftingSystemPtr) return;

    sf::RectangleShape panel(sf::Vector2f(500, 400));
    panel.setPosition(140, 50);
    panel.setFillColor(sf::Color(20, 20, 40, 230));
    panel.setOutlineThickness(2);
    panel.setOutlineColor(sf::Color(100, 150, 255));
    target.draw(panel);

    sf::Text title;
    title.setFont(font);
    title.setString("КРАФТИНГ");
    title.setCharacterSize(20);
    title.setFillColor(sf::Color(200, 220, 255));
    title.setPosition(340, 60);
    target.draw(title);

    auto recipes = craftingSystemPtr->getAvailableRecipes(craftPlayer);
    int recipeCount = static_cast<int>(recipes.size());
    int visibleSlots = 6;
    int maxScroll = std::max(0, recipeCount - visibleSlots);
    if (craftScrollOffset > maxScroll) craftScrollOffset = maxScroll;

    for (int i = 0; i < visibleSlots && (i + craftScrollOffset) < recipeCount; ++i) {
        int idx = i + craftScrollOffset;
        const auto* recipe = recipes[idx];
        if (!recipe) continue;

        float y = 90.0f + i * 50.0f;

        bool canCraft = craftingSystemPtr->canCraft(recipe->id, craftPlayer);

        sf::RectangleShape slot(sf::Vector2f(460, 44));
        slot.setPosition(160, y);
        slot.setFillColor(canCraft ? sf::Color(30, 60, 30, 200) : sf::Color(40, 30, 30, 200));
        slot.setOutlineThickness(1);
        slot.setOutlineColor(canCraft ? sf::Color(100, 200, 100) : sf::Color(120, 80, 80));
        target.draw(slot);

        sf::Text nameText;
        nameText.setFont(font);
        nameText.setString(recipe->name + "  x" + std::to_string(recipe->resultQuantity));
        nameText.setCharacterSize(14);
        nameText.setFillColor(canCraft ? sf::Color(200, 255, 200) : sf::Color(200, 150, 150));
        nameText.setPosition(170, y + 4);
        target.draw(nameText);

        std::string ingStr;
        for (const auto& ing : recipe->ingredients) {
            if (!ingStr.empty()) ingStr += ", ";
            ingStr += ing.itemName + " x" + std::to_string(ing.quantity);
        }
        sf::Text ingText;
        ingText.setFont(font);
        ingText.setString(ingStr);
        ingText.setCharacterSize(10);
        ingText.setFillColor(sf::Color(150, 150, 150));
        ingText.setPosition(170, y + 22);
        target.draw(ingText);

        if (canCraft) {
            sf::Text craftHint;
            craftHint.setFont(font);
            craftHint.setString("[Enter]");
            craftHint.setCharacterSize(11);
            craftHint.setFillColor(sf::Color(255, 255, 100));
            craftHint.setPosition(540, y + 10);
            target.draw(craftHint);
        }
    }

    if (craftingSystemPtr->isCrafting()) {
        sf::Text crafting;
        crafting.setFont(font);
        crafting.setString("Крафт в процессе...");
        crafting.setCharacterSize(14);
        crafting.setFillColor(sf::Color(255, 200, 50));
        crafting.setPosition(300, 420);
        target.draw(crafting);
    }

    sf::Text hint;
    hint.setFont(font);
    hint.setString("Enter - крафт | Стрелки - листать | C/Esc - закрыть");
    hint.setCharacterSize(12);
    hint.setFillColor(sf::Color(150, 150, 150));
    hint.setPosition(180, 430);
    target.draw(hint);
}

void UISystem::renderTradeMenu(sf::RenderTarget& target) {
    if (!tradeMenuVisible || !tradePlayer || !tradeSystemPtr) return;

    sf::RectangleShape panel(sf::Vector2f(600, 420));
    panel.setPosition(90, 40);
    panel.setFillColor(sf::Color(20, 30, 20, 235));
    panel.setOutlineThickness(2);
    panel.setOutlineColor(sf::Color(100, 255, 150));
    target.draw(panel);

    sf::Text title;
    title.setFont(font);
    title.setString("ТОРГОВЛЯ");
    title.setCharacterSize(20);
    title.setFillColor(sf::Color(200, 255, 200));
    title.setPosition(340, 50);
    target.draw(title);

    sf::Text goldText;
    goldText.setFont(font);
    goldText.setString("Золото: " + std::to_string(tradePlayer->getGold()));
    goldText.setCharacterSize(14);
    goldText.setFillColor(sf::Color(255, 215, 0));
    goldText.setPosition(490, 52);
    target.draw(goldText);

    const TradeOffer* offer = tradeSystemPtr->findTradeOffer(currentTradeNPC);
    if (!offer) {
        sf::Text noOffer;
        noOffer.setFont(font);
        noOffer.setString("Нет товаров");
        noOffer.setPosition(300, 240);
        target.draw(noOffer);
        return;
    }

    sf::Text buyTitle;
    buyTitle.setFont(font);
    buyTitle.setString("ПОКУПКА:");
    buyTitle.setCharacterSize(14);
    buyTitle.setFillColor(sf::Color(180, 255, 180));
    buyTitle.setPosition(100, 75);
    target.draw(buyTitle);

    sf::Text sellTitle;
    sellTitle.setFont(font);
    sellTitle.setString("ПРОДАЖА:");
    sellTitle.setCharacterSize(14);
    sellTitle.setFillColor(sf::Color(255, 255, 150));
    sellTitle.setPosition(400, 75);
    target.draw(sellTitle);

    int visibleSlots = 5;
    int maxScroll = std::max(0, static_cast<int>(offer->buyItems.size()) - visibleSlots);
    if (tradeScrollOffset > maxScroll) tradeScrollOffset = maxScroll;

    for (int i = 0; i < visibleSlots && (i + tradeScrollOffset) < static_cast<int>(offer->buyItems.size()); ++i) {
        int idx = i + tradeScrollOffset;
        const auto& item = offer->buyItems[idx];
        float y = 95.0f + i * 42.0f;

        sf::RectangleShape slot(sf::Vector2f(280, 36));
        slot.setPosition(100, y);
        slot.setFillColor(sf::Color(30, 50, 30, 180));
        slot.setOutlineThickness(1);
        slot.setOutlineColor(sf::Color(80, 150, 80));
        target.draw(slot);

        sf::Text nameText;
        nameText.setFont(font);
        nameText.setString(item.itemName);
        nameText.setCharacterSize(12);
        nameText.setFillColor(sf::Color(200, 255, 200));
        nameText.setPosition(108, y + 3);
        target.draw(nameText);

        int price = tradeSystemPtr->calculateBuyPrice(item, 0);
        bool canBuy = tradePlayer->getGold() >= price && item.stock != 0;

        sf::Text priceText;
        priceText.setFont(font);
        priceText.setString(std::to_string(price) + " золота" + (item.stock >= 0 ? " (" + std::to_string(item.stock) + ")" : ""));
        priceText.setCharacterSize(10);
        priceText.setFillColor(canBuy ? sf::Color(255, 215, 0) : sf::Color(150, 100, 100));
        priceText.setPosition(108, y + 19);
        target.draw(priceText);
    }

    for (int i = 0; i < visibleSlots && i < static_cast<int>(offer->sellItems.size()); ++i) {
        const auto& item = offer->sellItems[i];
        float y = 95.0f + i * 42.0f;

        int playerCount = 0;
        for (const auto& invItem : tradePlayer->getInventory()) {
            if (invItem && invItem->getId() == item.itemId) playerCount++;
        }
        bool canSell = playerCount > 0;

        sf::RectangleShape slot(sf::Vector2f(280, 36));
        slot.setPosition(400, y);
        slot.setFillColor(canSell ? sf::Color(50, 50, 30, 180) : sf::Color(40, 30, 30, 180));
        slot.setOutlineThickness(1);
        slot.setOutlineColor(canSell ? sf::Color(150, 150, 80) : sf::Color(100, 60, 60));
        target.draw(slot);

        sf::Text nameText;
        nameText.setFont(font);
        nameText.setString(item.itemName + (playerCount > 0 ? " (x" + std::to_string(playerCount) + ")" : ""));
        nameText.setCharacterSize(12);
        nameText.setFillColor(canSell ? sf::Color(255, 255, 200) : sf::Color(150, 100, 100));
        nameText.setPosition(408, y + 3);
        target.draw(nameText);

        int sellPrice = tradeSystemPtr->calculateSellPrice(item, 0);
        sf::Text priceText;
        priceText.setFont(font);
        priceText.setString(std::to_string(sellPrice) + " золота");
        priceText.setCharacterSize(10);
        priceText.setFillColor(canSell ? sf::Color(255, 215, 0) : sf::Color(150, 100, 100));
        priceText.setPosition(408, y + 19);
        target.draw(priceText);
    }

    sf::Text hint;
    hint.setFont(font);
    hint.setString("Стрелки - листать | Enter - купить (0-5: продать) | Esc - закрыть");
    hint.setCharacterSize(12);
    hint.setFillColor(sf::Color(150, 150, 150));
    hint.setPosition(120, 440);
    target.draw(hint);
}

void UISystem::showGodPanel(DivineSystem& divineSystem, Player* player) {
    if (godPanelUI) {
        godPanelUI->setDivineSystem(&divineSystem);
        godPanelUI->setPlayer(player);
        godPanelUI->show();
        godPanelVisible = true;
    }
}

void UISystem::hideGodPanel() {
    if (godPanelUI) {
        godPanelUI->hide();
    }
    godPanelVisible = false;
}

void UISystem::toggleGodPanel(DivineSystem& divineSystem, Player* player) {
    if (godPanelUI) {
        if (godPanelUI->isVisible()) {
            hideGodPanel();
        } else {
            showGodPanel(divineSystem, player);
        }
    }
}

bool UISystem::isGodPanelVisible() const {
    return godPanelUI && godPanelUI->isVisible();
}

void UISystem::toggleMinimap() {
    if (minimapSystem) minimapSystem->toggle();
}

void UISystem::showMinimap() {
    if (minimapSystem) minimapSystem->show();
}

void UISystem::hideMinimap() {
    if (minimapSystem) minimapSystem->hide();
}

bool UISystem::isMinimapVisible() const {
    return minimapSystem && minimapSystem->isVisible();
}

MinimapSystem* UISystem::getMinimapSystem() const {
    return minimapSystem.get();
}

void UISystem::toggleDiary() {
    if (heroDiary) heroDiary->toggle();
}

void UISystem::showDiary() {
    if (heroDiary) heroDiary->show();
}

void UISystem::hideDiary() {
    if (heroDiary) heroDiary->hide();
}

bool UISystem::isDiaryVisible() const {
    return heroDiary && heroDiary->isVisible();
}

HeroDiary* UISystem::getHeroDiary() const {
    return heroDiary.get();
}