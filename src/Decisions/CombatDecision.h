// Decisions/CombatDecision.h
#ifndef COMBATDECISION_H
#define COMBATDECISION_H

#include "Decisions/Decision.h"
#include <string>
#include <vector>

class Entity;
class HeroAttributes;
class HeroEmotions;

// Стили боя
enum class CombatStyle {
    AGGRESSIVE,     // Сила - больше урона, меньше защиты
    DEFENSIVE,      // Мудрость + Ловкость - больше защиты
    PRECISE,        // Ловкость - критические удары
    TACTICAL,       // Интеллект - анализ слабостей
    DESPERATE       // Низкое HP + Страх/Гнев - ва-банк
};

class CombatDecision : public Decision {
private:
    CombatStyle style;
    int enemyDifficulty;
    
    // Модификаторы от характеристик
    float strengthMod;
    float agilityMod;
    float intelligenceMod;
    float wisdomMod;
    float charismaMod;
    
    // Модификаторы от эмоций
    float fearMod;
    float angerMod;
    float braveryMod;
    
    std::string targetEnemy;
    
public:
    CombatDecision(const std::string& decisionId,
                   const std::string& description,
                   CombatStyle combatStyle,
                   int difficulty = 10,
                   const std::string& enemy = "Враг");
    
    ~CombatDecision() override = default;
    
    // Установка модификаторов от характеристик
    void setAttributeModifiers(const HeroAttributes& attributes);
    
    // Установка модификаторов от эмоций
    void setEmotionModifiers(const HeroEmotions& emotions);
    
    // Выполнение решения
    bool execute(Entity* actor) override;
    
    // Получить стиль боя
    CombatStyle getStyle() const;
    
    // Расчёт шанса успеха
    float calculateSuccessChance() const;
    
    // Расчёт урона
    float calculateDamage() const;
    
    // Расчёт получаемого урона
    float calculateReceivedDamage() const;
    
    // Описание стиля
    std::string getStyleDescription() const;
    
    // Рекомендация лучшего стиля на основе характеристик
    static CombatStyle recommendStyle(const HeroAttributes& attributes, 
                                      const HeroEmotions& emotions,
                                      int enemyDifficulty);
};

#endif // COMBATDECISION_H
