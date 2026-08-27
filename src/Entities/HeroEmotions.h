// Entities/HeroEmotions.h
#ifndef HEROEMOTIONS_H
#define HEROEMOTIONS_H

enum class EmotionType {
    FEAR,
    ANGER,
    JOY,
    LOVE,
    BRAVERY,
    SADNESS,
    DISGUST
};

enum class EmotionIntensity {
    NONE,       // 0-10%
    MILD,       // 10-30%
    MODERATE,   // 30-60%
    STRONG,     // 60-80%
    EXTREME     // 80-100%
};

class HeroEmotions {
private:
    float fear;
    float anger;
    float joy;
    float love;
    float bravery;
    float sadness;
    float disgust;

    float decayRate;

    EmotionIntensity getIntensity(float value) const;

public:
    HeroEmotions();

    void update(float deltaTime);

    float getFear() const;
    float getAnger() const;
    float getJoy() const;
    float getLove() const;
    float getBravery() const;
    float getSadness() const;
    float getDisgust() const;

    void setFear(float value);
    void setAnger(float value);
    void setJoy(float value);
    void setLove(float value);
    void setBravery(float value);
    void setSadness(float value);
    void setDisgust(float value);

    void addFear(float amount);
    void addAnger(float amount);
    void addJoy(float amount);
    void addLove(float amount);
    void addBravery(float amount);
    void addSadness(float amount);
    void addDisgust(float amount);

    EmotionIntensity getFearIntensity() const;
    EmotionIntensity getAngerIntensity() const;
    EmotionIntensity getJoyIntensity() const;
    EmotionIntensity getLoveIntensity() const;
    EmotionIntensity getBraveryIntensity() const;
    EmotionIntensity getSadnessIntensity() const;
    EmotionIntensity getDisgustIntensity() const;

    float getDominantEmotion() const;
    EmotionType getDominantEmotionType() const;

    float getMovementSpeedModifier() const;
    float getAttackDamageModifier() const;
    float getDefenseModifier() const;
    float getLuckModifier() const;

    bool isAfraid() const;
    bool isEnraged() const;
    bool isHappy() const;
    bool isBrave() const;
    bool isSad() const;

    void triggerFear(float intensity);
    void triggerAnger(float intensity);
    void triggerJoy(float intensity);
    void triggerLove(float intensity);
    void triggerBravery(float intensity);

    void onDamageTaken(float damage);
    void onEnemyKilled();
    void onFriendNearby();
    void onLevelUp();
    void onQuestCompleted();

    void setDecayRate(float rate);
};

#endif // HEROEMOTIONS_H
