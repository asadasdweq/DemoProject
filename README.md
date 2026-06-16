# DemoProject - UE5 Gameplay Demo

这是一个基于 Unreal Engine 5.5 和 C++ 开发的 ARPG 玩法 Demo，重点展示 Gameplay Ability System、角色战斗、背包、装备、UI、敌人 AI 以及多系统整合能力。

项目主要用于 UE 初级开发岗位投递和面试展示。相比单独的技能 Demo 或背包 Demo，本项目更关注“系统之间如何协作”：角色通过 GAS 管理能力和属性，背包通过 Item Manifest / Fragment 管理物品数据，装备和消耗品通过 GameplayEffect 接入 GAS，从而让物品属性、角色战斗和 UI 表现保持在同一套数据流中。

## 项目状态

- 引擎版本：Unreal Engine 5.5
- 开发语言：C++
- 项目类型：单人 / 联机基础结构 Demo
- 当前用途：个人作品集、面试展示、Gameplay 系统练习
- 演示素材：截图和视频待补充

## 核心功能

### Gameplay Ability System

- 使用 GAS 搭建角色能力框架。
- ASC 和 AttributeSet 放在 PlayerState 上，支持玩家角色的属性复制和能力管理。
- 支持 FireBolt、Lightning、Electrocute、ChargeRock、近战攻击等能力示例。
- 使用 GameplayTag 管理输入、技能、冷却、事件、属性和状态。
- 实现 Health、Mana、Power、Intelligence、Vigor、Resilience、Armor、Damage、Resistance 等属性。
- 通过 AttributeSet 处理属性 Clamp、伤害结算、死亡、受击反应和飘字显示。
- 使用 WidgetController 监听属性变化并广播给 HUD。

### 背包系统

- 基于 `UInventoryComponent` 管理背包逻辑。
- 使用 `FFastArraySerializer` 同步背包物品列表。
- 支持拾取、堆叠、丢弃、消耗、装备等基础流程。
- 拾取逻辑包含服务端距离校验，避免客户端直接修改背包状态。
- 使用 Item Manifest 描述物品，使用 Item Fragment 拆分图标、文本、网格大小、堆叠、消耗、装备词条等数据。
- 使用 UMG 实现网格背包、拖拽、悬浮物品、物品描述、拾取提示和信息提示。

### GAS 与背包整合

这是本项目最重要的展示点。

- 背包系统不直接修改角色属性，而是通过 GAS 的 GameplayEffect 修改属性。
- 消耗品 Fragment 在使用时创建 GameplayEffectSpec，并通过 SetByCaller 传入恢复数值。
- 装备 Fragment 在穿戴时应用 GameplayEffect，在卸下时移除对应 GameplayEffect。
- 装备词条可以影响 Intelligence、Vigor、Resilience 等角色属性。
- 属性变化仍然走 GAS 的 AttributeSet、Replication 和 UI Delegate，避免背包系统和战斗系统各自维护一套属性。
- 背包打开时会阻断技能输入，避免 UI 操作和战斗操作互相冲突。

简化流程：

```text
拾取物品
-> InventoryComponent 服务端校验并加入背包
-> FastArray 同步物品数据
-> 背包 UI 更新
-> 装备 / 消耗物品
-> Item Fragment 创建或移除 GameplayEffect
-> GAS AttributeSet 更新属性
-> HUD 通过 Delegate 刷新显示
```

### 装备系统

- 使用 `UInv_EquipmentComponent` 监听背包装备和卸装事件。
- 装备时生成 EquipActor，并附着到角色骨骼 Socket。
- 武器装备会同步角色武器 Mesh，并影响角色是否持有武器的状态。
- 装备属性通过 GameplayEffect 接入 GAS。
- 装备表现层和属性层分离：视觉附着由 EquipmentComponent 处理，数值修改由 GAS 处理。

### UI 系统

- 使用 UMG 实现 HUD、血量、蓝量、技能图标、冷却显示、敌人血条和伤害飘字。
- 使用 WidgetController 将 GAS 数据与 UI 解耦。
- 背包 UI 支持网格占用、装备槽、物品悬浮提示、描述面板和拾取消息。

### 敌人 AI 与战斗

- 实现 Enemy 角色、AIController、Behavior Tree Service 和攻击 Task。
- 敌人拥有独立 ASC 和 AttributeSet。
- 敌人血量变化会同步到血条 UI。
- 受击后可触发 HitReact，死亡时走 CombatInterface 流程。

## 技术栈

- Unreal Engine 5.5
- C++
- Gameplay Ability System
- GameplayTags
- GameplayTasks
- Enhanced Input
- UMG / Slate
- AI Module
- FastArraySerializer
- Replicated SubObject
- Motion Warping
- Niagara

## 代码结构

