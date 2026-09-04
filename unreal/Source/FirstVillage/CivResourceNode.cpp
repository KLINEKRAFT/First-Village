#include "CivResourceNode.h"
#include "Components/StaticMeshComponent.h"

ACivResourceNode::ACivResourceNode()
{
    PrimaryActorTick.bCanEverTick = false;
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
    RefreshVisual();
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
}

float ACivResourceNode::Harvest(float RequestedAmount)
{
    if (RequestedAmount <= 0.f || Quantity <= 0.f)
    {
        return 0.f;
    }

    const float Taken = FMath::Min(RequestedAmount, Quantity);
    Quantity -= Taken;

    if (Quantity <= KINDA_SMALL_NUMBER && !bRenewable)
    {
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
    }

    return Taken;
}
