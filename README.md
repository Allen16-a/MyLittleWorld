# My Little World (Unreal Engine 版)

我的第一个 3D 虚拟世界项目，用 **Unreal Engine 5 + C++** 开发，
场景模型用 **Blender** 手工制作。这是一个学习项目，也是我为大学
申请准备的作品，代码里写了很多注释，希望对其他初学者有帮助。

## 目前的功能

- 第一人称漫游（WASD 移动 + 鼠标视角）
- 视线交互：看着一个物件按 E，它会“开口说话”
- 可交互物件系统（`InteractableActor`）：往场景里放一个，
  填一句话就能用
- 所有场景模型为 Blender 自制

## 运行这个项目

1. 安装 Unreal Engine 5 和 Visual Studio Community 2022
   （勾选「使用 C++ 的游戏开发」工作负载）
2. 克隆本仓库
3. 右键 `MyLittleWorld.uproject` → Generate Visual Studio project files
4. 双击 `.uproject` 打开，按提示编译
5. 按 Play 运行

> 注意：本项目基于 Unreal Engine，使用需遵守
> [Epic 的 EULA](https://www.unrealengine.com/eula)。
> 本仓库中我个人编写的代码以 MIT 协议开源，
> 自制模型（`Content/Models/`）同样可自由使用。

## 操作

| 按键 | 作用 |
|------|------|
| W / A / S / D | 移动 |
| 鼠标 | 视角 |
| E | 与注视的物件交互 |

## 代码导读

- `Source/MyLittleWorld/MyWorldCharacter.h/.cpp`
  玩家角色：移动、视角、交互射线检测
- `Source/MyLittleWorld/InteractableActor.h/.cpp`
  可交互物件：外观 + 一句话
- `工具链工作流指南.md`
  我记录的三个软件（UE / Blender / VSCode）配合使用的完整流程

## 开发日志

见 `devlog/` 文件夹，记录每周的开发过程和踩坑笔记。

## License

MIT（仅限我个人编写的代码与模型；Unreal Engine 本身受 Epic EULA 约束）
