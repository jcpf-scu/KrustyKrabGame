# KrustyKrabGame

> 蟹堡王烹饪模拟游戏 — 基于 Qt 的桌面休闲游戏  
> 项目路径：**D:\KrustyKrabGame**  
> 参考 [potato-sandwich-shop](https://github.com/AriaaaCHEN/potato-sandwich-shop) 项目模板开发

---

## 项目简介

在蟹堡王厨房里，你是一名厨师。根据顾客订单，使用 **底面包、生肉饼、顶面包** 等食材，制作出完美的 **蟹黄堡**！

当前版本为 **v0.1 基础版**，包含：
- 主菜单（开始 / 说明 / 退出）
- 简单模式：60 秒倒计时 + 得分
- 汉堡制作核心玩法（取料 → 煎制 → 提交）
- 连击奖励、最高分记录、星级评价
- 暂停菜单

---

## 项目结构

```
D:\KrustyKrabGame\
├── KrustyKrabGame.pro   # Qt 项目文件（用 Qt Creator 打开这个）
├── main.cpp             # 程序入口
├── mainwindow.h/cpp     # 主菜单
├── gamewindow.h/cpp     # 游戏主界面（核心玩法）
├── menuwindow.h/cpp     # 暂停菜单
├── introwindow.h/cpp    # 游戏说明
└── README.md
```

---

## 如何运行

1. 用 Qt Creator 打开 **`D:\KrustyKrabGame\KrustyKrabGame.pro`**
2. 左下角 Kit 选：**Desktop Qt 5.6.1 MinGW 32bit**
3. **构建 → 清除全部 → 重新构建项目**
4. 点击 **绿色三角（运行）** 或按 **Ctrl+R**

或在命令行：

```bash
cd D:\KrustyKrabGame
qmake KrustyKrabGame.pro
mingw32-make
debug\KrustyKrabGame.exe
```

---

## 代码说明

- 所有界面用 **C++ 代码** 编写，没有 `.ui` 设计器文件
- 每个 `.h` / `.cpp` 文件顶部有 **@file / @brief** 注释
- 关键逻辑处配有中文行内注释，方便学习阅读

---

## 后续计划

- [ ] 添加图片资源（背景、按钮美化）
- [ ] 更多食材（芝士、生菜、番茄酱）
- [ ] 难度选择（简单 / 普通 / 困难）
- [ ] 随机订单系统
- [ ] 更多菜品（炸薯条、蟹肉堡等）
