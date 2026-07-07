/**
 * @file mainwindow.cpp
 * @brief 主菜单界面实现（参考 potato-sandwich-shop，用代码布局，不用 .ui 文件）
 */

#include "mainwindow.h"
#include "gamewindow.h"
#include "introwindow.h"
#include <QLabel>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ========= 窗口基本设置 =========
    setFixedSize(800, 600);
    setWindowTitle("蟹堡王 - Krusty Krab");

    // ========= 背景层（第一张参考图） =========
    QLabel *bgLabel = new QLabel(this);
    bgLabel->setGeometry(0, 0, 800, 600);
    bgLabel->setScaledContents(true);
    QPixmap bg(":/images/start_bg.jpg");
    if (bg.isNull()) {
        bg.load("images/start_bg.jpg");
    }
    if (!bg.isNull()) {
        bgLabel->setPixmap(bg.scaled(bgLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    } else {
        bgLabel->setStyleSheet("background-color: #8EE7E1;");
    }
    bgLabel->lower();  // 放到最底层

    // ========= 标题文字 =========
    QLabel *title = new QLabel("蟹堡王\n烹饪挑战", this);
    title->setGeometry(180, 84, 440, 150);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 62px; font-weight: 900; color: #FFDD00;"
        "line-height: 1.08;"
        "text-shadow: 2px 2px 0 #1D3557;"
        "font-family: 'Microsoft YaHei', sans-serif; background: transparent;"
    );

    QLabel *subtitle = new QLabel("限时内做出美美的蟹黄堡！", this);
    subtitle->setGeometry(225, 235, 350, 44);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: #2F3A4A;"
        "background-color: rgba(255, 255, 255, 230);"
        "border: 2px solid #B7D6F5; border-radius: 18px;"
    );

    // ========= 三个功能按钮 =========
    startBtn = new QPushButton("开始游戏", this);
    startBtn->setGeometry(250, 305, 300, 58);
    startBtn->setStyleSheet(
        "QPushButton { background-color: #FF5A5F; color: white;"
        "font-size: 30px; font-weight: bold; border-radius: 28px;"
        "border: 2px solid rgba(255,255,255,180); }"
        "QPushButton:hover { background-color: #F24950; }"
    );

    introBtn = new QPushButton("游戏说明", this);
    introBtn->setGeometry(250, 378, 300, 58);
    introBtn->setStyleSheet(
        "QPushButton { background-color: #4F86C6; color: white;"
        "font-size: 30px; font-weight: bold; border-radius: 28px;"
        "border: 2px solid rgba(255,255,255,180); }"
        "QPushButton:hover { background-color: #3F74B4; }"
    );

    exitBtn = new QPushButton("退出游戏", this);
    exitBtn->setGeometry(250, 451, 300, 58);
    exitBtn->setStyleSheet(
        "QPushButton { background-color: #7B8794; color: white;"
        "font-size: 30px; font-weight: bold; border-radius: 28px;"
        "border: 2px solid rgba(255,255,255,180); }"
        "QPushButton:hover { background-color: #66717D; }"
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
