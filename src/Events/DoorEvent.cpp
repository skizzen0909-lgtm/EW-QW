// Events/DoorEvent.cpp
#include "DoorEvent.h"
#include "Entities/Entity.h"
#include "Entities/HeroAttributes.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

DoorEvent::DoorEvent(const std::string& eventId, 
                     DoorType type,
                     int diff,
                     const std::string& name)
    : Event(eventId)
    , doorType(type)
    , difficulty(diff)
    , doorName(name)
    , isOpen(false)
    , isBroken(false)
    , hasKey(false)
    , hasLockpicks(true)  // По умолчанию считаем что отмычки есть
    , hasExplosives(false)
    , canUseMagic(false)
    , alternativeRouteAvailable(false)
    , chosenSolution(DoorSolutionType::USE_KEY)
    , success(false)
{
    // Генерация описания на основе типа двери
    std::ostringstream desc;
    switch (doorType) {
        case DoorType::LOCKED_WOODEN:
            description = "Старая деревянная дверь с ржавым замком. ";
            difficulty = std::min(difficulty, 12);
            break;
        case DoorType::LOCKED_IRON:
            description = "Массивная железная дверь с надёжным замком. ";
            difficulty = std::max(10, difficulty);
            break;
        case DoorType::LOCKED_MAGIC:
            description = "Дверь окутана магическим сиянием. Чувствуется мощная энергия. ";
            difficulty = std::max(15, difficulty);
            break;
        case DoorType::BLOCKED_DEBRIS:
            description = "Проход завален камнями и обломками. ";
            difficulty = std::max(8, difficulty);
            break;
        case DoorType::SECRET_PASSAGE:
            description = "Едва заметный скрытый проход в стене. ";
            difficulty = std::max(14, difficulty);
            break;
        case DoorType::GATE:
            description = "Большие ворота с тяжёлым засовом. ";
            difficulty = std::max(12, difficulty);
            break;
    }
    
    description += doorName + ". Сложность: " + std::to_string(difficulty);
}

void DoorEvent::execute() {
    // Этот метод вызывается после выбора решения
    // Реальное выполнение происходит в try* методах
    
    if (success) {
        std::cout << "Успех! " << doorName << " открыт." << std::endl;
        isOpen = true;
    } else {
        std::cout << "Неудача! Не удалось открыть " << doorName << "." << std::endl;
    }
}

std::string DoorEvent::getDescription() const {
    return description;
}

DoorType DoorEvent::getDoorType() const {
    return doorType;
}

int DoorEvent::getDifficulty() const {
    return difficulty;
}

bool DoorEvent::isOpened() const {
    return isOpen;
}

bool DoorEvent::isBrokenDoor() const {
    return isBroken;
}

void DoorEvent::setHasKey(bool value) {
    hasKey = value;
}

void DoorEvent::setHasLockpicks(bool value) {
    hasLockpicks = value;
}

void DoorEvent::setHasExplosives(bool value) {
    hasExplosives = value;
}

void DoorEvent::setCanUseMagic(bool value) {
    magicAvailable = value;
}

void DoorEvent::setAlternativeRouteAvailable(bool value) {
    alternativeRouteAvailable = value;
}

bool DoorEvent::canUseKey() const {
    return hasKey && !isOpen;
}

bool DoorEvent::canLockpick() const {
    return hasLockpicks && !isOpen && 
           (doorType == DoorType::LOCKED_WOODEN || doorType == DoorType::LOCKED_IRON);
}

bool DoorEvent::canForceOpen() const {
    return !isOpen && 
           (doorType == DoorType::LOCKED_WOODEN || 
            doorType == DoorType::BLOCKED_DEBRIS ||
            doorType == DoorType::GATE);
}

bool DoorEvent::canUseExplosive() const {
    return hasExplosives && !isOpen &&
           (doorType == DoorType::LOCKED_IRON || 
            doorType == DoorType::BLOCKED_DEBRIS ||
            doorType == DoorType::GATE);
}

bool DoorEvent::canUseMagic() const {
    return magicAvailable && !isOpen;
}

bool DoorEvent::canFindAlternative() const {
    return alternativeRouteAvailable && !isOpen;
}

bool DoorEvent::tryUseKey(Entity* actor) {
    if (!canUseKey()) return false;
    
    chosenSolution = DoorSolutionType::USE_KEY;
    success = true;
    isOpen = true;
    
    std::cout << actor->getName() << " использует ключ и открывает " << doorName << "." << std::endl;
    
    // Опыт для использования правильного предмета
    // Можно добавить проверку наличия ключа в инвентаре
    
    return true;
}

bool DoorEvent::tryLockpick(Entity* actor) {
    if (!canLockpick()) return false;
    
    chosenSolution = DoorSolutionType::LOCKPICK;
    
    // Проверка характеристик: Ловкость + Интеллект
    float lockpickChance = 0.0f;
    if (actor) {
        // Предполагаем что у Entity есть доступ к HeroAttributes
        // В реальной реализации нужно получить атрибуты
        lockpickChance = 0.5f; // Базовый шанс
        // Здесь должна быть логика получения атрибутов героя
    } else {
        lockpickChance = 0.3f;
    }
    
    // Модификатор сложности
    float difficultyMod = difficulty / 20.0f;
    float finalChance = lockpickChance - difficultyMod * 0.3f;
    
    int roll = std::rand() % 100;
    success = (roll < finalChance * 100);
    
    if (success) {
        isOpen = true;
        std::cout << actor->getName() << " успешно взламывает замок " << doorName << "." << std::endl;
    } else {
        std::cout << actor->getName() << " не смог взломать замок " << doorName << "." << std::endl;
        // Шанс сломать отмычку или заклинить замок
        if (roll < 10) {
            std::cout << "Отмычка сломалась!" << std::endl;
            hasLockpicks = false;
        }
    }
    
    return success;
}

