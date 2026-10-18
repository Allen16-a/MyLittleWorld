// ============================================================
//  MyWorldCharacter.h
//  玩家角色：第一人称视角 + 走路 + 按 E 交互
//
//  这是「头文件」——只写“这个角色有什么、能做什么”，
//  具体“怎么做”写在对应的 .cpp 文件里。
//  C++ 项目基本都是这样一个 .h 配一个 .cpp。
// ============================================================

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyWorldCharacter.generated.h"   // UE 自动生成的代码，必须在最后一个 include

class UCameraComponent;   // 前向声明：告诉编译器“有这么个类”，细节用到时再引入

UCLASS()
class MYLITTLEWORLD_API AMyWorldCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // 构造函数：角色被创建时跑一次，用来设置默认值
    AMyWorldCharacter();

    // Tick：每一帧都会被调用（比如每秒 60 次）
    virtual void Tick(float DeltaTime) override;

    // 把键盘鼠标的输入绑定到下面的函数上
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    // BeginPlay：游戏开始、角色出现在世界里时跑一次
    virtual void BeginPlay() override;

    // ---- 移动和视角（会被绑定到 WASD 和鼠标）----
    void MoveForward(float Value);   // 前后走（W/S）
    void MoveRight(float Value);     // 左右平移（A/D）
    void Turn(float Value);          // 左右转头（鼠标横向）
    void LookUp(float Value);        // 抬头低头（鼠标纵向）

    // ---- 交互（按 E）----
    void Interact();

    // ---- 疾跑（按住 Shift）----
    void StartSprint();   // 按下 Shift 时调用：提速 + 开模糊
    void StopSprint();    // 松开 Shift 时调用：恢复速度 + 关模糊
    // 跳跃不用自己写函数——父亲 ACharacter 自带 Jump()，直接绑定就行

private:
    // 玩家的“眼睛”
    // UPROPERTY 是 UE 的标记，写上之后这个变量才能在编辑器里被看到、被引擎管理
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    UCameraComponent* Camera;

    // 交互距离，单位是厘米（UE 的默认单位），300cm = 3米
    // EditAnywhere 表示可以在编辑器里直接改这个数，不用改代码
    UPROPERTY(EditAnywhere, Category = "Interact")
    float InteractDistance = 300.0f;

    // ---- 移动参数（全都可以在编辑器里调手感）----
    UPROPERTY(EditAnywhere, Category = "Movement")
    float WalkSpeed = 600.0f;     // 普通走路速度（厘米/秒）

    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintSpeed = 1200.0f;  // 疾跑速度

    UPROPERTY(EditAnywhere, Category = "Movement")
    float NormalFOV = 90.0f;      // 平时的视野角度

    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintFOV = 100.0f;     // 疾跑时的视野角度（更广 = 更有冲刺感）

    bool bIsSprinting = false;    // 此刻是否正在疾跑（b 开头 = 布尔变量，UE 命名规矩）

    // 工具函数：返回玩家此刻正看着的可交互物件（没有就返回 nullptr）
    class AInteractableActor* FindLookedInteractable() const;
};