// World/DayNightSystem.cpp
#include "DayNightSystem.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

DayNightSystem::DayNightSystem()
    : timeOfDay(0.25f)
    , dayLength(300.0f)
    , timeSpeed(1.0f)
    , dayCount(1)
    , currentTime(TimeOfDay::DAY)
    , currentWeather(WeatherType::CLEAR)
    , weatherTimer(0.0f)
    , weatherDuration(120.0f)
    , visibilityModifier(1.0f)
    , enemyAggressionModifier(1.0f)
    , resourceGatherModifier(1.0f)
    , temperatureModifier(0.0f) {
}

void DayNightSystem::update(float deltaTime) {
    timeOfDay += (deltaTime * timeSpeed) / dayLength;
    if (timeOfDay >= 1.0f) {
        timeOfDay -= 1.0f;
        dayCount++;
    }

    updateTimeOfDay();
    updateWeather(deltaTime);
    recalculateModifiers();
}

void DayNightSystem::updateTimeOfDay() {
    if (timeOfDay < 0.2f) currentTime = TimeOfDay::NIGHT;
    else if (timeOfDay < 0.3f) currentTime = TimeOfDay::DAWN;
    else if (timeOfDay < 0.7f) currentTime = TimeOfDay::DAY;
    else if (timeOfDay < 0.8f) currentTime = TimeOfDay::DUSK;
    else currentTime = TimeOfDay::NIGHT;
}

void DayNightSystem::updateWeather(float deltaTime) {
    weatherTimer += deltaTime;
    if (weatherTimer >= weatherDuration) {
        weatherTimer = 0.0f;
        int weatherChance = std::rand() % 100;
        if (weatherChance < 50) currentWeather = WeatherType::CLEAR;
        else if (weatherChance < 70) currentWeather = WeatherType::RAIN;
        else if (weatherChance < 85) currentWeather = WeatherType::FOG;
        else if (weatherChance < 95) currentWeather = WeatherType::SNOW;
        else currentWeather = WeatherType::STORM;

        std::ostringstream oss;
        oss << "[WEATHER] Changed to: " << static_cast<int>(currentWeather);
        std::cout << oss.str() << std::endl;
    }
}

void DayNightSystem::recalculateModifiers() {
    switch (currentTime) {
    case TimeOfDay::DAWN:
        visibilityModifier = 0.8f;
        enemyAggressionModifier = 0.7f;
        resourceGatherModifier = 1.1f;
        temperatureModifier = -0.1f;
        break;
    case TimeOfDay::DAY:
        visibilityModifier = 1.0f;
        enemyAggressionModifier = 1.0f;
        resourceGatherModifier = 1.0f;
        temperatureModifier = 0.2f;
        break;
    case TimeOfDay::DUSK:
        visibilityModifier = 0.7f;
        enemyAggressionModifier = 1.2f;
        resourceGatherModifier = 0.9f;
        temperatureModifier = 0.0f;
        break;
    case TimeOfDay::NIGHT:
        visibilityModifier = 0.3f;
        enemyAggressionModifier = 1.5f;
        resourceGatherModifier = 0.6f;
        temperatureModifier = -0.3f;
        break;
    }

    switch (currentWeather) {
    case WeatherType::CLEAR:
        visibilityModifier *= 1.0f;
        break;
    case WeatherType::RAIN:
        visibilityModifier *= 0.8f;
        resourceGatherModifier *= 0.9f;
        break;
    case WeatherType::STORM:
        visibilityModifier *= 0.5f;
        enemyAggressionModifier *= 1.3f;
        break;
    case WeatherType::SNOW:
        visibilityModifier *= 0.7f;
        resourceGatherModifier *= 0.7f;
        temperatureModifier -= 0.5f;
        break;
    case WeatherType::FOG:
        visibilityModifier *= 0.4f;
        enemyAggressionModifier *= 0.8f;
        break;
    }
}

TimeOfDay DayNightSystem::getTimeOfDay() const { return currentTime; }
WeatherType DayNightSystem::getWeather() const { return currentWeather; }
float DayNightSystem::getTimeProgress() const { return timeOfDay; }
float DayNightSystem::getDayLength() const { return dayLength; }
int DayNightSystem::getDay() const { return dayCount; }
bool DayNightSystem::isNight() const { return currentTime == TimeOfDay::NIGHT; }
bool DayNightSystem::isDawn() const { return currentTime == TimeOfDay::DAWN; }
bool DayNightSystem::isDusk() const { return currentTime == TimeOfDay::DUSK; }

float DayNightSystem::getVisibility() const { return visibilityModifier; }
float DayNightSystem::getEnemyAggression() const { return enemyAggressionModifier; }
float DayNightSystem::getResourceGatherRate() const { return resourceGatherModifier; }
float DayNightSystem::getTemperature() const { return temperatureModifier; }

void DayNightSystem::setDayLength(float length) { dayLength = length; }
void DayNightSystem::setTimeSpeed(float speed) { timeSpeed = speed; }
void DayNightSystem::forceWeather(WeatherType weather) { currentWeather = weather; weatherTimer = 0.0f; }
void DayNightSystem::setWeatherDuration(float duration) { weatherDuration = duration; }

float DayNightSystem::getCraftingBonus() const {
    if (currentTime == TimeOfDay::NIGHT) return 1.3f;
    if (currentTime == TimeOfDay::DAWN) return 1.1f;
    return 1.0f;
}

float DayNightSystem::getRestBonus() const {
    if (currentTime == TimeOfDay::NIGHT) return 1.5f;
    if (currentTime == TimeOfDay::DUSK) return 1.2f;
    return 1.0f;
}

bool DayNightSystem::shouldSpawnNightEnemies() const {
    return currentTime == TimeOfDay::NIGHT || currentTime == TimeOfDay::DUSK;
}
