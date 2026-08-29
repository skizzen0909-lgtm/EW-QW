// Decisions/ExplorationDecision.cpp
#include "ExplorationDecision.h"
#include "Entities/Entity.h"
#include "Entities/HeroAttributes.h"
#include "Entities/HeroEmotions.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

ExplorationDecision::ExplorationDecision(const std::string& decisionId,
                                         const std::string& description,
                                         ExplorationAction explorationAction,
                                         int diff,
                                         const std::string& location)
    : Decision(decisionId, description)
    , action(explorationAction)
    , difficulty(diff)
    , locationName(location)
    , strengthMod(1.0f)
    , agilityMod(1.0f)
    , intelligenceMod(1.0f)
    , wisdomMod(1.0f)
    , braveryMod(1.0f)
    , fearMod(1.0f)
    , joyMod(1.0f)
    , success(false) {
}

void ExplorationDecision::setAttributeModifiers(const HeroAttributes& attributes) {
    strengthMod = 1.0f + (attributes.getStrength() - 10) * 0.05f;
    agilityMod = 1.0f + (attributes.getAgility() - 10) * 0.05f;
    intelligenceMod = 1.0f + (attributes.getIntelligence() - 10) * 0.05f;
    wisdomMod = 1.0f + (attributes.getWisdom() - 10) * 0.05f;
}

void ExplorationDecision::setEmotionModifiers(const HeroEmotions& emotions) {
    // Храбрость помогает в исследовании
    if (emotions.isBrave()) {
        braveryMod = 1.2f;
    }
    
    // Страх мешает
    if (emotions.isAfraid()) {
        fearMod = 0.7f;
    }
    
    // Радость немного помогает
    if (emotions.isHappy()) {
        joyMod = 1.1f;
    }
}

void ExplorationDecision::setDifficulty(int diff) {
    difficulty = std::max(1, std::min(20, diff));
}

bool ExplorationDecision::execute(Entity* actor) {
    if (!actor) return false;
    
    float successChance = calculateSuccessChance();
    int roll = std::rand() % 100;
    success = (roll < successChance * 100);
    
    if (success) {
        std::string findings = calculateFindings();
        std::cout << actor->getName() << " выполняет \"" 
                  << getActionDescription() << "\" в локации \"" 
                  << locationName << "\"" << std::endl;
        std::cout << "Успех! " << findings << std::endl;
        
        // Опыт для используемых характеристик
        switch (action) {
            case ExplorationAction::SEARCH_AREA:
            case ExplorationAction::DETECT_TRAPS:
                // Мудрость и Интеллект получают опыт
                break;
            case ExplorationAction::CLIMB_HEIGHTS:
            case ExplorationAction::SWIM_ACROSS:
                // Сила и Ловкость получают опыт
                break;
            case ExplorationAction::SQUEEZE_THROUGH:
            case ExplorationAction::DISGUISE:
                // Ловкость получает опыт
                break;
            case ExplorationAction::BREAK_OBSTACLE:
                // Сила получает опыт
                break;
            case ExplorationAction::NAVIGATE_WILDERNESS:
            case ExplorationAction::FOLLOW_TRACKS:
                // Мудрость и Интеллект получают опыт
                break;
            case ExplorationAction::SCOUT_AHEAD:
                // Ловкость и Мудрость получают опыт
                break;
        }
    } else {
        std::cout << actor->getName() << " терпит неудачу при попытке \"" 
                  << getActionDescription() << "\"" << std::endl;
        
        // Возможные негативные последствия
        if (std::rand() % 100 < 30) {
            int damage = std::rand() % 5 + 1;
            std::cout << "Вы получили " << damage << " урона!" << std::endl;
            if (actor) actor->takeDamage(damage);
        }
    }
    
    return success;
}

ExplorationAction ExplorationDecision::getAction() const {
    return action;
}

float ExplorationDecision::calculateSuccessChance() const {
    float baseChance = 0.5f;
    
    switch (action) {
        case ExplorationAction::SEARCH_AREA:
            // Мудрость + Интеллект
            baseChance += ((wisdomMod + intelligenceMod) / 2.0f - 1.0f) * 0.6f;
            break;
            
        case ExplorationAction::CLIMB_HEIGHTS:
            // Сила + Ловкость
            baseChance += ((strengthMod + agilityMod) / 2.0f - 1.0f) * 0.6f;
            baseChance *= braveryMod;
            break;
            
        case ExplorationAction::SWIM_ACROSS:
            // Сила + Ловкость
            baseChance += ((strengthMod + agilityMod) / 2.0f - 1.0f) * 0.5f;
            break;
            
        case ExplorationAction::SQUEEZE_THROUGH:
            // Только Ловкость
            baseChance += (agilityMod - 1.0f) * 0.7f;
            break;
            
        case ExplorationAction::BREAK_OBSTACLE:
            // Только Сила
            baseChance += (strengthMod - 1.0f) * 0.7f;
            baseChance *= braveryMod;
            break;
            
        case ExplorationAction::NAVIGATE_WILDERNESS:
            // Мудрость + Интеллект
            baseChance += ((wisdomMod + intelligenceMod) / 2.0f - 1.0f) * 0.6f;
            break;
            
        case ExplorationAction::DETECT_TRAPS:
            // Мудрость + Интеллект
            baseChance += ((wisdomMod + intelligenceMod) / 2.0f - 1.0f) * 0.7f;
            break;
            
        case ExplorationAction::DISGUISE:
            // Интеллект + Ловкость
            baseChance += ((intelligenceMod + agilityMod) / 2.0f - 1.0f) * 0.5f;
            break;
            
        case ExplorationAction::FOLLOW_TRACKS:
            // Мудрость + Интеллект
            baseChance += ((wisdomMod + intelligenceMod) / 2.0f - 1.0f) * 0.6f;
            break;
            
        case ExplorationAction::SCOUT_AHEAD:
            // Ловкость + Мудрость
            baseChance += ((agilityMod + wisdomMod) / 2.0f - 1.0f) * 0.6f;
            baseChance *= braveryMod;
            break;
    }
    
    // Модификатор сложности
    float difficultyMod = difficulty / 20.0f;
    baseChance -= difficultyMod * 0.3f;
    
    // Эмоциональные модификаторы
    baseChance *= braveryMod * joyMod;
    baseChance /= fearMod;
    
    return std::clamp(baseChance, 0.1f, 0.95f);
}

