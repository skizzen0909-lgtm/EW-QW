// Entities/Wolf.h
#ifndef WOLF_H
#define WOLF_H

#include "Entities/Enemy.h"

class Wolf : public Enemy {
private:
    bool packLeader;
    float howlTimer;
    float howlCooldown;

public:
    Wolf(int level = 1);
    ~Wolf() override = default;

    void update(float deltaTime) override;

    bool isWolfPackLeader() const;
    void setPackLeader(bool leader);

    void howl();
    bool canHowl() const;

    static std::unique_ptr<Wolf> createAlphaWolf(int level);
};

#endif // WOLF_H
