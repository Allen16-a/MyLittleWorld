// ============================================================
//  InteractableActor.h
//  可交互物件：世界里“有话可说”的东西，比如告示牌、房子、树
//
//  一个可交互物件 = 一个外观（静态网格）+ 一句话
//  在 UE 编辑器里把它拖进场景，填上想说的话就能用
// ============================================================

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class MYLITTLEWORLD_API AInteractableActor : public AActor
{
    GENERATED_BODY()

public:
    AInteractableActor();

    // 这个物件的“话”：玩家按 E 时显示
    // EditAnywhere = 可以在编辑器的细节面板里直接编辑
    // 所以同一类物件可以摆出很多个，每个说不同的话
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
    FString Message;

    // 外观：一个静态网格（可以是方块，也可以是 Blender 做的模型）
    UPROPERTY(VisibleAnywhere, Category = "Appearance")
    UStaticMeshComponent* Mesh;
};
