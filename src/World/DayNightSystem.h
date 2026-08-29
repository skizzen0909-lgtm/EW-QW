// World/DayNightSystem.h
#ifndef DAYNIGHTSYSTEM_H
#define DAYNIGHTSYSTEM_H

enum class TimeOfDay {
    DAWN,
    DAY,
    DUSK,
    NIGHT
};

enum class WeatherType {
    CLEAR,
    RAIN,
    STORM,
    SNOW,
    FOG
};

class DayNightSystem {
private:
    float timeOfDay;
    float dayLength;
    float timeSpeed;
    int dayCount;

    TimeOfDay currentTime;
    WeatherType currentWeather;
    float weatherTimer;
    float weatherDuration;

    float visibilityModifier;
    float enemyAggressionModifier;
    float resourceGatherModifier;
    float temperatureModifier;

    void updateTimeOfDay();
    void updateWeather(float deltaTime);
    void recalculateModifiers();

public:
    DayNightSystem();
    ~DayNightSystem() = default;

    void update(float deltaTime);

    TimeOfDay getTimeOfDay() const;
    WeatherType getWeather() const;
    float getTimeProgress() const;
    float getDayLength() const;
    int getDay() const;
    bool isNight() const;
    bool isDawn() const;
    bool isDusk() const;

    float getVisibility() const;
    float getEnemyAggression() const;
    float getResourceGatherRate() const;
    float getTemperature() const;

    void setDayLength(float length);
    void setTimeSpeed(float speed);
    void forceWeather(WeatherType weather);
    void setWeatherDuration(float duration);

    float getCraftingBonus() const;
    float getRestBonus() const;
    bool shouldSpawnNightEnemies() const;

    DayNightSystem(const DayNightSystem&) = delete;
    DayNightSystem& operator=(const DayNightSystem&) = delete;
};

#endif // DAYNIGHTSYSTEM_H
