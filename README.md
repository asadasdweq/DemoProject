# UE5 GAS Gameplay Demo

这是一个基于 Unreal Engine 5.5 和 C++ 开发的第三人称俯视角 Gameplay Demo，主要用于展示 UE 客户端 / Gameplay 开发能力。

项目核心内容包括 Gameplay Ability System 技能系统、背包装备系统、客户端技能表现、物品拾取与丢弃、AI 追击、UMG UI 以及基础网络同步流程。

## 演示视频

[UE5 GAS + 背包装备系统客户端演示](Showcase/UE5_GAS_Inventory_Demo_Client_Showcase.mp4)

如果 GitHub 页面无法直接播放视频，可以点击 Download 下载查看。

## 项目信息

- 引擎版本：Unreal Engine 5.5
- 开发语言：C++、Blueprint
- 项目类型：个人 Gameplay Demo
- 演示重点：GAS 技能、背包装备、客户端同步、UI、AI
- 求职方向：UE 客户端开发 / UE Gameplay 开发 / UE C++ 初级开发

## 技术栈

- Unreal Engine 5.5
- C++
- Gameplay Ability System
- GameplayTag
- GameplayEffect / GameplayCue / TargetData
- Enhanced Input
- UMG
- FastArraySerializer
- Replicated SubObject
- Motion Warping
- Niagara
- AIController / Behavior Tree / NavMesh

## 核心功能

### GAS 技能系统

- 将玩家的 AbilitySystemComponent 和 AttributeSet 放在 PlayerState 中，支持更稳定的玩家属性和技能管理。
- 基于 GameplayTag 绑定技能输入，PlayerController 只负责传递输入 Tag，AbilitySystemComponent 根据 Tag 激活对应 Ability。
- 实现火球、闪电链、滚石蓄力等技能，覆盖目标选择、技能释放、伤害结算、特效表现和冷却流程。
- 使用 GameplayEffect 和 ExecutionCalculation 处理伤害，支持物理伤害、魔法伤害、抗性计算、受击反应和死亡处理。
- 使用自定义 TargetData 传递滚石技能的目标点和蓄力百分比，使蓄力数据在客户端和服务器之间表达更清晰。
- 技能释放期间支持通用输入阻挡 Tag，避免持续施法时其他技能输入打断当前技能逻辑。

### 滚石蓄力技能

- 按住技能键后角色进入施法状态，并显示头顶蓄力进度条。
- 蓄力时间影响石头大小和最终伤害。
- 松开技能键后，服务器根据客户端传来的目标点和蓄力百分比生成滚石。
- 滚石沿鼠标方向滚动，命中敌人造成物理伤害，达到最大距离或碰撞阻挡后销毁。
- 客户端能够看到蓄力后的滚石大小和发射表现。

### 背包与装备系统

- 使用 UInventoryComponent 管理背包逻辑。
- 使用 FastArraySerializer 同步背包物品列表。
- 物品数据基于 ItemManifest 和 ItemFragment 组合，支持图标、文本、格子大小、堆叠、消耗品、装备属性等配置。
- 支持物品拾取、堆叠、拖拽、装备、卸下、消耗、丢弃和重新生成地上物品。
- 拾取和堆叠逻辑包含服务器校验，避免客户端直接修改背包权威数据。
- 装备和消耗品通过 GameplayEffect 接入 GAS，使物品效果能够进入统一的属性系统。

### 装备表现

- 装备物品后生成 EquipActor 并附着到角色对应 Socket。
- 武器装备后同步角色武器 Mesh 和持武器状态。
- 技能生成位置会根据角色当前武器 CombatSocket 更新，保证火球、闪电链等技能从正确位置释放。

### UI 系统

- 使用 UMG 实现血量、蓝量、技能图标、冷却、敌人血条、背包界面和伤害飘字。
- 使用 WidgetController 监听 GAS 属性变化，将 AttributeSet 与 UI 表现解耦。
- 背包 UI 支持网格占用、物品拖拽、堆叠数量显示、装备槽、物品描述和拾取提示。
- 打开背包时会阻挡技能输入，避免 UI 操作和战斗输入冲突。