std::string ExplorationDecision::calculateFindings() const {
    if (!success) return "Ничего не найдено";
    
    switch (action) {
        case ExplorationAction::SEARCH_AREA:
            return "Найдены ресурсы: травы, ягоды или полезные предметы.";
        case ExplorationAction::CLIMB_HEIGHTS:
            return "С высоты виден новый путь или интересные объекты.";
        case ExplorationAction::SWIM_ACROSS:
            return "Преграда преодолена! На другом берегу что-то есть.";
        case ExplorationAction::SQUEEZE_THROUGH:
            return "Узкий проход ведёт в новое место.";
        case ExplorationAction::BREAK_OBSTACLE:
            return "Препятствие разрушено! Путь открыт.";
        case ExplorationAction::NAVIGATE_WILDERNESS:
            return "Вы нашли безопасный маршрут через местность.";
        case ExplorationAction::DETECT_TRAPS:
            return "Обнаружены и обезврежены ловушки!";
        case ExplorationAction::DISGUISE:
            return "Маскировка успешна - вы остались незамеченным.";
        case ExplorationAction::FOLLOW_TRACKS:
            return "След привёл к интересной находке или существу.";
        case ExplorationAction::SCOUT_AHEAD:
            return "Разведка показала безопасный путь и потенциальные угрозы.";
        default:
            return "Что-то найдено.";
    }
}

std::string ExplorationDecision::getActionDescription() const {
    switch (action) {
        case ExplorationAction::SEARCH_AREA:
            return "Поиск в местности (Мудрость+Интеллект)";
        case ExplorationAction::CLIMB_HEIGHTS:
            return "Взобраться на высоту (Сила+Ловкость)";
        case ExplorationAction::SWIM_ACROSS:
            return "Переплыть преграду (Сила+Ловкость)";
        case ExplorationAction::SQUEEZE_THROUGH:
            return "Протиснуться (Ловкость)";
        case ExplorationAction::BREAK_OBSTACLE:
            return "Сломать препятствие (Сила)";
        case ExplorationAction::NAVIGATE_WILDERNESS:
            return "Ориентирование (Мудрость+Интеллект)";
        case ExplorationAction::DETECT_TRAPS:
            return "Обнаружение ловушек (Мудрость+Интеллект)";
        case ExplorationAction::DISGUISE:
            return "Маскировка (Интеллект+Ловкость)";
        case ExplorationAction::FOLLOW_TRACKS:
            return "Идти по следу (Мудрость+Интеллект)";
        case ExplorationAction::SCOUT_AHEAD:
            return "Разведка (Ловкость+Мудрость)";
        default:
            return "Неизвестное действие";
    }
}

ExplorationAction ExplorationDecision::recommendAction(const HeroAttributes& attributes,
                                                       const HeroEmotions& emotions,
                                                       int difficulty) {
    int str = attributes.getStrength();
    int agi = attributes.getAgility();
    int intel = attributes.getIntelligence();
    int wis = attributes.getWisdom();
    
    bool isBrave = emotions.isBrave();
    bool isAfraid = emotions.isAfraid();
    
    // Если высокая сила и храбрость
    if (str >= 13 && isBrave) {
        if (difficulty <= 10) {
            return ExplorationAction::BREAK_OBSTACLE;
        }
        return ExplorationAction::CLIMB_HEIGHTS;
    }
    
    // Если высокая ловкость
    if (agi >= 13) {
        if (difficulty <= 8) {
            return ExplorationAction::SQUEEZE_THROUGH;
        }
        return ExplorationAction::SCOUT_AHEAD;
    }
    
    // Если высокие ментальные характеристики
    if (wis >= 13 || intel >= 13) {
        if (isAfraid) {
            return ExplorationAction::DETECT_TRAPS;
        }
        return ExplorationAction::SEARCH_AREA;
    }
    
    // По умолчанию - поиск
    return ExplorationAction::SEARCH_AREA;
}

bool ExplorationDecision::wasSuccessful() const {
    return success;
}
