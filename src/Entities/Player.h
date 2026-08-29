// Entities/Player.h
#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <string>
#include <random>

class Player : public Entity {
private:
    int experienceToNextLevel;
    std::mt19937 gen;
    std::uniform_int_distribution<> moveDist;

public:
    Player(const std::string& name, int health = 100);
    ~Player() override = default;

    // Тип сущности
    std::string getType() const override;

    // Урон и исцеление
    void takeDamage(int damage) override;
    void heal(int amount) override;

    // Автоматические действия
    void performAutoAction(float deltaTime) override;

    // Уровень и опыт
    int getExperienceToNextLevel() const;
    void setExperienceToNextLevel(int exp);

    void gainExperience(int exp) override;
    void levelUp() override;

    // Запрет копирования (унаследовано от Entity)
};

#endif // PLAYER_H