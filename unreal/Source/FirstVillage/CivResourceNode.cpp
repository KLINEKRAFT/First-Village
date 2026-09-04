#include "CivResourceNode.h"
#include "Components/StaticMeshComponent.h"

ACivResourceNode::ACivResourceNode()
{
    PrimaryActorTick.bCanEverTick = false;
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
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
