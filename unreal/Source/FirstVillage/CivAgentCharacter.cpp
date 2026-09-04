#include "CivAgentCharacter.h"
#include "CivAgentMindComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace
{
void ConfigureVisual(UStaticMeshComponent* Component, UStaticMesh* Mesh, const FVector& Location, const FVector& Scale)
{
    if (!Component) return;
    Component->SetStaticMesh(Mesh);
    Component->SetRelativeLocation(Location);
    Component->SetRelativeScale3D(Scale);
    Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Component->SetCastShadow(true);
}
}

ACivAgentCharacter::ACivAgentCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    Mind = CreateDefaultSubobject<UCivAgentMindComponent>(TEXT("Mind"));

    BodyVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyVisual"));
    BodyVisual->SetupAttachment(GetRootComponent());
    HeadVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadVisual"));
    HeadVisual->SetupAttachment(GetRootComponent());
    LeftLegVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftLegVisual"));
    LeftLegVisual->SetupAttachment(GetRootComponent());
    RightLegVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightLegVisual"));
    RightLegVisual->SetupAttachment(GetRootComponent());
    LeftArmVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftArmVisual"));
    LeftArmVisual->SetupAttachment(GetRootComponent());
    RightArmVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightArmVisual"));
    RightArmVisual->SetupAttachment(GetRootComponent());

    UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    UStaticMesh* Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

    ConfigureVisual(BodyVisual, Cube, FVector(0.f, 0.f, 30.f), FVector(0.30f, 0.20f, 0.52f));
    ConfigureVisual(HeadVisual, Sphere, FVector(0.f, 0.f, 104.f), FVector(0.23f, 0.23f, 0.25f));
    ConfigureVisual(LeftLegVisual, Cylinder, FVector(0.f, -13.f, -34.f), FVector(0.09f, 0.09f, 0.46f));
    ConfigureVisual(RightLegVisual, Cylinder, FVector(0.f, 13.f, -34.f), FVector(0.09f, 0.09f, 0.46f));
    ConfigureVisual(LeftArmVisual, Cylinder, FVector(0.f, -33.f, 28.f), FVector(0.07f, 0.07f, 0.43f));
    ConfigureVisual(RightArmVisual, Cylinder, FVector(0.f, 33.f, 28.f), FVector(0.07f, 0.07f, 0.43f));

    Nameplate = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Nameplate"));
    Nameplate->SetupAttachment(GetRootComponent());
    Nameplate->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
    Nameplate->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    Nameplate->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
    Nameplate->SetWorldSize(24.f);
    Nameplate->SetTextRenderColor(FColor(235, 226, 204));
    Nameplate->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetCharacterMovement()->MaxWalkSpeed = 260.f;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
}

FLinearColor ACivAgentCharacter::RoleColor() const
{
    const FString Role = Mind ? Mind->Role.ToString() : FString();
    if (Role == TEXT("Hunter") || Role == TEXT("Scout")) return FLinearColor(0.56f, 0.18f, 0.10f, 1.f);
    if (Role == TEXT("Builder") || Role == TEXT("Maker")) return FLinearColor(0.72f, 0.46f, 0.13f, 1.f);
    if (Role == TEXT("Healer")) return FLinearColor(0.78f, 0.72f, 0.56f, 1.f);
    if (Role == TEXT("Farmer") || Role == TEXT("Forager") || Role == TEXT("Gatherer")) return FLinearColor(0.24f, 0.38f, 0.16f, 1.f);
    if (Role == TEXT("Organizer")) return FLinearColor(0.30f, 0.22f, 0.42f, 1.f);
    if (Role == TEXT("Cook")) return FLinearColor(0.48f, 0.28f, 0.13f, 1.f);
    return FLinearColor(0.42f, 0.34f, 0.24f, 1.f);
}

