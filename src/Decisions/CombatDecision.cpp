// Decisions/CombatDecision.cpp
#include "CombatDecision.h"
#include "Entities/Entity.h"
#include "Entities/HeroAttributes.h"
#include "Entities/HeroEmotions.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

CombatDecision::CombatDecision(const std::string& decisionId,
                               const std::string& description,
                               CombatStyle combatStyle,
                               int difficulty,
                               const std::string& enemy)
    : Decision(decisionId, description)
    , style(combatStyle)
    , enemyDifficulty(difficulty)
    , strengthMod(1.0f)
    , agilityMod(1.0f)
    , intelligenceMod(1.0f)
    , wisdomMod(1.0f)
    , charismaMod(1.0f)
    , fearMod(1.0f)
    , angerMod(1.0f)
    , braveryMod(1.0f)
    , targetEnemy(enemy) {
}

void CombatDecision::setAttributeModifiers(const HeroAttributes& attributes) {
    strengthMod = 1.0f + (attributes.getStrength() - 10) * 0.05f;
    agilityMod = 1.0f + (attributes.getAgility() - 10) * 0.05f;
    intelligenceMod = 1.0f + (attributes.getIntelligence() - 10) * 0.05f;
    wisdomMod = 1.0f + (attributes.getWisdom() - 10) * 0.05f;
    charismaMod = 1.0f + (attributes.getCharisma() - 10) * 0.03f;
}

void CombatDecision::setEmotionModifiers(const HeroEmotions& emotions) {
    // Страх снижает эффективность
    if (emotions.isAfraid()) {
        fearMod = 0.7f;
    } else if (emotions.getFearIntensity() == EmotionIntensity::MODERATE) {
        fearMod = 0.85f;
    }
    
    // Гнев увеличивает урон но снижает защиту
    if (emotions.isEnraged()) {
        angerMod = 1.3f;
    } else if (emotions.getAngerIntensity() == EmotionIntensity::MODERATE) {
        angerMod = 1.15f;
    }
    
    // Храбрость улучшает все показатели
    if (emotions.isBrave()) {
        braveryMod = 1.2f;
    } else if (emotions.getBraveryIntensity() == EmotionIntensity::MODERATE) {
        braveryMod = 1.1f;
    }
}

bool CombatDecision::execute(Entity* actor) {
    if (!actor) return false;
    
    float successChance = calculateSuccessChance();
    int roll = std::rand() % 100;
    bool success = (roll < successChance * 100);
    
    if (success) {
        float damage = calculateDamage();
        std::cout << actor->getName() << " применяет стиль \"" 
                  << getStyleDescription() << "\" против " << targetEnemy << std::endl;
        std::cout << "Успех! Нанесено " << damage << " урона." << std::endl;
        
        // Опыт для используемой характеристики
        switch (style) {
            case CombatStyle::AGGRESSIVE:
                // Сила получает опыт
                break;
            case CombatStyle::DEFENSIVE:
                // Мудрость и Ловкость получают опыт
                break;
            case CombatStyle::PRECISE:
                // Ловкость получает опыт
                break;
            case CombatStyle::TACTICAL:
                // Интеллект получает опыт
                break;
            case CombatStyle::DESPERATE:
                // Все характеристики немного растут
                break;
        }
    } else {
        float receivedDamage = calculateReceivedDamage();
        std::cout << actor->getName() << " терпит неудачу с стилем \"" 
                  << getStyleDescription() << "\"" << std::endl;
        std::cout << "Получено " << receivedDamage << " урона в ответ!" << std::endl;
    }
    
    return success;
}

CombatStyle CombatDecision::getStyle() const {
    return style;
}

float CombatDecision::calculateSuccessChance() const {
    float baseChance = 0.5f;
    
    switch (style) {
        case CombatStyle::AGGRESSIVE:
            // Зависит от силы
            baseChance += (strengthMod - 1.0f) * 0.5f;
            baseChance *= angerMod * braveryMod;
            break;
            
        case CombatStyle::DEFENSIVE:
            // Зависит от мудрости и ловкости
            baseChance += ((wisdomMod + agilityMod) / 2.0f - 1.0f) * 0.5f;
            baseChance *= braveryMod;
            break;
            
        case CombatStyle::PRECISE:
            // Зависит от ловкости
            baseChance += (agilityMod - 1.0f) * 0.6f;
            baseChance *= braveryMod;
            break;
            
        case CombatStyle::TACTICAL:
            // Зависит от интеллекта
            baseChance += (intelligenceMod - 1.0f) * 0.7f;
            baseChance *= braveryMod;
            break;
            
        case CombatStyle::DESPERATE:
            // Базовый шанс но с высокими ставками
            baseChance = 0.4f;
            baseChance *= angerMod;
            break;
    }
    
    // Модификатор сложности
    float difficultyMod = enemyDifficulty / 20.0f;
    baseChance -= difficultyMod * 0.2f;
    
    return std::clamp(baseChance, 0.1f, 0.95f);
}

