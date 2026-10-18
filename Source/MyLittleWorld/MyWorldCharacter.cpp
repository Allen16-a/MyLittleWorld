// ============================================================
//  MyWorldCharacter.cpp
//  玩家角色的具体实现
//
//  核心思路（也是大多数第一人称游戏的做法）：
//    视角：鼠标 → 直接转控制器（Controller）的角度
//    移动：根据控制器朝向算出“前/右”方向，只取水平分量
//    交互：从相机向前发一条射线，看打中了谁
// ============================================================

#include "MyWorldCharacter.h"
#include "InteractableActor.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"   // 移动组件：管速度、跳跃、碰撞
#include "Engine/Engine.h"            // GEngine，用来往屏幕上打印文字
#include "DrawDebugHelpers.h"         // 画调试用的辅助线

AMyWorldCharacter::AMyWorldCharacter()
{
    // 允许 Tick 每帧运行（我们要用它检测“正在看着什么”）
    PrimaryActorTick.bCanEverTick = true;

    // 创建相机，挂在角色身上，位置大约在“眼睛”的高度
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(GetRootComponent());
    Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
    // 让相机跟着控制器的旋转走（鼠标转 = 视角转）
    Camera->bUsePawnControlRotation = true;

    // 接管相机的“动态模糊”参数，默认不模糊
    // bOverride_ 开头 = “这个参数别用场景默认的，听我的”
    Camera->PostProcessSettings.bOverride_MotionBlurAmount = true;
    Camera->PostProcessSettings.MotionBlurAmount = 0.0f;

    // 出生时的移动速度设为普通速度
    // GetCharacterMovement() 拿到的是从 ACharacter 继承来的“移动组件”
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMyWorldCharacter::BeginPlay()
{
    Super::BeginPlay();
    // 目前这里什么都不用做，先留着。
    // 以后想加“开局显示欢迎语”之类的逻辑就写在这里。
}

// ------------------------------------------------------------
//  移动：前后走
// ------------------------------------------------------------
void AMyWorldCharacter::MoveForward(float Value)
{
    // Value：按 W 是 +1，按 S 是 -1，没按是 0
    if (Controller && Value != 0.0f)
    {
        // 取控制器的朝向，但只保留水平旋转（Yaw）
        // 这样抬头看天时按 W 也是往前走，而不是往天上飞
        const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

// ------------------------------------------------------------
//  移动：左右平移
// ------------------------------------------------------------
void AMyWorldCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        // 和上面同理，只不过取的是“右”方向（Y 轴）
        const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

// ------------------------------------------------------------
//  视角：鼠标直接转成控制器的角度
// ------------------------------------------------------------
void AMyWorldCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);     // 左右
}

void AMyWorldCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);   // 上下
}

// ------------------------------------------------------------
//  工具：发射一条“视线”，看玩家正盯着什么
//  射线检测（LineTrace）是游戏里最常用的技术之一：
//  从 A 点到 B 点画一条看不见的线，返回第一个撞到的物体
// ------------------------------------------------------------
AInteractableActor* AMyWorldCharacter::FindLookedInteractable() const
{
    FVector Start = Camera->GetComponentLocation();
    FVector End   = Start + Camera->GetForwardVector() * InteractDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);   // 别让射线打到玩家自己

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, Start, End, ECC_Visibility, Params);

    if (bHit && Hit.GetActor())
    {
        // 打中的东西是不是“可交互物件”？是就返回它，不是就当没看到
        return Cast<AInteractableActor>(Hit.GetActor());
    }
    return nullptr;
}

// ------------------------------------------------------------
//  疾跑：开始 / 停止
//  这两个函数只负责“改状态”（速度和开关），
//  画面特效（FOV、模糊）由 Tick 每帧平滑处理——
//  分开写的好处：特效是渐变的，不会“啪”地突然变化
// ------------------------------------------------------------
void AMyWorldCharacter::StartSprint()
{
    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMyWorldCharacter::StopSprint()
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
void AMyWorldCharacter::Interact()
{
    AInteractableActor* Target = FindLookedInteractable();
    if (Target && GEngine)
    {
        // 最简单的显示方式：直接往屏幕左上角打印，3 秒后自动消失
        // 以后学了 UMG（UE 的界面系统）再换成漂亮的对话框
        GEngine->AddOnScreenDebugMessage(
            /*Key=*/-1, /*Time=*/3.0f, FColor::White, Target->Message);
    }
}

// ------------------------------------------------------------
//  Tick：每一帧检查玩家是不是正看着可交互的东西
//  是的话显示一个提示。这就是“靠近物件出现 [E]”的原理
// ------------------------------------------------------------
void AMyWorldCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ---- 疾跑特效：每帧把数值往目标平滑推一点 ----
    // FInterpTo(当前值, 目标值, 帧时间, 速度) = 返回一个“更靠近目标”的新值
    // 这就是“渐变”的原理：不是瞬间切换，而是每帧挪一点
    const float TargetFOV  = bIsSprinting ? SprintFOV : NormalFOV;   // 三元表达式：条件 ? 是 : 否
    Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, TargetFOV, DeltaTime, 8.0f));

    const float TargetBlur = bIsSprinting ? 0.8f : 0.0f;
    Camera->PostProcessSettings.MotionBlurAmount = FMath::FInterpTo(
        Camera->PostProcessSettings.MotionBlurAmount, TargetBlur, DeltaTime, 6.0f);

    // ---- 交互提示：正看着可交互物件就显示 [E] ----
    AInteractableActor* Target = FindLookedInteractable();
    if (Target && GEngine)
    {
        // Key 用固定数字 100：同样的 Key 会刷新同一条消息而不是刷屏
        GEngine->AddOnScreenDebugMessage(
            /*Key=*/100, /*Time=*/0.0f, FColor::Yellow, TEXT("[E] Interact"));
    }
}

// ------------------------------------------------------------
//  输入绑定：告诉引擎“哪个键对应哪个函数”
//  注意：按键本身要在 项目设置 → Input 里配置（见指南第 4 步）
// ------------------------------------------------------------
void AMyWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Axis（轴）：输出连续数值的输入，比如 WASD、鼠标移动
    PlayerInputComponent->BindAxis("MoveForward", this, &AMyWorldCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight",   this, &AMyWorldCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn",        this, &AMyWorldCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp",      this, &AMyWorldCharacter::LookUp);

    // Action（动作）：按下去触发一次，比如 E 交互
    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMyWorldCharacter::Interact);

    // 跳跃：不用自己写函数，父亲 ACharacter 自带 Jump()，直接绑定它的地址
    // IE_Pressed / IE_Released = 按下时 / 松开时分别触发
    PlayerInputComponent->BindAction("Jump", IE_Pressed,  this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    // 疾跑：按住 Shift 提速，松开恢复——所以两个时机都要绑
    PlayerInputComponent->BindAction("Sprint", IE_Pressed,  this, &AMyWorldCharacter::StartSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMyWorldCharacter::StopSprint);
}