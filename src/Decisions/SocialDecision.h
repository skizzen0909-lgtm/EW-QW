// Decisions/SocialDecision.h
#ifndef SOCIALDECISION_H
#define SOCIALDECISION_H

#include "Decisions/Decision.h"
#include <string>

class Entity;
class HeroAttributes;
class HeroEmotions;

// Типы социальных взаимодействий
enum class SocialAction {
    PERSUADE,       // Убеждение (Харизма + Мудрость)
    INTIMIDATE,     // Запугивание (Сила + Харизма)
    DECEIVE,        // Обман (Интеллект + Харизма)
    BARTER,         // Торговля (Харизма)
    INSPIRE,        // Вдохновение (Харизма + Мудрость)
    BEFRIEND,       // Подружиться (Харизма + Эмоции)
    THREATEN,       // Угрожать (Сила + Гнев)
    COMPLIMENT,     // Сделать комплимент (Харизма + Радость)
    APOLOGIZE       // Извиниться (Мудрость + Печаль)
};

class SocialDecision : public Decision {
private:
    SocialAction action;
    int targetNPCId;
    std::string targetName;
    
    // Модификаторы от характеристик
    float charismaMod;
    float strengthMod;
    float intelligenceMod;
    float wisdomMod;
    
    // Модификаторы от эмоций
    float joyMod;
    float angerMod;
    float sadnessMod;
    float loveMod;
    
    // Отношения с NPC
    float relationshipLevel;  // -100 до +100
    int npcDifficulty;        // Сложность убеждения
    
public:
    SocialDecision(const std::string& decisionId,
                   const std::string& description,
                   SocialAction socialAction,
                   int npcId = -1,
                   const std::string& npcName = "NPC");
    
    ~SocialDecision() override = default;
    
    // Установка модификаторов
    void setAttributeModifiers(const HeroAttributes& attributes);
    void setEmotionModifiers(const HeroEmotions& emotions);
    void setRelationship(float level);
    void setNPCDifficulty(int difficulty);
    
    // Выполнение социального действия
    bool execute(Entity* actor) override;
    
    // Получить действие
    SocialAction getAction() const;
    
    // Расчёт шанса успеха
    float calculateSuccessChance() const;
    
    // Расчёт изменения отношений
    float calculateRelationshipChange() const;
    
    // Описание действия
    std::string getActionDescription() const;
    
    // Рекомендация лучшего действия
    static SocialAction recommendAction(const HeroAttributes& attributes,
                                        const HeroEmotions& emotions,
                                        float relationshipLevel,
                                        int npcDifficulty);
};

#endif // SOCIALDECISION_H
