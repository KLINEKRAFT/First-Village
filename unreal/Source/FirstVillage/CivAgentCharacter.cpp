#include "CivAgentCharacter.h"
#include "CivAgentMindComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACivAgentCharacter::ACivAgentCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    Mind = CreateDefaultSubobject<UCivAgentMindComponent>(TEXT("Mind"));

    BodyVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyVisual"));
    BodyVisual->SetupAttachment(GetRootComponent());
    BodyVisual->SetRelativeLocation(FVector(0.f, 0.f, -8.f));
    BodyVisual->SetRelativeScale3D(FVector(0.34f, 0.34f, 0.9f));
    BodyVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (UStaticMesh* BodyMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Capsule.Capsule")))
    {
        BodyVisual->SetStaticMesh(BodyMesh);
    }

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
