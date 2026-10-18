// ============================================================
//  InteractableActor.cpp
//  可交互物件的实现（现在很简单，以后可以慢慢加功能）
// ============================================================

#include "InteractableActor.h"
#include "Components/StaticMeshComponent.h"

AInteractableActor::AInteractableActor()
{
    // 可交互物件不需要每帧更新，关掉 Tick 省一点性能
    PrimaryActorTick.bCanEverTick = false;

    // 创建网格组件，并设为根组件（决定这个物件的位置和旋转）
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    // 网格默认要挡视线，这样玩家的交互射线才能打到它
    Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // 默认的话，放进场景后可以在细节面板里改成别的
    Message = TEXT("Hello! I am something interactive.");
}
