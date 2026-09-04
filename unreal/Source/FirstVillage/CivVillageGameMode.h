#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CivVillageGameMode.generated.h"

UCLASS()
class FIRSTVILLAGE_API ACivVillageGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACivVillageGameMode();

protected:
    virtual void BeginPlay() override;

private:
    void SpawnRuntimeGround();
    void SpawnNavigationBounds();
    void SpawnWorldDirector();
};
