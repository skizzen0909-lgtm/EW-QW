// Entities/HeroEmotions.cpp
#include "HeroEmotions.h"
#include <algorithm>
#include <cmath>

HeroEmotions::HeroEmotions()
    : fear(0.0f), anger(0.0f), joy(20.0f), love(0.0f), bravery(20.0f)
    , sadness(0.0f), disgust(0.0f)
    , decayRate(2.0f) {
}

void HeroEmotions::update(float deltaTime) {
    float decay = decayRate * deltaTime;
    fear = std::max(0.0f, fear - decay);
    anger = std::max(0.0f, anger - decay * 0.5f);
    joy = std::max(0.0f, joy - decay * 0.3f);
    love = std::max(0.0f, love - decay * 0.8f);
    bravery = std::max(0.0f, bravery - decay * 0.4f);
    sadness = std::max(0.0f, sadness - decay * 0.6f);
    disgust = std::max(0.0f, disgust - decay * 0.7f);
}

float HeroEmotions::getFear() const { return fear; }
float HeroEmotions::getAnger() const { return anger; }
float HeroEmotions::getJoy() const { return joy; }
float HeroEmotions::getLove() const { return love; }
float HeroEmotions::getBravery() const { return bravery; }
float HeroEmotions::getSadness() const { return sadness; }
float HeroEmotions::getDisgust() const { return disgust; }

void HeroEmotions::setFear(float value) { fear = std::clamp(value, 0.0f, 100.0f); }
void HeroEmotions::setAnger(float value) { anger = std::clamp(value, 0.0f, 100.0f); }
void HeroEmotions::setJoy(float value) { joy = std::clamp(value, 0.0f, 100.0f); }
void HeroEmotions::setLove(float value) { love = std::clamp(value, 0.0f, 100.0f); }
void HeroEmotions::setBravery(float value) { bravery = std::clamp(value, 0.0f, 100.0f); }
void HeroEmotions::setSadness(float value) { sadness = std::clamp(value, 0.0f, 100.0f); }
void HeroEmotions::setDisgust(float value) { disgust = std::clamp(value, 0.0f, 100.0f); }

void HeroEmotions::addFear(float amount) { fear = std::clamp(fear + amount, 0.0f, 100.0f); }
void HeroEmotions::addAnger(float amount) { anger = std::clamp(anger + amount, 0.0f, 100.0f); }
void HeroEmotions::addJoy(float amount) { joy = std::clamp(joy + amount, 0.0f, 100.0f); }
void HeroEmotions::addLove(float amount) { love = std::clamp(love + amount, 0.0f, 100.0f); }
void HeroEmotions::addBravery(float amount) { bravery = std::clamp(bravery + amount, 0.0f, 100.0f); }
void HeroEmotions::addSadness(float amount) { sadness = std::clamp(sadness + amount, 0.0f, 100.0f); }
void HeroEmotions::addDisgust(float amount) { disgust = std::clamp(disgust + amount, 0.0f, 100.0f); }

EmotionIntensity HeroEmotions::getIntensity(float value) const {
    if (value < 10.0f) return EmotionIntensity::NONE;
    if (value < 30.0f) return EmotionIntensity::MILD;
    if (value < 60.0f) return EmotionIntensity::MODERATE;
    if (value < 80.0f) return EmotionIntensity::STRONG;
    return EmotionIntensity::EXTREME;
}

EmotionIntensity HeroEmotions::getFearIntensity() const { return getIntensity(fear); }
EmotionIntensity HeroEmotions::getAngerIntensity() const { return getIntensity(anger); }
EmotionIntensity HeroEmotions::getJoyIntensity() const { return getIntensity(joy); }
EmotionIntensity HeroEmotions::getLoveIntensity() const { return getIntensity(love); }
EmotionIntensity HeroEmotions::getBraveryIntensity() const { return getIntensity(bravery); }
EmotionIntensity HeroEmotions::getSadnessIntensity() const { return getIntensity(sadness); }
EmotionIntensity HeroEmotions::getDisgustIntensity() const { return getIntensity(disgust); }

float HeroEmotions::getDominantEmotion() const {
    return std::max({fear, anger, joy, love, bravery, sadness, disgust});
}

EmotionType HeroEmotions::getDominantEmotionType() const {
    float maxVal = getDominantEmotion();
    if (maxVal == fear) return EmotionType::FEAR;
    if (maxVal == anger) return EmotionType::ANGER;
    if (maxVal == joy) return EmotionType::JOY;
    if (maxVal == love) return EmotionType::LOVE;
    if (maxVal == bravery) return EmotionType::BRAVERY;
    if (maxVal == sadness) return EmotionType::SADNESS;
    return EmotionType::DISGUST;
}

float HeroEmotions::getMovementSpeedModifier() const {
    float modifier = 1.0f;
    if (fear > 50.0f) modifier *= 1.2f;
    if (anger > 50.0f) modifier *= 1.1f;
    if (sadness > 50.0f) modifier *= 0.8f;
    if (joy > 50.0f) modifier *= 1.15f;
    return modifier;
}

float HeroEmotions::getAttackDamageModifier() const {
    float modifier = 1.0f;
    if (anger > 50.0f) modifier *= 1.3f;
    if (bravery > 50.0f) modifier *= 1.2f;
    if (fear > 50.0f) modifier *= 0.7f;
    if (sadness > 50.0f) modifier *= 0.8f;
    return modifier;
}

float HeroEmotions::getDefenseModifier() const {
    float modifier = 1.0f;
    if (bravery > 50.0f) modifier *= 1.2f;
    if (fear > 50.0f) modifier *= 0.6f;
    if (anger > 50.0f) modifier *= 0.9f;
    return modifier;
}

float HeroEmotions::getLuckModifier() const {
    float modifier = 1.0f;
    if (joy > 50.0f) modifier *= 1.3f;
    if (love > 50.0f) modifier *= 1.2f;
    if (sadness > 50.0f) modifier *= 0.7f;
    return modifier;
}

bool HeroEmotions::isAfraid() const { return fear > 50.0f; }
bool HeroEmotions::isEnraged() const { return anger > 70.0f; }
bool HeroEmotions::isHappy() const { return joy > 50.0f; }
bool HeroEmotions::isBrave() const { return bravery > 50.0f; }
bool HeroEmotions::isSad() const { return sadness > 50.0f; }

void HeroEmotions::triggerFear(float intensity) { addFear(intensity); }
void HeroEmotions::triggerAnger(float intensity) { addAnger(intensity); }
void HeroEmotions::triggerJoy(float intensity) { addJoy(intensity); }
void HeroEmotions::triggerLove(float intensity) { addLove(intensity); }
void HeroEmotions::triggerBravery(float intensity) { addBravery(intensity); }

void HeroEmotions::onDamageTaken(float damage) {
    addFear(damage * 0.1f);
    addAnger(damage * 0.2f);
}

void HeroEmotions::onEnemyKilled() {
    addJoy(15.0f);
    addBravery(10.0f);
}

void HeroEmotions::onFriendNearby() {
    addLove(10.0f);
    addFear(-10.0f);
}

void HeroEmotions::onLevelUp() {
    addJoy(30.0f);
    addBravery(20.0f);
}

void HeroEmotions::onQuestCompleted() {
    addJoy(25.0f);
}

void HeroEmotions::setDecayRate(float rate) { decayRate = rate; }
