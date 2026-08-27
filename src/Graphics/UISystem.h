// src/UI/UISystem.h
#ifndef UISYSTEM_H
#define UISYSTEM_H

#include "Core/Engine.h"  
#include <memory>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Forward declarations
class Entity;
class Player;
class QuestLogUI;
class InventoryUI;
class SkillTreeUI;
class DialogSystem;
class AdvancedGraphicsSystem;
class CraftingSystem;
class TradeSystem;
class DivineSystem;
class Player;
class GodPanelUI;
class DayNightSystem;
class HeroAI;
class MinimapSystem;
class HeroDiary;
class ChunkSystem;

class UISystem {
private:
    bool initialized;
    sf::RenderWindow* window;
    Entity* playerEntity;

    // --- External systems (raw pointers, not owned) ---
    DivineSystem* divineSystemPtr;
    DayNightSystem* dayNightSystemPtr;
    HeroAI* heroAIPtr;

    // --- Основные элементы UI ---
    sf::Font font;
    sf::Text hudText;
    sf::RectangleShape hudBackground;

    // --- God HUD (top-left overlay) ---
    sf::RectangleShape faithBarBg;
    sf::RectangleShape faithBarFill;
    sf::Text faithLabel;
    sf::Text xpLabel;
    sf::RectangleShape healthBarBg;
    sf::RectangleShape healthBarFill;
    sf::RectangleShape xpBarBg;
    sf::RectangleShape xpBarFill;
    sf::Text heroInfoText;
    sf::Text timeOfDayText;

    // --- UI Панели ---
    std::unique_ptr<InventoryUI> inventoryUI;
    std::unique_ptr<QuestLogUI> questLogUI;
    std::unique_ptr<SkillTreeUI> skillTreeUI;
    std::unique_ptr<DialogSystem> dialogSystem;

    // --- Крафт и Торговля UI ---
    bool craftingMenuVisible;
    bool tradeMenuVisible;
    std::string currentTradeNPC;
    Player* tradePlayer;
    Player* craftPlayer;
    TradeSystem* tradeSystemPtr;
    CraftingSystem* craftingSystemPtr;
    int tradeScrollOffset;
    int craftScrollOffset;

    // --- Панель бога ---
    std::unique_ptr<GodPanelUI> godPanelUI;
    bool godPanelVisible;

    // --- Мини-карта ---
    std::unique_ptr<MinimapSystem> minimapSystem;
    bool minimapVisible;

    // --- Дневник героя ---
    std::unique_ptr<HeroDiary> heroDiary;
    bool diaryVisible;

    // --- Внешние указатели для миникарты ---
    ChunkSystem* chunkSystemPtr;
    const std::vector<std::shared_ptr<Entity>>* worldEntitiesPtr;
    Entity* heroEntityPtr;

    // --- Сообщения ---
    struct TimedMessage {
        sf::Text text;
        sf::Clock timer;
    };
    std::vector<TimedMessage> messages;

    // --- Диалоговые элементы ---
    sf::RectangleShape dialogBox;
    sf::Text speakerText;
    sf::Text dialogText;
    sf::Texture speakerPortrait;
    sf::Sprite portraitSprite;
    std::vector<sf::RectangleShape> optionBoxes;
    std::vector<sf::Text> optionTexts;

    // --- Внутренние методы ---
    void loadFont();
    void setupHUD();
    void updateHUD();

    // Крафт и Торговля рендер
    void renderCraftingMenu(sf::RenderTarget& target);
    void renderTradeMenu(sf::RenderTarget& target);

public:
    UISystem();
    ~UISystem();

    // Инициализация и очистка — ТОЛЬКО ОБЪЯВЛЕНИЕ!
    bool initialize(sf::RenderWindow& targetWindow);
    void shutdown();

    // Основные методы UI
    void showGameHUD();
    void hideGameHUD();
    bool isGameHUDVisible() const;

    // Сообщения
    void showMessage(const std::string& message);
    void clearMessages();

    void showDialog(const std::string& dialogueId, Player* player, Entity* npc);
    void endDialog();
    bool isInDialog() const;
    DialogSystem* getDialogSystem() const;

    void setPlayerEntity(Entity* player);
    void setDivineSystem(DivineSystem* ds);
    void setDayNightSystem(DayNightSystem* dns);
    void setHeroAI(HeroAI* hai);
    void setChunkSystem(ChunkSystem* cs);
    void setWorldEntities(const std::vector<std::shared_ptr<Entity>>* entities);
    void setHeroEntity(Entity* hero);

    // Панели
    void showInventory();
    void hideInventory();
    bool isInventoryVisible() const;

    void showQuestLog();
    void hideQuestLog();
    bool isQuestLogVisible() const;

    void showSkillTree();
    void hideSkillTree();
    bool isSkillTreeVisible() const;

    // Крафт и Торговля
    void showCraftingMenu(Player* player, CraftingSystem& craftingSystem);
    void hideCraftingMenu();
    bool isCraftingMenuVisible() const;

    void showTradeMenu(Player* player, TradeSystem& tradeSystem, const std::string& npcId);
    void hideTradeMenu();
    bool isTradeMenuVisible() const;

    // Панель бога
    void showGodPanel(DivineSystem& divineSystem, Player* player);
    void hideGodPanel();
    void toggleGodPanel(DivineSystem& divineSystem, Player* player);
    bool isGodPanelVisible() const;

    // Мини-карта
    void toggleMinimap();
    void showMinimap();
    void hideMinimap();
    bool isMinimapVisible() const;
    MinimapSystem* getMinimapSystem() const;

    // Дневник героя
    void toggleDiary();
    void showDiary();
    void hideDiary();
    bool isDiaryVisible() const;
    HeroDiary* getHeroDiary() const;

    // Методы цикла
    void update(float deltaTime);
    void draw(sf::RenderTarget& target);
    bool isActive() const;
    void handleEvent(const sf::Event& event);
    void hideAll(); // Теперь публичный метод
};

#endif // UISYSTEM_H