```text
Source/DemoProject/
├── AbilitySystem/          GAS 组件、属性、技能、伤害计算、AbilityTask
├── Actors/                 火球、滚石等技能相关 Actor
├── AI/                     AIController、BTService、BTTask
├── Characters/             玩家角色、敌人角色、角色基类
├── EquipmentManagement/    装备组件、装备 Actor、代理 Mesh
├── Input/                  Enhanced Input 与技能输入 Tag 绑定
├── Interaction/            战斗、敌人、高亮等交互接口
├── InventoryManagement/    背包组件、FastArray、背包工具函数
├── Items/                  物品组件、InventoryItem、Manifest、Fragment、ItemTag
├── Player/                 PlayerController、PlayerState
├── Types/                  背包网格和类型定义
└── UI/                     HUD、WidgetController、背包 UI、飘字、血条
```

## 重点源码

- `Source/DemoProject/Private/Player/DemoPlayerState.cpp`
  创建并复制 ASC / AttributeSet，设置 GAS 复制模式。

- `Source/DemoProject/Private/Characters/DemoCharacter.cpp`
  初始化 AbilityActorInfo、默认属性、HUD，并处理拾取范围、武器 Mesh 同步。

- `Source/DemoProject/Private/AbilitySystem/DemoAbilitySystemComponent.cpp`
  处理技能输入 Tag、Ability 激活、输入按下/释放和技能信息广播。

- `Source/DemoProject/Private/AbilitySystem/DemoAttributeSet.cpp`
  处理属性复制、Clamp、伤害结算、死亡、受击反应和伤害飘字。

- `Source/DemoProject/Private/InventoryManagement/Components/InventoryComponent.cpp`
  处理拾取、堆叠、丢弃、消耗、装备事件和背包 UI 开关。

- `Source/DemoProject/Private/InventoryManagement/FastArray/Inv_FastArray.cpp`
  使用 FastArraySerializer 同步背包物品变化。

- `Source/DemoProject/Private/Items/Fragments/Inv_ItemFragment.cpp`
  物品 Fragment 的核心逻辑，包括消耗品和装备通过 GameplayEffect 接入 GAS。

- `Source/DemoProject/Private/EquipmentManagement/Components/Inv_EquipmentComponent.cpp`
  处理装备 Actor 生成、附着、卸下和角色武器状态同步。

## 面试展示建议

推荐按照下面的顺序演示项目：

1. 展示角色移动、技能释放、敌人受击、血条和伤害飘字。
2. 展示打开背包、拾取物品、物品堆叠和网格占用。
3. 展示消耗药水，说明它通过 GameplayEffect 修改 Health 或 Mana。
4. 展示装备物品，说明装备词条通过 GameplayEffect 修改角色属性。
5. 展示装备 Actor 附着到角色身上，并同步武器状态。
6. 打开代码讲解 `InventoryComponent -> ItemFragment -> GameplayEffect -> AttributeSet -> UI` 这条链路。

## 如何打开项目

1. 安装 Unreal Engine 5.5。
2. 克隆或下载本仓库。
3. 打开 `DemoProject.uproject`。
4. 如有需要，右键 `.uproject` 重新生成 Visual Studio 项目文件。
5. 使用 Unreal Editor 或 Visual Studio 编译项目。
6. 打开默认地图进行测试。

## 仓库内容

本仓库建议保留：

- `Config/`
- `Content/`
- `Source/`
- `DemoProject.uproject`
- `README.md`

以下目录属于生成文件，通常不需要提交：

- `Binaries/`
- `Intermediate/`
- `Saved/`
- `.vs/`
- `DerivedDataCache/`

## 演示素材

当前还没有截图和视频，后续建议补充：

- 角色释放技能截图
- 背包网格与装备槽截图
- 装备前后属性变化截图
- 1 到 2 分钟玩法演示视频
- 可选：打包版本下载链接

建议截图文件放在：

```text
Docs/Images/
```

后续可以在这里补充：

```markdown
![Combat Demo](Docs/Images/combat-demo.png)
![Inventory Demo](Docs/Images/inventory-demo.png)
![Equipment Demo](Docs/Images/equipment-demo.png)
```

## 我的职责

- 搭建 GAS 角色能力框架和 AttributeSet。
- 实现技能输入 Tag 绑定、技能激活、冷却和 UI 广播。
- 实现角色属性、伤害结算、受击反馈、死亡和飘字显示。
- 实现基于 Item Manifest / Fragment 的背包物品数据结构。
- 实现背包拾取、堆叠、丢弃、消耗和装备流程。
- 使用 FastArraySerializer 同步背包物品变化。
- 将装备和消耗品通过 GameplayEffect 接入 GAS。
- 实现装备 Actor 附着、角色武器 Mesh 同步和装备状态切换。
- 实现 HUD、背包 UI、物品描述、拾取提示和敌人血条等 UMG 交互。

## 可继续优化

- 补充截图、视频和打包版本链接。
- 增加装备前后属性对比 UI。
- 增加存档系统，保存背包和装备状态。
- 增加更多自动化测试或 PIE 多客户端测试记录。
- 增加更完整的技能升级、装备词条和掉落表配置。