bool DoorEvent::tryForceOpen(Entity* actor) {
    if (!canForceOpen()) return false;
    
    chosenSolution = DoorSolutionType::FORCE_OPEN;
    
    // Проверка силы
    float forceChance = 0.0f;
    if (actor) {
        forceChance = 0.4f; // Базовый шанс
    } else {
        forceChance = 0.2f;
    }
    
    float difficultyMod = difficulty / 20.0f;
    float finalChance = forceChance - difficultyMod * 0.2f;
    
    int roll = std::rand() % 100;
    success = (roll < finalChance * 100);
    
    if (success) {
        isOpen = true;
        std::cout << actor->getName() << " выбивает " << doorName << " с грохотом!" << std::endl;
        
        // Шанс получить урон или шум
        if (doorType == DoorType::LOCKED_IRON) {
            std::cout << "Вы получили немного урона от удара!" << std::endl;
            if (actor) actor->takeDamage(5);
        }
    } else {
        std::cout << actor->getName() << " не смог выбить " << doorName << "." << std::endl;
        // Шанс получить урон при неудаче
        if (roll < 30) {
            std::cout << "Вы ушиблись при попытке!" << std::endl;
            if (actor) actor->takeDamage(3);
        }
    }
    
    return success;
}

bool DoorEvent::tryUseExplosive(Entity* actor) {
    if (!canUseExplosive()) return false;
    
    chosenSolution = DoorSolutionType::USE_EXPLOSIVE;
    
    // Взрывчатка почти всегда работает, но с последствиями
    success = true;
    isOpen = true;
    isBroken = true;
    
    std::cout << actor->getName() << " использует взрывчатку!" << std::endl;
    std::cout << "БА-БАХ! " << doorName << " разнесён в щепки!" << std::endl;
    
    // Урон от взрыва
    if (actor) {
        int damage = 10 + std::rand() % 10;
        std::cout << "Вы получили " << damage << " урона от взрыва!" << std::endl;
        actor->takeDamage(damage);
    }
    
    // Шум привлекает врагов
    std::cout << "Взрыв привлёк внимание nearby врагов!" << std::endl;
    
    return true;
}

bool DoorEvent::tryMagicUnlock(Entity* actor) {
    if (!canUseMagic()) return false;
    
    chosenSolution = DoorSolutionType::MAGIC_UNLOCK;
    
    // Проверка магической силы (Интеллект + Мудрость)
    float magicChance = 0.6f; // Базовый шанс для мага
    
    float difficultyMod = difficulty / 20.0f;
    float finalChance = magicChance - difficultyMod * 0.2f;
    
    int roll = std::rand() % 100;
    success = (roll < finalChance * 100);
    
    if (success) {
        isOpen = true;
        std::cout << actor->getName() << " произносит заклинание открытия. " 
                  << doorName << " бесшумно открывается." << std::endl;
    } else {
        std::cout << actor->getName() << " пытается открыть дверь магией, но заклинание не сработало." << std::endl;
        // Потеря маны или откат
    }
    
    return success;
}

bool DoorEvent::findAlternativeRoute(Entity* actor) {
    if (!canFindAlternative()) return false;
    
    chosenSolution = DoorSolutionType::FIND_ALTERNATIVE;
    
    // Проверка мудрости/интеллекта для поиска пути
    float findChance = 0.5f;
    
    int roll = std::rand() % 100;
    success = (roll < findChance * 100);
    
    if (success) {
        std::cout << actor->getName() << " находит обходной путь вокруг " << doorName << "." << std::endl;
        // Герой перемещается на другую сторону без открытия двери
    } else {
        std::cout << actor->getName() << " не смог найти обходной путь." << std::endl;
    }
    
    return success;
}

std::string DoorEvent::getSolutionDescription(DoorSolutionType solution) const {
    switch (solution) {
        case DoorSolutionType::USE_KEY:
            return "Использовать ключ (гарантированный успех)";
        case DoorSolutionType::LOCKPICK:
            return "Взломать отмычкой (зависит от ловкости)";
        case DoorSolutionType::FORCE_OPEN:
            return "Выбить силой (зависит от силы, возможен урон)";
        case DoorSolutionType::USE_EXPLOSIVE:
            return "Использовать взрывчатку (гарантированно, но шумно и опасно)";
        case DoorSolutionType::MAGIC_UNLOCK:
            return "Магическое открытие (зависит от магической силы)";
        case DoorSolutionType::FIND_ALTERNATIVE:
            return "Найти обходной путь (зависит от внимательности)";
        case DoorSolutionType::BYPASS_STEALTH:
            return "Пройти скрытно (зависит от скрытности)";
        default:
            return "Неизвестное решение";
    }
}

std::vector<DoorSolutionType> DoorEvent::getAvailableSolutions() const {
    std::vector<DoorSolutionType> solutions;
    
    if (canUseKey()) solutions.push_back(DoorSolutionType::USE_KEY);
    if (canLockpick()) solutions.push_back(DoorSolutionType::LOCKPICK);
    if (canForceOpen()) solutions.push_back(DoorSolutionType::FORCE_OPEN);
    if (canUseExplosive()) solutions.push_back(DoorSolutionType::USE_EXPLOSIVE);
    if (canUseMagic()) solutions.push_back(DoorSolutionType::MAGIC_UNLOCK);
    if (canFindAlternative()) solutions.push_back(DoorSolutionType::FIND_ALTERNATIVE);
    
    return solutions;
}