void ACivAgentCharacter::RefreshVisualIdentity()
{
    if (Mind && Nameplate)
    {
        Nameplate->SetText(FText::FromString(FString::Printf(TEXT("%s  ·  %s"), *Mind->DisplayName, *Mind->Role.ToString())));
    }

    UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (BaseMaterial)
    {
        RoleMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        if (RoleMaterial)
        {
            RoleMaterial->SetVectorParameterValue(TEXT("Color"), RoleColor());
            BodyVisual->SetMaterial(0, RoleMaterial);
            LeftLegVisual->SetMaterial(0, RoleMaterial);
            RightLegVisual->SetMaterial(0, RoleMaterial);
        }

        const int32 ToneIndex = Mind ? FMath::Abs(Mind->AgentId) % 4 : 0;
        const FLinearColor SkinTones[] = {
            FLinearColor(0.58f, 0.37f, 0.23f, 1.f),
            FLinearColor(0.48f, 0.29f, 0.17f, 1.f),
            FLinearColor(0.68f, 0.46f, 0.29f, 1.f),
            FLinearColor(0.39f, 0.23f, 0.14f, 1.f)
        };
        if (UMaterialInstanceDynamic* SkinMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this))
        {
            SkinMaterial->SetVectorParameterValue(TEXT("Color"), SkinTones[ToneIndex]);
            HeadVisual->SetMaterial(0, SkinMaterial);
            LeftArmVisual->SetMaterial(0, SkinMaterial);
            RightArmVisual->SetMaterial(0, SkinMaterial);
        }
    }
}

void ACivAgentCharacter::SetObserverSelected(bool bSelected)
{
    bObserverSelected = bSelected;
    if (Nameplate)
    {
        Nameplate->SetTextRenderColor(bSelected ? FColor(255, 201, 78) : FColor(235, 226, 204));
        Nameplate->SetWorldSize(bSelected ? 32.f : 24.f);
    }
}

void ACivAgentCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const float Scale = FMath::Max(0.f, NeedTimeScale);
    Hunger = FMath::Clamp(Hunger + DeltaSeconds * 0.055f * Scale, 0.f, 100.f);
    Thirst = FMath::Clamp(Thirst + DeltaSeconds * 0.085f * Scale, 0.f, 100.f);
    Fatigue = FMath::Clamp(Fatigue + DeltaSeconds * 0.045f * Scale, 0.f, 100.f);

    if (Hunger > 90.f) Health = FMath::Clamp(Health - DeltaSeconds * 0.08f * Scale, 0.f, 100.f);
    if (Thirst > 92.f) Health = FMath::Clamp(Health - DeltaSeconds * 0.16f * Scale, 0.f, 100.f);

    const float Speed = GetVelocity().Size2D();
    const float Swing = Speed > 15.f && GetWorld() ? FMath::Sin(GetWorld()->GetTimeSeconds() * 8.f) * 25.f : 0.f;
    if (LeftLegVisual) LeftLegVisual->SetRelativeRotation(FRotator(Swing, 0.f, 0.f));
    if (RightLegVisual) RightLegVisual->SetRelativeRotation(FRotator(-Swing, 0.f, 0.f));
    if (LeftArmVisual) LeftArmVisual->SetRelativeRotation(FRotator(-Swing * 0.75f, 0.f, 0.f));
    if (RightArmVisual) RightArmVisual->SetRelativeRotation(FRotator(Swing * 0.75f, 0.f, 0.f));

    if (Nameplate)
    {
        if (APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(this, 0))
        {
            const FVector ToCamera = Camera->GetCameraLocation() - Nameplate->GetComponentLocation();
            Nameplate->SetWorldRotation(FRotator(0.f, ToCamera.Rotation().Yaw + 180.f, 0.f));
        }
        APawn* Observer = UGameplayStatics::GetPlayerPawn(this, 0);
        const bool bNearObserver = !Observer || FVector::DistSquared(GetActorLocation(), Observer->GetActorLocation()) < FMath::Square(2600.f);
        Nameplate->SetVisibility(bNearObserver || bObserverSelected);
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
