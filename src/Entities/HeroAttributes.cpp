// Entities/HeroAttributes.cpp
#include "HeroAttributes.h"
#include <sstream>
#include <cmath>

HeroAttributes::HeroAttributes() {
    // Инициализация всех характеристик базовыми значениями
    attributes[AttributeType::STRENGTH] = HeroAttribute(10);
    attributes[AttributeType::AGILITY] = HeroAttribute(10);
    attributes[AttributeType::INTELLIGENCE] = HeroAttribute(10);
    attributes[AttributeType::WISDOM] = HeroAttribute(10);
    attributes[AttributeType::CHARISMA] = HeroAttribute(10);
}

int HeroAttributes::getStrength() const {
    auto it = attributes.find(AttributeType::STRENGTH);
    return (it != attributes.end()) ? it->second.getTotal() : 10;
}

int HeroAttributes::getAgility() const {
    auto it = attributes.find(AttributeType::AGILITY);
    return (it != attributes.end()) ? it->second.getTotal() : 10;
}

int HeroAttributes::getIntelligence() const {
    auto it = attributes.find(AttributeType::INTELLIGENCE);
    return (it != attributes.end()) ? it->second.getTotal() : 10;
}

int HeroAttributes::getWisdom() const {
    auto it = attributes.find(AttributeType::WISDOM);
    return (it != attributes.end()) ? it->second.getTotal() : 10;
}

int HeroAttributes::getCharisma() const {
    auto it = attributes.find(AttributeType::CHARISMA);
    return (it != attributes.end()) ? it->second.getTotal() : 10;
}

HeroAttribute& HeroAttributes::getAttribute(AttributeType type) {
    return attributes[type];
}

const HeroAttribute& HeroAttributes::getAttribute(AttributeType type) const {
    auto it = attributes.find(type);
    return (it != attributes.end()) ? it->second : attributes.at(AttributeType::STRENGTH);
}

void HeroAttributes::setStrength(int value) {
    attributes[AttributeType::STRENGTH].baseValue = std::max(1, value);
}

void HeroAttributes::setAgility(int value) {
    attributes[AttributeType::AGILITY].baseValue = std::max(1, value);
}

void HeroAttributes::setIntelligence(int value) {
    attributes[AttributeType::INTELLIGENCE].baseValue = std::max(1, value);
}

void HeroAttributes::setWisdom(int value) {
    attributes[AttributeType::WISDOM].baseValue = std::max(1, value);
}

void HeroAttributes::setCharisma(int value) {
    attributes[AttributeType::CHARISMA].baseValue = std::max(1, value);
}

void HeroAttributes::addStrengthMod(int mod) {
    attributes[AttributeType::STRENGTH].modifier += mod;
}

void HeroAttributes::addAgilityMod(int mod) {
    attributes[AttributeType::AGILITY].modifier += mod;
}

void HeroAttributes::addIntelligenceMod(int mod) {
    attributes[AttributeType::INTELLIGENCE].modifier += mod;
}

void HeroAttributes::addWisdomMod(int mod) {
    attributes[AttributeType::WISDOM].modifier += mod;
}

void HeroAttributes::addCharismaMod(int mod) {
    attributes[AttributeType::CHARISMA].modifier += mod;
}

void HeroAttributes::clearModifiers() {
    for (auto& pair : attributes) {
        pair.second.modifier = 0;
    }
}

void HeroAttributes::addAttributeExperience(AttributeType type, int exp) {
    auto it = attributes.find(type);
    if (it != attributes.end()) {
        it->second.addExperience(exp);
    }
}

float HeroAttributes::calculateMeleeDamage() const {
    // Сила даёт прямой бонус к урону
    float base = 1.0f;
    float strengthBonus = (getStrength() - 10) * 0.1f;
    return base + strengthBonus;
}

float HeroAttributes::calculateRangedDamage() const {
    // Ловкость влияет на точность и криты
    float base = 1.0f;
    float agilityBonus = (getAgility() - 10) * 0.08f;
    return base + agilityBonus;
}

float HeroAttributes::calculateMagicPower() const {
    // Интеллект + Мудрость для магической силы
    float base = 1.0f;
    float intBonus = (getIntelligence() - 10) * 0.1f;
    float wisBonus = (getWisdom() - 10) * 0.05f;
    return base + intBonus + wisBonus;
}

float HeroAttributes::calculateDefense() const {
    // Сила + Ловкость для защиты
    float base = 0.0f;
    float strBonus = (getStrength() - 10) * 0.05f;
    float agiBonus = (getAgility() - 10) * 0.05f;
    return base + strBonus + agiBonus;
}

float HeroAttributes::calculateMagicResist() const {
    // Мудрость + Интеллект для магического сопротивления
    float base = 0.0f;
    float wisBonus = (getWisdom() - 10) * 0.1f;
    float intBonus = (getIntelligence() - 10) * 0.05f;
    return base + wisBonus + intBonus;
}

