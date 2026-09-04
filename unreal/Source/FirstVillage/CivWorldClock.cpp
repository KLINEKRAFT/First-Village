#include "CivWorldClock.h"
#include "CivRuntimeEnvironment.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyLightComponent.h"

ACivWorldClock::ACivWorldClock()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ACivWorldClock::BeginPlay()
{
    Super::BeginPlay();
    Environment = Cast<ACivRuntimeEnvironment>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivRuntimeEnvironment::StaticClass()));
    UpdateEnvironment();
}

void ACivWorldClock::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    MinuteOfDay += DeltaSeconds * GameMinutesPerRealSecond;
    if (MinuteOfDay >= 1440.f)
    {
        MinuteOfDay = FMath::Fmod(MinuteOfDay, 1440.f);
        ++Day;
        AdvanceDay();
    }
    UpdateEnvironment();
}

void ACivWorldClock::AdvanceDay()
{
    const float Roll = FMath::FRand();
    Weather = Roll < 0.08f ? ECivWeatherState::Storm :
              Roll < 0.18f ? ECivWeatherState::Rain :
              Roll < 0.28f ? ECivWeatherState::Overcast :
              Roll < 0.33f ? ECivWeatherState::ColdSnap : ECivWeatherState::Clear;
}

void ACivWorldClock::UpdateEnvironment()
{
    const float Hour = GetHourOfDay();
    const float SolarAlpha = (Hour - 6.f) / 12.f;
    const float SunPitch = FMath::Lerp(-5.f, -175.f, SolarAlpha);
    const float Daylight = FMath::Clamp(FMath::Sin(SolarAlpha * PI), 0.f, 1.f);

    float WeatherLight = 1.f;
    if (Weather == ECivWeatherState::Overcast) WeatherLight = 0.65f;
    if (Weather == ECivWeatherState::Rain) WeatherLight = 0.5f;
    if (Weather == ECivWeatherState::Storm) WeatherLight = 0.3f;

    if (Environment)
    {
        if (Environment->Sun)
        {
            Environment->Sun->SetWorldRotation(FRotator(SunPitch, -32.f, 0.f));
            Environment->Sun->SetIntensity(FMath::Lerp(0.02f, 8.5f, Daylight) * WeatherLight);
        }
        if (Environment->SkyLight)
        {
            Environment->SkyLight->SetIntensity(FMath::Lerp(0.08f, 1.0f, Daylight) * FMath::Lerp(0.7f, 1.f, WeatherLight));
        }
        if (Environment->Fog)
        {
            const float WeatherFog = Weather == ECivWeatherState::Storm ? 0.035f : Weather == ECivWeatherState::Rain ? 0.02f : 0.008f;
            Environment->Fog->SetFogDensity(WeatherFog);
        }
    }

    const float DailyTemp = 56.f + 18.f * FMath::Clamp(FMath::Sin(((Hour - 6.f) / 24.f) * 2.f * PI), -0.5f, 1.f);
    TemperatureF = DailyTemp - (Weather == ECivWeatherState::ColdSnap ? 18.f : Weather == ECivWeatherState::Storm ? 7.f : Weather == ECivWeatherState::Rain ? 4.f : 0.f);
}

FString ACivWorldClock::GetClockText() const
{
    const int32 Hour = FMath::FloorToInt(MinuteOfDay / 60.f);
    const int32 Minute = FMath::FloorToInt(FMath::Fmod(MinuteOfDay, 60.f));
    return FString::Printf(TEXT("Day %d  %02d:%02d"), Day, Hour, Minute);
}
