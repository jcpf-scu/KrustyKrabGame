/**
 * @file mainwindow.cpp
 * @brief 主菜单界面实现（参考 potato-sandwich-shop，用代码布局，不用 .ui 文件）
 */

#include "mainwindow.h"
#include "gamewindow.h"
#include "introwindow.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ========= 窗口基本设置 =========
    setFixedSize(800, 600);
    setWindowTitle("蟹堡王 - Krusty Krab");

    // ========= 背景层 =========
    QLabel *bgLabel = new QLabel(this);
    bgLabel->setGeometry(0, 0, 800, 600);
    bgLabel->setStyleSheet("background-color: #F1FAEE;");
    bgLabel->lower();  // 放到最底层

    // ========= 标题文字 =========
    QLabel *title = new QLabel("蟹堡王烹饪挑战", this);
    title->setGeometry(100, 80, 600, 60);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 36px; font-weight: bold; color: #E63946;"
        "font-family: 'Microsoft YaHei', sans-serif; background: transparent;"
    );

    QLabel *subtitle = new QLabel("在限时内做出完美的蟹黄堡！", this);
    subtitle->setGeometry(100, 150, 600, 40);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet(
        "font-size: 18px; color: #457B9D; background: transparent;"
    );

    // ========= 三个功能按钮 =========
    startBtn = new QPushButton("开始游戏", this);
    startBtn->setGeometry(250, 280, 300, 55);
    startBtn->setStyleSheet(
        "QPushButton { background-color: #E63946; color: white;"
        "font-size: 20px; font-weight: bold; border-radius: 8px; }"
        "QPushButton:hover { background-color: #C1121F; }"
    );

    introBtn = new QPushButton("游戏说明", this);
    introBtn->setGeometry(250, 360, 300, 55);
    introBtn->setStyleSheet(
        "QPushButton { background-color: #457B9D; color: white;"
        "font-size: 20px; font-weight: bold; border-radius: 8px; }"
        "QPushButton:hover { background-color: #1D3557; }"
    );

    exitBtn = new QPushButton("退出游戏", this);
    exitBtn->setGeometry(250, 440, 300, 55);
    exitBtn->setStyleSheet(
        "QPushButton { background-color: #6C757D; color: white;"
        "font-size: 20px; font-weight: bold; border-radius: 8px; }"
        "QPushButton:hover { background-color: #495057; }"
    );

    // ========= 信号槽连接 =========
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(introBtn, &QPushButton::clicked, this, &MainWindow::onIntroClicked);
    connect(exitBtn,  &QPushButton::clicked, this, &MainWindow::onExitClicked);
}

MainWindow::~MainWindow() {}

// 点击「开始游戏」：打开游戏窗口，关闭当前大厅
void MainWindow::onStartClicked()
{
    GameWindow *game = new GameWindow();
    game->show();
    close();
}

// 点击「游戏说明」：弹出说明窗口（模态叠在大厅上方）
void MainWindow::onIntroClicked()
{
    IntroWindow *intro = new IntroWindow(this);
    intro->show();
}

// 点击「退出游戏」：关闭主窗口，程序结束
void MainWindow::onExitClicked()
{
    close();
}
