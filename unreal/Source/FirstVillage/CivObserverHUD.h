#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CivObserverHUD.generated.h"

UCLASS()
class FIRSTVILLAGE_API ACivObserverHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;
};