float CombatDecision::calculateDamage() const {
    float baseDamage = 10.0f;
    
    switch (style) {
        case CombatStyle::AGGRESSIVE:
            // Высокий урон от силы
            baseDamage *= strengthMod * 1.5f * angerMod;
            break;
            
        case CombatStyle::DEFENSIVE:
            // Умеренный урон
            baseDamage *= (wisdomMod + agilityMod) / 2.0f;
            break;
            
        case CombatStyle::PRECISE:
            // Критический урон от ловкости
            baseDamage *= agilityMod * 1.8f;
            break;
            
        case CombatStyle::TACTICAL:
            // Точный урон от интеллекта
            baseDamage *= intelligenceMod * 1.6f;
            break;
            
        case CombatStyle::DESPERATE:
            // Очень высокий или очень низкий урон
            baseDamage *= (angerMod * 2.0f);
            break;
    }
    
    return baseDamage;
}

float CombatDecision::calculateReceivedDamage() const {
    float baseDamage = 8.0f;
    
    switch (style) {
        case CombatStyle::AGGRESSIVE:
            // Получаем больше урона из-за агрессии
            baseDamage *= 1.5f / braveryMod;
            break;
            
        case CombatStyle::DEFENSIVE:
            // Получаем меньше урона
            baseDamage *= 0.6f / (wisdomMod * braveryMod);
            break;
            
        case CombatStyle::PRECISE:
            // Средний получаемый урон
            baseDamage *= 0.9f;
            break;
            
        case CombatStyle::TACTICAL:
            // Немного сниженный урон
            baseDamage *= 0.85f;
            break;
            
        case CombatStyle::DESPERATE:
            // Получаем много урона
            baseDamage *= 1.8f;
            break;
    }
    
    return baseDamage;
}

std::string CombatDecision::getStyleDescription() const {
    switch (style) {
        case CombatStyle::AGGRESSIVE:
            return "Агрессивная атака (Сила)";
        case CombatStyle::DEFENSIVE:
            return "Оборонительная тактика (Мудрость+Ловкость)";
        case CombatStyle::PRECISE:
            return "Точные удары (Ловкость)";
        case CombatStyle::TACTICAL:
            return "Тактический анализ (Интеллект)";
        case CombatStyle::DESPERATE:
            return "Отчаянная атака (Ва-банк)";
        default:
            return "Неизвестный стиль";
    }
}

CombatStyle CombatDecision::recommendStyle(const HeroAttributes& attributes,
                                           const HeroEmotions& emotions,
                                           int enemyDifficulty) {
    // Анализ характеристик
    int str = attributes.getStrength();
    int agi = attributes.getAgility();
    int intel = attributes.getIntelligence();
    int wis = attributes.getWisdom();
    
    // Находим доминирующую характеристику
    int maxStat = std::max({str, agi, intel, wis});
    
    // Учитываем эмоции
    bool isAfraid = emotions.isAfraid();
    bool isEnraged = emotions.isEnraged();
    bool isBrave = emotions.isBrave();
    
    // Рекомендации
    if (isEnraged && str >= 12) {
        return CombatStyle::AGGRESSIVE;
    }
    
    if (isAfraid && wis >= 11) {
        return CombatStyle::DEFENSIVE;
    }
    
    if (agi >= maxStat && agi >= 12) {
        return CombatStyle::PRECISE;
    }
    
    if (intel >= maxStat && intel >= 12) {
        return CombatStyle::TACTICAL;
    }
    
    if (str >= maxStat && str >= 11) {
        return CombatStyle::AGGRESSIVE;
    }
    
    if (wis >= 11 || isBrave) {
        return CombatStyle::DEFENSIVE;
    }
    
    // По умолчанию - точные удары
    return CombatStyle::PRECISE;
}
