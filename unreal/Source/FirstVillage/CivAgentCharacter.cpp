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

void ACivAgentCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const float Scale = FMath::Max(0.f, NeedTimeScale);
    Hunger = FMath::Clamp(Hunger + DeltaSeconds * 0.055f * Scale, 0.f, 100.f);
    Thirst = FMath::Clamp(Thirst + DeltaSeconds * 0.085f * Scale, 0.f, 100.f);
    Fatigue = FMath::Clamp(Fatigue + DeltaSeconds * 0.045f * Scale, 0.f, 100.f);

    if (Hunger > 90.f)
    {
        Health = FMath::Clamp(Health - DeltaSeconds * 0.08f * Scale, 0.f, 100.f);
    }
    if (Thirst > 92.f)
    {
        Health = FMath::Clamp(Health - DeltaSeconds * 0.16f * Scale, 0.f, 100.f);
    }

    if (Health <= 0.f)
    {
        GetCharacterMovement()->DisableMovement();
        SetActorEnableCollision(false);
    }
}

void ACivAgentCharacter::ApplyNeedDelta(float HungerDelta, float ThirstDelta, float FatigueDelta, float MoraleDelta)
{
    Hunger = FMath::Clamp(Hunger + HungerDelta, 0.f, 100.f);
    Thirst = FMath::Clamp(Thirst + ThirstDelta, 0.f, 100.f);
    Fatigue = FMath::Clamp(Fatigue + FatigueDelta, 0.f, 100.f);
    Morale = FMath::Clamp(Morale + MoraleDelta, 0.f, 100.f);
}

void ACivAgentCharacter::ApplyHealthDelta(float Delta)
{
    Health = FMath::Clamp(Health + Delta, 0.f, 100.f);
}
