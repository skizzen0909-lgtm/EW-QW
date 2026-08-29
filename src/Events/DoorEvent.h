// Events/DoorEvent.h
#ifndef DOOREVENT_H
#define DOOREVENT_H

#include "Events/Event.h"
#include <string>
#include <memory>
#include <vector>

class Entity;

// Типы дверей/препятствий
enum class DoorType {
    LOCKED_WOODEN,      // Деревянная запертая дверь
    LOCKED_IRON,        // Железная запертая дверь
    LOCKED_MAGIC,       // Магическая печать
    BLOCKED_DEBRIS,     // Завал из обломков
    SECRET_PASSAGE,     // Скрытый проход
    GATE                // Ворота
};

// Решение для двери
enum class DoorSolutionType {
    USE_KEY,            // Использовать ключ
    LOCKPICK,           // Взломать отмычкой
    FORCE_OPEN,         // Выбить силой
    USE_EXPLOSIVE,      // Использовать взрывчатку
    MAGIC_UNLOCK,       // Магическое открытие
    FIND_ALTERNATIVE,   // Найти обходной путь
    BYPASS_STEALTH      // Пройти скрытно (окно, лазейка)
};

class DoorEvent : public Event {
private:
    DoorType doorType;
    int difficulty;              // Сложность открытия (1-20)
    std::string doorName;
    std::string description;
    
    bool isOpen;
    bool isBroken;
    
    // Доступные решения
    bool hasKey;
    bool hasLockpicks;
    bool hasExplosives;
    bool magicAvailable;  // Переименовано для избежания конфликта
    bool alternativeRouteAvailable;
    
    // Результаты
    DoorSolutionType chosenSolution;
    bool success;
    
public:
    DoorEvent(const std::string& eventId, 
              DoorType type,
              int diff = 10,
              const std::string& name = "Запертая дверь");
    
    ~DoorEvent() override = default;
    
    void execute() override;
    std::string getDescription() const override;
    
    // Геттеры
    DoorType getDoorType() const;
    int getDifficulty() const;
    bool isOpened() const;
    bool isBrokenDoor() const;
    
    // Установка условий
    void setHasKey(bool value);
    void setHasLockpicks(bool value);
    void setHasExplosives(bool value);
    void setCanUseMagic(bool value);
    void setAlternativeRouteAvailable(bool value);
    
    // Проверка решений
    bool canUseKey() const;
    bool canLockpick() const;
    bool canForceOpen() const;
    bool canUseExplosive() const;
    bool canUseMagic() const;
    bool canFindAlternative() const;
    
    // Выполнение конкретного решения
    bool tryUseKey(Entity* actor);
    bool tryLockpick(Entity* actor);
    bool tryForceOpen(Entity* actor);
    bool tryUseExplosive(Entity* actor);
    bool tryMagicUnlock(Entity* actor);
    bool findAlternativeRoute(Entity* actor);
    
    // Описание решений
    std::string getSolutionDescription(DoorSolutionType solution) const;
    
    // Получить список доступных решений
    std::vector<DoorSolutionType> getAvailableSolutions() const;
};

#endif // DOOREVENT_H
