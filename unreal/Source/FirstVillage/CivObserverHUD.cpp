#include "CivObserverHUD.h"
#include "CivWorldClock.h"
#include "CivWorldDirector.h"
#include "CivAgentApiSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"
#include "Engine/GameInstance.h"

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
    UCivAgentApiSubsystem* Api = GetWorld()->GetGameInstance() ? GetWorld()->GetGameInstance()->GetSubsystem<UCivAgentApiSubsystem>() : nullptr;

    float Y = 28.f;
    const float X = 28.f;
    DrawText(TEXT("FIRST VILLAGE"), FLinearColor(0.95f, 0.82f, 0.58f), X, Y, nullptr, 1.35f, false); Y += 28.f;

    if (Clock)
    {
        DrawText(Clock->GetClockText(), FLinearColor::White, X, Y, nullptr, 1.05f, false); Y += 21.f;
        DrawText(FString::Printf(TEXT("%s  ·  %.0f F"), *WeatherName(Clock->Weather), Clock->TemperatureF), FLinearColor(0.82f, 0.84f, 0.78f), X, Y); Y += 27.f;
    }

    if (Director)
    {
        DrawText(FString::Printf(TEXT("Population  %d"), Director->Agents.Num()), FLinearColor::White, X, Y); Y += 19.f;
        DrawText(FString::Printf(TEXT("Food %.0f   Water %.0f"), Director->FoodStore, Director->WaterStore), FLinearColor::White, X, Y); Y += 19.f;
        DrawText(FString::Printf(TEXT("Wood %.0f   Stone %.0f   Clay %.0f   Thatch %.0f"), Director->WoodStore, Director->StoneStore, Director->ClayStore, Director->ThatchStore), FLinearColor::White, X, Y); Y += 24.f;
    }

    if (Api)
    {
        const FLinearColor BrainColor = Api->bBackendReachable ? FLinearColor(0.48f, 0.85f, 0.52f) : FLinearColor(0.92f, 0.72f, 0.30f);
        DrawText(Api->GetStatusText(), BrainColor, X, Y, nullptr, 0.95f, false);
    }

    const float Bottom = Canvas->SizeY - 64.f;
    DrawText(TEXT("C / Home  camp    Tab  next villager    F  nearest villager"), FLinearColor(0.86f, 0.82f, 0.72f), 28.f, Bottom, nullptr, 0.9f, false);
    DrawText(TEXT("WASD  fly    Q / E  descend / rise    mouse  look"), FLinearColor(0.68f, 0.70f, 0.66f), 28.f, Bottom + 20.f, nullptr, 0.85f, false);
}
