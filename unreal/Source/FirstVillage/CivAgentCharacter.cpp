#include "CivAgentCharacter.h"
#include "CivAgentMindComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACivAgentCharacter::ACivAgentCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    Mind = CreateDefaultSubobject<UCivAgentMindComponent>(TEXT("Mind"));

    GetCharacterMovement()->MaxWalkSpeed = 260.f;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
}

void ACivAgentCharacter::ApplyNeedDelta(float HungerDelta, float ThirstDelta, float FatigueDelta, float MoraleDelta)
{
    Hunger = FMath::Clamp(Hunger + HungerDelta, 0.f, 100.f);
    Thirst = FMath::Clamp(Thirst + ThirstDelta, 0.f, 100.f);
    Fatigue = FMath::Clamp(Fatigue + FatigueDelta, 0.f, 100.f);
    Morale = FMath::Clamp(Morale + MoraleDelta, 0.f, 100.f);
}