float HeroAttributes::calculateLockpickChance() const {
    // Ловкость + Интеллект для взлома
    float base = 0.3f; // Базовый шанс 30%
    float agiBonus = (getAgility() - 10) * 0.02f;
    float intBonus = (getIntelligence() - 10) * 0.015f;
    return std::min(0.95f, base + agiBonus + intBonus);
}

float HeroAttributes::calculateForceChance() const {
    // Сила для силового взлома/выбивания
    float base = 0.2f; // Базовый шанс 20%
    float strBonus = (getStrength() - 10) * 0.025f;
    return std::min(0.95f, base + strBonus);
}

float HeroAttributes::calculateTradeDiscount() const {
    // Харизма для скидок в торговле
    float base = 0.0f;
    float chaBonus = (getCharisma() - 10) * 0.02f;
    return std::min(0.5f, base + chaBonus); // Максимум 50% скидки
}

float HeroAttributes::calculatePersuasionChance() const {
    // Харизма + Мудрость для убеждения
    float base = 0.4f; // Базовый шанс 40%
    float chaBonus = (getCharisma() - 10) * 0.025f;
    float wisBonus = (getWisdom() - 10) * 0.015f;
    return std::min(0.95f, base + chaBonus + wisBonus);
}

float HeroAttributes::calculateInitiative() const {
    // Ловкость + Интеллект для инициативы в бою
    float base = 1.0f;
    float agiBonus = (getAgility() - 10) * 0.1f;
    float intBonus = (getIntelligence() - 10) * 0.05f;
    return base + agiBonus + intBonus;
}

float HeroAttributes::calculateCarryCapacity() const {
    // Сила определяет максимальный вес инвентаря
    float base = 50.0f; // Базовая грузоподъёмность 50 единиц
    float strBonus = (getStrength() - 10) * 5.0f;
    return base + strBonus;
}

bool HeroAttributes::checkStrength(int difficulty) const {
    // Проверка на силу: d20 + модификатор >= сложность
    int modifier = (getStrength() - 10) / 2;
    int roll = std::rand() % 20 + 1;
    return (roll + modifier) >= difficulty;
}

bool HeroAttributes::checkAgility(int difficulty) const {
    int modifier = (getAgility() - 10) / 2;
    int roll = std::rand() % 20 + 1;
    return (roll + modifier) >= difficulty;
}

bool HeroAttributes::checkIntelligence(int difficulty) const {
    int modifier = (getIntelligence() - 10) / 2;
    int roll = std::rand() % 20 + 1;
    return (roll + modifier) >= difficulty;
}

bool HeroAttributes::checkWisdom(int difficulty) const {
    int modifier = (getWisdom() - 10) / 2;
    int roll = std::rand() % 20 + 1;
    return (roll + modifier) >= difficulty;
}

bool HeroAttributes::checkCharisma(int difficulty) const {
    int modifier = (getCharisma() - 10) / 2;
    int roll = std::rand() % 20 + 1;
    return (roll + modifier) >= difficulty;
}

std::string HeroAttributes::getAttributeName(AttributeType type) const {
    switch (type) {
        case AttributeType::STRENGTH: return "Сила";
        case AttributeType::AGILITY: return "Ловкость";
        case AttributeType::INTELLIGENCE: return "Интеллект";
        case AttributeType::WISDOM: return "Мудрость";
        case AttributeType::CHARISMA: return "Харизма";
        default: return "Неизвестно";
    }
}

std::string HeroAttributes::getAttributeDescription(AttributeType type) const {
    switch (type) {
        case AttributeType::STRENGTH:
            return "Влияет на урон в ближнем бою, грузоподъёмность и силовые проверки";
        case AttributeType::AGILITY:
            return "Влияет на уклонение, критические удары, взлом и инициативу";
        case AttributeType::INTELLIGENCE:
            return "Влияет на магическую силу, анализ и эффективность заклинаний";
        case AttributeType::WISDOM:
            return "Влияет на магическое сопротивление, обнаружение скрытого и веру";
        case AttributeType::CHARISMA:
            return "Влияет на торговлю, убеждение NPC и репутацию";
        default:
            return "Нет описания";
    }
}

std::string HeroAttributes::serialize() const {
    std::ostringstream oss;
    oss << getStrength() << "," 
        << getAgility() << "," 
        << getIntelligence() << "," 
        << getWisdom() << "," 
        << getCharisma();
    return oss.str();
}

void HeroAttributes::deserialize(const std::string& data) {
    std::istringstream iss(data);
    int str, agi, intel, wis, cha;
    char delim;
    
    if (iss >> str >> delim >> agi >> delim >> intel >> delim >> wis >> delim >> cha) {
        setStrength(str);
        setAgility(agi);
        setIntelligence(intel);
        setWisdom(wis);
        setCharisma(cha);
    }
}
