// Decisions/SocialDecision.cpp
#include "SocialDecision.h"
#include "Entities/Entity.h"
#include "Entities/HeroAttributes.h"
#include "Entities/HeroEmotions.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

SocialDecision::SocialDecision(const std::string& decisionId,
                               const std::string& description,
                               SocialAction socialAction,
                               int npcId,
                               const std::string& npcName)
    : Decision(decisionId, description)
    , action(socialAction)
    , targetNPCId(npcId)
    , targetName(npcName)
    , charismaMod(1.0f)
    , strengthMod(1.0f)
    , intelligenceMod(1.0f)
    , wisdomMod(1.0f)
    , joyMod(1.0f)
    , angerMod(1.0f)
    , sadnessMod(1.0f)
    , loveMod(1.0f)
    , relationshipLevel(0.0f)
    , npcDifficulty(10) {
}

void SocialDecision::setAttributeModifiers(const HeroAttributes& attributes) {
    charismaMod = 1.0f + (attributes.getCharisma() - 10) * 0.05f;
    strengthMod = 1.0f + (attributes.getStrength() - 10) * 0.05f;
    intelligenceMod = 1.0f + (attributes.getIntelligence() - 10) * 0.05f;
    wisdomMod = 1.0f + (attributes.getWisdom() - 10) * 0.05f;
}

void SocialDecision::setEmotionModifiers(const HeroEmotions& emotions) {
    // Радость улучшает социальные взаимодействия
    if (emotions.isHappy()) {
        joyMod = 1.25f;
    } else if (emotions.getJoyIntensity() == EmotionIntensity::MODERATE) {
        joyMod = 1.1f;
    }
    
    // Гнев помогает в запугивании но мешает в дружбе
    if (emotions.isEnraged()) {
        angerMod = 1.4f;
    } else if (emotions.getAngerIntensity() == EmotionIntensity::MODERATE) {
        angerMod = 1.15f;
    }
    
    // Печаль может помочь в извинениях
    if (emotions.isSad()) {
        sadnessMod = 1.3f;
    }
    
    // Любовь улучшает все социальные действия
    if (emotions.getLoveIntensity() == EmotionIntensity::MODERATE || 
        emotions.getLoveIntensity() == EmotionIntensity::STRONG) {
        loveMod = 1.2f;
    }
}

void SocialDecision::setRelationship(float level) {
    relationshipLevel = std::clamp(level, -100.0f, 100.0f);
}

void SocialDecision::setNPCDifficulty(int difficulty) {
    npcDifficulty = std::max(1, std::min(20, difficulty));
}

bool SocialDecision::execute(Entity* actor) {
    if (!actor) return false;
    
    float successChance = calculateSuccessChance();
    int roll = std::rand() % 100;
    bool success = (roll < successChance * 100);
    
    if (success) {
        float relChange = calculateRelationshipChange();
        std::cout << actor->getName() << " использует \"" 
                  << getActionDescription() << "\" на " << targetName << std::endl;
        std::cout << "Успех! Отношения изменены на " << relChange << std::endl;
        
        // Опыт для используемых характеристик
        switch (action) {
            case SocialAction::PERSUADE:
            case SocialAction::INSPIRE:
                // Харизма и Мудрость получают опыт
                break;
            case SocialAction::INTIMIDATE:
            case SocialAction::THREATEN:
                // Сила и Харизма получают опыт
                break;
            case SocialAction::DECEIVE:
                // Интеллект и Харизма получают опыт
                break;
            case SocialAction::BARTER:
                // Харизма получает опыт
                break;
            case SocialAction::BEFRIEND:
            case SocialAction::COMPLIMENT:
                // Харизма и Эмоции получают опыт
                break;
            case SocialAction::APOLOGIZE:
                // Мудрость получает опыт
                break;
        }
    } else {
        float relChange = calculateRelationshipChange() * 0.5f; // Половинный негативный эффект
        std::cout << actor->getName() << " терпит неудачу с \"" 
                  << getActionDescription() << "\"" << std::endl;
        std::cout << targetName << " реагирует негативно! Отношения: " << relChange << std::endl;
    }
    
    return success;
}

SocialAction SocialDecision::getAction() const {
    return action;
}

