#include "CivCampBeacon.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

namespace
{
UMaterialInstanceDynamic* MakeTint(UObject* Outer, const FLinearColor& Color)
{
    UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (!Base) return nullptr;
    UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, Outer);
    if (MID) MID->SetVectorParameterValue(TEXT("Color"), Color);
    return MID;
}
}

ACivCampBeacon::ACivCampBeacon()
{
    PrimaryActorTick.bCanEverTick = true;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    UStaticMesh* Rock = Sphere;

    FireCore = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FireCore"));
    FireCore->SetupAttachment(Root);
    FireCore->SetStaticMesh(Sphere);
    FireCore->SetRelativeLocation(FVector(0.f, 0.f, 28.f));
    FireCore->SetRelativeScale3D(FVector(0.38f, 0.38f, 0.65f));
    FireCore->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    HearthStoneA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HearthStoneA"));
    HearthStoneA->SetupAttachment(Root);
    HearthStoneA->SetStaticMesh(Rock);
    HearthStoneA->SetRelativeLocation(FVector(42.f, 0.f, 12.f));
    HearthStoneA->SetRelativeScale3D(FVector(0.32f, 0.24f, 0.18f));

    HearthStoneB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HearthStoneB"));
    HearthStoneB->SetupAttachment(Root);
    HearthStoneB->SetStaticMesh(Rock);
    HearthStoneB->SetRelativeLocation(FVector(-22.f, 36.f, 12.f));
    HearthStoneB->SetRelativeScale3D(FVector(0.32f, 0.24f, 0.18f));

    HearthStoneC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HearthStoneC"));
    HearthStoneC->SetupAttachment(Root);
    HearthStoneC->SetStaticMesh(Rock);
    HearthStoneC->SetRelativeLocation(FVector(-22.f, -36.f, 12.f));
    HearthStoneC->SetRelativeScale3D(FVector(0.32f, 0.24f, 0.18f));

    for (UStaticMeshComponent* Stone : { HearthStoneA.Get(), HearthStoneB.Get(), HearthStoneC.Get() })
    {
        if (Stone) Stone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    FireLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FireLight"));
    FireLight->SetupAttachment(Root);
    FireLight->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
    FireLight->SetIntensity(1800.f);
    FireLight->SetAttenuationRadius(850.f);
    FireLight->SetLightColor(FLinearColor(1.f, 0.38f, 0.08f));
    FireLight->SetCastShadows(false);

    Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SettlementLabel"));
    Label->SetupAttachment(Root);
    Label->SetRelativeLocation(FVector(0.f, 0.f, 270.f));
    Label->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    Label->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
    Label->SetWorldSize(38.f);
    Label->SetText(FText::FromString(TEXT("FIRST VILLAGE")));
    Label->SetTextRenderColor(FColor(255, 214, 135));
    Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACivCampBeacon::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!FireCore->GetMaterial(0))
    {
        if (UMaterialInstanceDynamic* Fire = MakeTint(this, FLinearColor(1.f, 0.16f, 0.02f, 1.f))) FireCore->SetMaterial(0, Fire);
        if (UMaterialInstanceDynamic* Stone = MakeTint(this, FLinearColor(0.18f, 0.16f, 0.13f, 1.f)))
        {
            HearthStoneA->SetMaterial(0, Stone);
            HearthStoneB->SetMaterial(0, Stone);
            HearthStoneC->SetMaterial(0, Stone);
        }
    }

    const float Pulse = 0.88f + FMath::Sin(GetWorld() ? GetWorld()->GetTimeSeconds() * 9.f : 0.f) * 0.12f;
    FireCore->SetRelativeScale3D(FVector(0.38f, 0.38f, 0.65f) * Pulse);
    FireLight->SetIntensity(1600.f + Pulse * 420.f);

    if (APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(this, 0))
    {
        const FVector ToCamera = Camera->GetCameraLocation() - Label->GetComponentLocation();
        Label->SetWorldRotation(FRotator(0.f, ToCamera.Rotation().Yaw + 180.f, 0.f));
    }
}
