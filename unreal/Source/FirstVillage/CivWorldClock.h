#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CivWorldClock.generated.h"

class ACivRuntimeEnvironment;

UENUM(BlueprintType)
enum class ECivWeatherState : uint8
{
    Clear,
    Overcast,
    Rain,
    Storm,
    ColdSnap
};

UCLASS()
class FIRSTVILLAGE_API ACivWorldClock : public AActor
{
    GENERATED_BODY()

public:
    ACivWorldClock();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Time")
    float GameMinutesPerRealSecond = 4.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Time")
    int32 Day = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Time")
    float MinuteOfDay = 420.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weather")
    ECivWeatherState Weather = ECivWeatherState::Clear;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weather")
    float TemperatureF = 68.f;

    UFUNCTION(BlueprintPure, Category="Time")
    float GetHourOfDay() const { return MinuteOfDay / 60.f; }

    UFUNCTION(BlueprintPure, Category="Time")
    FString GetClockText() const;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    TObjectPtr<ACivRuntimeEnvironment> Environment;
    void AdvanceDay();
    void UpdateEnvironment();
};
