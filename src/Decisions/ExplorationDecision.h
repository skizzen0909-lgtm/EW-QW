// Decisions/ExplorationDecision.h
#ifndef EXPLORATIONDECISION_H
#define EXPLORATIONDECISION_H

#include "Decisions/Decision.h"
#include <string>

class Entity;
class HeroAttributes;
class HeroEmotions;

// Типы исследовательских действий
enum class ExplorationAction {
    SEARCH_AREA,        // Исследовать местность (Мудрость + Интеллект)
    CLIMB_HEIGHTS,      // Взобраться на высоту (Сила + Ловкость)
    SWIM_ACROSS,        // Переплыть преграду (Сила + Ловкость)
    SQUEEZE_THROUGH,    // Протиснуться через узкий проход (Ловкость)
    BREAK_OBSTACLE,     // Сломать препятствие (Сила)
    NAVIGATE_WILDERNESS,// Ориентирование в дикой местности (Мудрость + Интеллект)
    DETECT_TRAPS,       // Обнаружить ловушки (Мудрость + Интеллект)
    DISGUISE,           // Замаскироваться (Интеллект + Ловкость)
    FOLLOW_TRACKS,      // Идти по следу (Мудрость + Интеллект)
    SCOUT_AHEAD         // Разведка впереди (Ловкость + Мудрость)
};

class ExplorationDecision : public Decision {
private:
    ExplorationAction action;
    int difficulty;
    std::string locationName;
    
    // Модификаторы от характеристик
    float strengthMod;
    float agilityMod;
    float intelligenceMod;
    float wisdomMod;
    
    // Модификаторы от эмоций
    float braveryMod;
    float fearMod;
    float joyMod;
    
    // Флаг успеха
    bool success;
    
public:
    ExplorationDecision(const std::string& decisionId,
                        const std::string& description,
                        ExplorationAction explorationAction,
                        int diff = 10,
                        const std::string& location = "Неизвестная местность");
    
    ~ExplorationDecision() override = default;
    
    // Установка модификаторов
    void setAttributeModifiers(const HeroAttributes& attributes);
    void setEmotionModifiers(const HeroEmotions& emotions);
    void setDifficulty(int diff);
    
    // Выполнение исследования
    bool execute(Entity* actor) override;
    
    // Получить действие
    ExplorationAction getAction() const;
    
    // Расчёт шанса успеха
    float calculateSuccessChance() const;
    
    // Расчёт возможных находок
    std::string calculateFindings() const;
    
    // Описание действия
    std::string getActionDescription() const;
    
    // Рекомендация лучшего действия
    static ExplorationAction recommendAction(const HeroAttributes& attributes,
                                             const HeroEmotions& emotions,
                                             int difficulty);
                                             
    // Проверка успеха
    bool wasSuccessful() const;
};

#endif // EXPLORATIONDECISION_H
