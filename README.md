# KrustyKrabGame

> 基于 Qt Widgets 的蟹堡王烹饪小游戏  
> 项目路径：`D:\KrustyKrabGame`

## 项目简介

在 60 秒倒计时内，根据订单完成蟹黄堡制作并获取金币。  
游戏包含主菜单、难度选择、玩法说明、暂停菜单、评分与最高分记录。

当前版本已实现：

- 主菜单：开始游戏 / 游戏说明 / 退出游戏
- 难度选择：简单模式、困难模式
- 订单系统：按难度随机生成订单
- 烹饪逻辑：添加食材、煎制、糊锅判定、丢弃重做
- 判分系统：基础分、连击奖励、星级结算
- 数据持久化：最高分保存到 `maxscore.dat`

## 玩法说明

- 简单模式：基础订单
- 困难模式：包含更多食材（如薯条、可乐）及更复杂订单
- 正确提交：`+10`
- 错误提交：`-5`（并清空连击）
- 连击奖励：3 连击额外 `+5`，5 连击额外 `+10`

## 资源说明

主要图片资源在 `images/` 目录，通过 `images.qrc` 管理。当前关键资源：

- 难度页背景：`images/bg-difficult.png`
- 简单按钮：`images/di-simple.png`
- 困难按钮：`images/di-hard.jpg`
- 返回按钮：`images/di-return.png`
- 简单模式游戏背景：`images/game_bg.jpg`
- 困难模式游戏背景：`images/game_hard_bg.jpg`

## 运行方式

### Qt Creator

1. 打开 `KrustyKrabGame.pro`
2. 选择 `Desktop Qt 5.6.1 MinGW 32bit`
3. 执行 `构建 -> Run qmake`
4. 执行 `构建 -> 重新构建项目`
5. 运行程序

### 命令行（已配置 Qt 环境时）

```bash
qmake KrustyKrabGame.pro
mingw32-make
debug\KrustyKrabGame.exe
```

## 常见问题

### 1) 修改图片后没生效

通常是资源缓存未刷新。执行一次：

- `Run qmake`
- `Rebuild`

### 2) 报错：`No rule to make target 'images/xxx'`

说明构建依赖仍引用旧文件名（常见于改了图片后缀/名称）。  
请确认：

- `images.qrc` 中资源文件名正确
- 重新执行 `Run qmake` 后再编译

## 代码结构

- `main.cpp`：程序入口
- `mainwindow.*`：主菜单
- `difficultywindow.*`：难度选择页
- `introwindow.*`：游戏说明页
- `gamewindow.*`：核心玩法（订单、烹饪、计时、结算）
- `menuwindow.*`：暂停菜单
