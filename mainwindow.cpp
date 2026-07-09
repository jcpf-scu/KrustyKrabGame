/**
 * @brief 主菜单界面实现
 */
#include "difficultywindow.h"
#include "mainwindow.h"
#include "gamewindow.h"
#include "introwindow.h"
#include <QLabel>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ========= 窗口基本设置 =========
    setFixedSize(1000, 750);
    setWindowTitle("蟹堡王 - Krusty Krab");

    // ========= 背景层（第一张参考图） =========
    QLabel *bgLabel = new QLabel(this);
    bgLabel->setGeometry(0, 0, 1000, 750);
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

    // ========= 布局常量 =========
    const int startBtnW = 375;
    const int startBtnH = 73;
    const int startBtnX = (1000 - startBtnW) / 2;
    const int startBtnY = 420;

    // ========= 标题图片（居中、醒目展示） =========
    const int titleW = 1000;
    const int titleH = 400;
    const int titleX = 0;
    const int titleY = 8;

    QLabel *titleImage = new QLabel(this);
    titleImage->setGeometry(titleX, titleY, titleW, titleH);
    titleImage->setAlignment(Qt::AlignCenter);
    titleImage->setStyleSheet("background: transparent;");

    QPixmap titlePixmap(":/images/title_logo.png");
    if (titlePixmap.isNull()) {
        titlePixmap.load("images/title_logo.png");
    }
    if (!titlePixmap.isNull()) {
        titleImage->setPixmap(
            titlePixmap.scaled(titleImage->size(),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation));
    }

    // ========= 三个功能按钮 =========
    startBtn = new QPushButton("开始游戏", this);
    startBtn->setGeometry(startBtnX, startBtnY, startBtnW, startBtnH);
    startBtn->setStyleSheet(
        "QPushButton { background-color: #FF5A5F; color: white;"
        "font-size: 38px; font-weight: bold; border-radius: 35px;"
        "border: 2px solid rgba(255,255,255,180); }"
        "QPushButton:hover { background-color: #F24950; }"
    );

    introBtn = new QPushButton("游戏说明", this);
    introBtn->setGeometry(startBtnX, 512, startBtnW, startBtnH);
    introBtn->setStyleSheet(
        "QPushButton { background-color: #4F86C6; color: white;"
        "font-size: 38px; font-weight: bold; border-radius: 35px;"
        "border: 2px solid rgba(255,255,255,180); }"
        "QPushButton:hover { background-color: #3F74B4; }"
    );

    exitBtn = new QPushButton("退出游戏", this);
    exitBtn->setGeometry(startBtnX, 604, startBtnW, startBtnH);
    exitBtn->setStyleSheet(
        "QPushButton { background-color: #7B8794; color: white;"
        "font-size: 38px; font-weight: bold; border-radius: 35px;"
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
    DifficultyWindow *dif = new DifficultyWindow();
    dif->show();
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
