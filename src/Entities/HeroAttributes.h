// Entities/HeroAttributes.h
#ifndef HEROATTRIBUTES_H
#define HEROATTRIBUTES_H

#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>

// Основные характеристики героя
enum class AttributeType {
    STRENGTH,      // Сила - урон, переносимый вес, взлом силой
    AGILITY,       // Ловкость - уклонение, криты, взлом ловкостью, инициатива
    INTELLIGENCE,  // Интеллект - мана, эффективность заклинаний, анализ
    WISDOM,        // Мудрость - сопротивление магии, обнаружение скрытого, вера
    CHARISMA       // Харизма - торговля, убеждение, репутация
};

struct HeroAttribute {
    int baseValue;      // Базовое значение
    int modifier;       // Временный модификатор
    int experience;     // Опыт использования этой характеристики
    
    HeroAttribute() : baseValue(10), modifier(0), experience(0) {}
    HeroAttribute(int base) : baseValue(base), modifier(0), experience(0) {}
    
    int getTotal() const { return std::max(1, baseValue + modifier); }
    
    float getModifierBonus() const {
        // Каждая характеристика даёт бонус после 10
        return static_cast<float>(getTotal() - 10) * 0.05f;
    }
    
    void addExperience(int exp) {
        experience += exp;
        // Каждые 100 очков опыта дают +1 к базе
        if (experience >= 100) {
            baseValue += experience / 100;
            experience %= 100;
        }
    }
};

class HeroAttributes {
private:
    std::unordered_map<AttributeType, HeroAttribute> attributes;
    
public:
    HeroAttributes();
    
    // Получение характеристик
    int getStrength() const;
    int getAgility() const;
    int getIntelligence() const;
    int getWisdom() const;
    int getCharisma() const;
    
    HeroAttribute& getAttribute(AttributeType type);
    const HeroAttribute& getAttribute(AttributeType type) const;
    
    // Установка значений
    void setStrength(int value);
    void setAgility(int value);
    void setIntelligence(int value);
    void setWisdom(int value);
    void setCharisma(int value);
    
    // Модификаторы
    void addStrengthMod(int mod);
    void addAgilityMod(int mod);
    void addIntelligenceMod(int mod);
    void addWisdomMod(int mod);
    void addCharismaMod(int mod);
    
    void clearModifiers();
    
    // Опыт и прокачка
    void addAttributeExperience(AttributeType type, int exp);
    
    // Расчёты на основе характеристик
    float calculateMeleeDamage() const;           // Сила
    float calculateRangedDamage() const;          // Ловкость
    float calculateMagicPower() const;            // Интеллект + Мудрость
    float calculateDefense() const;               // Сила + Ловкость
    float calculateMagicResist() const;           // Мудрость + Интеллект
    float calculateLockpickChance() const;        // Ловкость + Интеллект
    float calculateForceChance() const;           // Сила
    float calculateTradeDiscount() const;         // Харизма
    float calculatePersuasionChance() const;      // Харизма + Мудрость
    float calculateInitiative() const;            // Ловкость + Интеллект
    float calculateCarryCapacity() const;         // Сила
    
    // Проверки навыков
    bool checkStrength(int difficulty) const;
    bool checkAgility(int difficulty) const;
    bool checkIntelligence(int difficulty) const;
    bool checkWisdom(int difficulty) const;
    bool checkCharisma(int difficulty) const;
    
    // Описание
    std::string getAttributeName(AttributeType type) const;
    std::string getAttributeDescription(AttributeType type) const;
    
    // Серийлизация
    std::string serialize() const;
    void deserialize(const std::string& data);
};

#endif // HEROATTRIBUTES_H