### AI 与战斗

- 敌人拥有独立的 AbilitySystemComponent 和 AttributeSet。
- 使用 NavMesh 支持敌人追击玩家。
- 敌人受到伤害后刷新血条，触发受击反应；生命值归零后走死亡流程。

## 代码结构

```text
Source/DemoProject/
├── AbilitySystem/          GAS 组件、AttributeSet、GameplayAbility、伤害计算、AbilityTask
├── Actors/                 火球、滚石等技能相关 Actor
├── AI/                     AIController、Behavior Tree Service、Behavior Tree Task
├── Characters/             玩家角色、敌人角色、角色基类
├── EquipmentManagement/    装备组件、装备 Actor、代理 Mesh
├── Input/                  Enhanced Input 与技能输入 Tag 绑定
├── Interaction/            战斗、敌人、高亮等交互接口
├── InventoryManagement/    背包组件、FastArray、背包工具函数
├── Items/                  ItemComponent、InventoryItem、Manifest、Fragment、ItemTag
├── Player/                 PlayerController、PlayerState
├── Types/                  背包网格和通用类型定义
└── UI/                     HUD、WidgetController、背包 UI、血条、伤害飘字
```

## 重点源码

- `Source/DemoProject/Private/Player/DemoPlayerState.cpp`  
  创建并复制玩家 AbilitySystemComponent 和 AttributeSet。

- `Source/DemoProject/Private/Characters/DemoCharacter.cpp`  
  初始化 AbilityActorInfo、默认属性、HUD，以及处理拾取范围和武器 Mesh 同步。

- `Source/DemoProject/Private/AbilitySystem/DemoAbilitySystemComponent.cpp`  
  处理技能输入 Tag、技能激活、输入按下 / 释放，以及技能信息广播。

- `Source/DemoProject/Private/AbilitySystem/DemoAttributeSet.cpp`  
  处理属性复制、属性 Clamp、伤害接收、死亡、受击反应和伤害飘字。

- `Source/DemoProject/Private/AbilitySystem/ExecCalc/ExecCalc_Damage.cpp`  
  使用 ExecutionCalculation 计算最终伤害。

- `Source/DemoProject/Private/AbilitySystem/Abilities/DemoChargeRockAbility.cpp`  
  实现滚石蓄力、目标数据发送、服务器生成滚石和客户端表现同步。

- `Source/DemoProject/Private/InventoryManagement/Components/InventoryComponent.cpp`  
  处理拾取、堆叠、丢弃、消耗、装备事件和背包 UI 开关。

- `Source/DemoProject/Private/InventoryManagement/FastArray/Inv_FastArray.cpp`  
  使用 FastArraySerializer 同步背包物品变化。

- `Source/DemoProject/Private/Items/Fragments/Inv_ItemFragment.cpp`  
  实现物品 Fragment 的核心逻辑，包括消耗品和装备通过 GameplayEffect 接入 GAS。

- `Source/DemoProject/Private/EquipmentManagement/Components/Inv_EquipmentComponent.cpp`  
  处理装备 Actor 生成、附着、卸下和角色武器状态同步。

## 项目说明

本项目为个人学习和求职作品，重点展示对 UE C++、GAS、Gameplay、背包装备系统和网络同步流程的理解。项目中部分美术资源和基础学习路线参考公开课程与学习资料，个人在此基础上完成了技能扩展、背包装备联动、客户端同步问题修复和整体系统整合。

## 运行方式

1. 安装 Unreal Engine 5.5。
2. 克隆或下载本仓库。
3. 打开 `DemoProject.uproject`。
4. 如有需要，右键 `.uproject` 重新生成 Visual Studio 项目文件。
5. 使用 Unreal Editor 或 Visual Studio 编译并运行项目。
