#include "CivResourceNode.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

ACivResourceNode::ACivResourceNode()
{
    PrimaryActorTick.bCanEverTick = true;
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));

    Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
    Label->SetupAttachment(RootComponent);
    Label->SetRelativeLocation(FVector(0.f, 0.f, 145.f));
    Label->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    Label->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
    Label->SetWorldSize(20.f);
    Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RefreshVisual();
}

FString ACivResourceNode::ResourceLabel() const
{
    switch (ResourceType)
    {
        case ECivResourceType::Water: return TEXT("WATER");
        case ECivResourceType::Food: return TEXT("FOOD");
        case ECivResourceType::Wood: return TEXT("WOOD");
        case ECivResourceType::Stone: return TEXT("STONE");
        case ECivResourceType::Clay: return TEXT("CLAY");
        case ECivResourceType::Game: return TEXT("GAME");
    }
    return TEXT("RESOURCE");
}

FLinearColor ACivResourceNode::ResourceColor() const
{
    switch (ResourceType)
    {
        case ECivResourceType::Water: return FLinearColor(0.05f, 0.32f, 0.48f, 1.f);
        case ECivResourceType::Food: return FLinearColor(0.42f, 0.12f, 0.08f, 1.f);
        case ECivResourceType::Wood: return FLinearColor(0.19f, 0.09f, 0.035f, 1.f);
        case ECivResourceType::Stone: return FLinearColor(0.30f, 0.30f, 0.28f, 1.f);
        case ECivResourceType::Clay: return FLinearColor(0.52f, 0.20f, 0.09f, 1.f);
        case ECivResourceType::Game: return FLinearColor(0.35f, 0.24f, 0.12f, 1.f);
    }
    return FLinearColor::White;
}

void ACivResourceNode::RefreshVisual()
{
    if (!Mesh) return;

    const TCHAR* MeshPath = TEXT("/Engine/BasicShapes/Sphere.Sphere");
    FVector Scale(0.8f);

    switch (ResourceType)
    {
        case ECivResourceType::Water:
            MeshPath = TEXT("/Engine/BasicShapes/Cylinder.Cylinder");
            Scale = FVector(1.8f, 1.8f, 0.12f);
            break;
        case ECivResourceType::Food:
            MeshPath = TEXT("/Engine/BasicShapes/Sphere.Sphere");
            Scale = FVector(0.65f, 0.65f, 1.1f);
            break;
        case ECivResourceType::Wood:
            MeshPath = TEXT("/Engine/BasicShapes/Cylinder.Cylinder");
            Scale = FVector(0.35f, 0.35f, 1.7f);
            break;
        case ECivResourceType::Stone:
            MeshPath = TEXT("/Engine/BasicShapes/Sphere.Sphere");
            Scale = FVector(0.95f, 0.8f, 0.6f);
            break;
        case ECivResourceType::Clay:
            MeshPath = TEXT("/Engine/BasicShapes/Cube.Cube");
            Scale = FVector(0.8f, 0.8f, 0.28f);
            break;
        case ECivResourceType::Game:
            MeshPath = TEXT("/Engine/BasicShapes/Capsule.Capsule");
            Scale = FVector(0.45f, 0.45f, 0.75f);
            break;
    }

    if (UStaticMesh* Shape = LoadObject<UStaticMesh>(nullptr, MeshPath))
    {
        Mesh->SetStaticMesh(Shape);
        Mesh->SetRelativeScale3D(Scale);
    }

    if (UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial")))
    {
        if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Base, this))
        {
            MID->SetVectorParameterValue(TEXT("Color"), ResourceColor());
            Mesh->SetMaterial(0, MID);
        }
    }

    if (Label)
    {
        Label->SetText(FText::FromString(ResourceLabel()));
        Label->SetTextRenderColor(ResourceColor().ToFColor(true));
        Label->SetVisibility(ResourceType != ECivResourceType::Water);
    }
}

void ACivResourceNode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!Label || !Label->IsVisible()) return;

    APawn* Observer = UGameplayStatics::GetPlayerPawn(this, 0);
    const float DistanceSq = Observer ? FVector::DistSquared(Observer->GetActorLocation(), GetActorLocation()) : 0.f;
    Label->SetVisibility(!Observer || DistanceSq < FMath::Square(2200.f));

    if (APlayerCameraManager* Camera = UGameplayStatics::GetPlayerCameraManager(this, 0))
    {
        const FVector ToCamera = Camera->GetCameraLocation() - Label->GetComponentLocation();
        Label->SetWorldRotation(FRotator(0.f, ToCamera.Rotation().Yaw + 180.f, 0.f));
    }
}

float ACivResourceNode::Harvest(float RequestedAmount)
{
    if (RequestedAmount <= 0.f || Quantity <= 0.f) return 0.f;

    const float Taken = FMath::Min(RequestedAmount, Quantity);
    Quantity -= Taken;

    if (Quantity <= KINDA_SMALL_NUMBER && !bRenewable)
    {
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
    }

    return Taken;
}
