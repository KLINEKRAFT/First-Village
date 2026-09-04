#include "CivObserverHUD.h"
#include "CivWorldClock.h"
#include "CivWorldDirector.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"

namespace
{
FString WeatherName(ECivWeatherState State)
{
    switch (State)
    {
        case ECivWeatherState::Clear: return TEXT("Clear");
        case ECivWeatherState::Overcast: return TEXT("Overcast");
        case ECivWeatherState::Rain: return TEXT("Rain");
        case ECivWeatherState::Storm: return TEXT("Storm");
        case ECivWeatherState::ColdSnap: return TEXT("Cold snap");
    }
    return TEXT("Unknown");
}
}

void ACivObserverHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas || !GetWorld()) return;

    ACivWorldClock* Clock = Cast<ACivWorldClock>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivWorldClock::StaticClass()));
    ACivWorldDirector* Director = Cast<ACivWorldDirector>(UGameplayStatics::GetActorOfClass(GetWorld(), ACivWorldDirector::StaticClass()));

    float Y = 34.f;
    const float X = 34.f;
    if (Clock)
    {
        DrawText(Clock->GetClockText(), FLinearColor::White, X, Y, nullptr, 1.1f, false); Y += 24.f;
        DrawText(FString::Printf(TEXT("%s  %.0f F"), *WeatherName(Clock->Weather), Clock->TemperatureF), FLinearColor(0.85f,0.85f,0.8f), X, Y); Y += 30.f;
    }
    if (Director)
    {
        DrawText(FString::Printf(TEXT("Population: %d"), Director->Agents.Num()), FLinearColor::White, X, Y); Y += 20.f;
        DrawText(FString::Printf(TEXT("Food %.0f   Water %.0f"), Director->FoodStore, Director->WaterStore), FLinearColor::White, X, Y); Y += 20.f;
        DrawText(FString::Printf(TEXT("Wood %.0f   Stone %.0f   Clay %.0f   Thatch %.0f"), Director->WoodStore, Director->StoneStore, Director->ClayStore, Director->ThatchStore), FLinearColor::White, X, Y);
    }
}