float SocialDecision::calculateSuccessChance() const {
    float baseChance = 0.5f;
    
    switch (action) {
        case SocialAction::PERSUADE:
            // Харизма + Мудрость
            baseChance += ((charismaMod + wisdomMod) / 2.0f - 1.0f) * 0.6f;
            baseChance *= joyMod * loveMod;
            // Бонус за хорошие отношения
            if (relationshipLevel > 0) {
                baseChance += relationshipLevel / 200.0f;
            }
            break;
            
        case SocialAction::INTIMIDATE:
            // Сила + Харизма
            baseChance += ((strengthMod + charismaMod) / 2.0f - 1.0f) * 0.6f;
            baseChance *= angerMod;
            break;
            
        case SocialAction::DECEIVE:
            // Интеллект + Харизма
            baseChance += ((intelligenceMod + charismaMod) / 2.0f - 1.0f) * 0.6f;
            // Штраф за хорошие отношения (труднее обмануть друга)
            if (relationshipLevel > 50) {
                baseChance -= 0.2f;
            }
            break;
            
        case SocialAction::BARTER:
            // Только Харизма
            baseChance += (charismaMod - 1.0f) * 0.7f;
            baseChance *= joyMod;
            break;
            
        case SocialAction::INSPIRE:
            // Харизма + Мудрость + Радость
            baseChance += ((charismaMod + wisdomMod) / 2.0f - 1.0f) * 0.5f;
            baseChance *= joyMod * loveMod;
            break;
            
        case SocialAction::BEFRIEND:
            // Харизма + Любовь
            baseChance += (charismaMod - 1.0f) * 0.5f;
            baseChance *= loveMod * joyMod;
            break;
            
        case SocialAction::THREATEN:
            // Сила + Гнев
            baseChance += ((strengthMod + angerMod) / 2.0f - 1.0f) * 0.7f;
            break;
            
        case SocialAction::COMPLIMENT:
            // Харизма + Радость
            baseChance += (charismaMod - 1.0f) * 0.5f;
            baseChance *= joyMod * loveMod;
            break;
            
        case SocialAction::APOLOGIZE:
            // Мудрость + Печаль
            baseChance += (wisdomMod - 1.0f) * 0.5f;
            baseChance *= sadnessMod;
            // Бонус если отношения плохие
            if (relationshipLevel < 0) {
                baseChance += std::abs(relationshipLevel) / 300.0f;
            }
            break;
    }
    
    // Модификатор сложности NPC
    float difficultyMod = npcDifficulty / 20.0f;
    baseChance -= difficultyMod * 0.3f;
    
    return std::clamp(baseChance, 0.1f, 0.95f);
}

float SocialDecision::calculateRelationshipChange() const {
    float baseChange = 10.0f;
    
    switch (action) {
        case SocialAction::PERSUADE:
            baseChange *= charismaMod * wisdomMod;
            break;
            
        case SocialAction::INTIMIDATE:
            baseChange *= -0.5f; // Обычно ухудшает отношения
            baseChange *= strengthMod;
            break;
            
        case SocialAction::DECEIVE:
            baseChange *= -0.3f; // Небольшое ухудшение при успехе
            break;
            
        case SocialAction::BARTER:
            baseChange *= charismaMod * 0.3f; // Торговля мало влияет на отношения
            break;
            
        case SocialAction::INSPIRE:
            baseChange *= 1.5f;
            baseChange *= charismaMod * joyMod;
            break;
            
        case SocialAction::BEFRIEND:
            baseChange *= 2.0f;
            baseChange *= charismaMod * loveMod;
            break;
            
        case SocialAction::THREATEN:
            baseChange *= -1.5f;
            baseChange *= strengthMod * angerMod;
            break;
            
        case SocialAction::COMPLIMENT:
            baseChange *= 1.2f;
            baseChange *= charismaMod * joyMod;
            break;
            
        case SocialAction::APOLOGIZE:
            baseChange *= 1.8f;
            baseChange *= wisdomMod * sadnessMod;
            // Лучше работает при плохих отношениях
            if (relationshipLevel < 0) {
                baseChange *= 1.5f;
            }
            break;
    }
    
    return baseChange;
}

std::string SocialDecision::getActionDescription() const {
    switch (action) {
        case SocialAction::PERSUADE:
            return "Убеждение (Харизма+Мудрость)";
        case SocialAction::INTIMIDATE:
            return "Запугивание (Сила+Харизма)";
        case SocialAction::DECEIVE:
            return "Обман (Интеллект+Харизма)";
        case SocialAction::BARTER:
            return "Торговля (Харизма)";
        case SocialAction::INSPIRE:
            return "Вдохновение (Харизма+Мудрость)";
        case SocialAction::BEFRIEND:
            return "Дружелюбие (Харизма+Эмоции)";
        case SocialAction::THREATEN:
            return "Угрозы (Сила+Гнев)";
        case SocialAction::COMPLIMENT:
            return "Комплимент (Харизма+Радость)";
        case SocialAction::APOLOGIZE:
            return "Извинение (Мудрость+Печаль)";
        default:
            return "Неизвестное действие";
    }
}

SocialAction SocialDecision::recommendAction(const HeroAttributes& attributes,
                                             const HeroEmotions& emotions,
                                             float relationshipLevel,
                                             int npcDifficulty) {
    int cha = attributes.getCharisma();
    int str = attributes.getStrength();
    int intel = attributes.getIntelligence();
    int wis = attributes.getWisdom();
    
    bool isHappy = emotions.isHappy();
    bool isEnraged = emotions.isEnraged();
    bool isSad = emotions.isSad();
    bool hasLove = emotions.getLoveIntensity() >= EmotionIntensity::MILD;
    
    // Если есть гнев и сила - запугивание
    if (isEnraged && str >= 12) {
        return SocialAction::THREATEN;
    }
    
    // Если плохие отношения и есть печаль - извиниться
    if (relationshipLevel < -30 && isSad && wis >= 11) {
        return SocialAction::APOLOGIZE;
    }
    
    // Если хорошие отношения - подружиться или сделать комплимент
    if (relationshipLevel > 50 && hasLove) {
        return SocialAction::BEFRIEND;
    }
    
    // Если высокая харизма
    if (cha >= 14) {
        if (isHappy) {
            return SocialAction::INSPIRE;
        }
        return SocialAction::PERSUADE;
    }
    
    // Если высокий интеллект
    if (intel >= 13 && cha >= 11) {
        return SocialAction::DECEIVE;
    }
    
    // Если высокая сила
    if (str >= 13 && cha >= 10) {
        return SocialAction::INTIMIDATE;
    }
    
    // По умолчанию - убеждение
    return SocialAction::PERSUADE;
}
