// Entities/Skeleton.h
#ifndef SKELETON_H
#define SKELETON_H

#include "Entities/Enemy.h"

class Skeleton : public Enemy {
private:
    bool nightSpawned;
    bool shield;
    bool reanimateAbility;

public:
    Skeleton(int level = 1);
    ~Skeleton() override = default;

    void update(float deltaTime) override;

    bool isSkeletonNightSpawned() const;
    void setNightSpawned(bool night);

    bool hasShield() const;
    void setShield(bool shieldEnabled);

    bool canReanimate() const;
    void reanimate();

    static std::unique_ptr<Skeleton> createBoneWarrior(int level);
    static std::unique_ptr<Skeleton> createLichMinion(int level);
};

#endif // SKELETON_H
