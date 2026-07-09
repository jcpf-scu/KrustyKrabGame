/**
 * @brief 难度选择窗口实现
 */

#include "difficultywindow.h"
#include "gamewindow.h"
#include "mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QPixmap>

DifficultyWindow::DifficultyWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(1000, 750);
    setWindowTitle("选择难度");

    // ========= 背景层 =========
    QLabel *bgLabel = new QLabel(this);
    bgLabel->setGeometry(0, 0, 1000, 750);
    bgLabel->setScaledContents(true);
    QPixmap bg(":/images/difficulty_bg.jpg");
    if (bg.isNull()) {
        bg.load("images/difficulty_bg.jpg");
    }
    if (!bg.isNull()) {
        bgLabel->setPixmap(bg.scaled(bgLabel->size(),
                                     Qt::KeepAspectRatioByExpanding,
                                     Qt::FastTransformation));
        bgLabel->setAttribute(Qt::WA_StaticContents, true);
        bgLabel->setAttribute(Qt::WA_OpaquePaintEvent, true);
    } else {
        bgLabel->setStyleSheet("background-color: #F1FAEE;");
    }
    bgLabel->lower();

    // ========= 按钮：对齐背景图黄色内容框，居中并放大 =========
    // 黄色框在 1000×750 窗口下的区域（与 difficulty_bg.jpg 对齐）
    const int boxX = 335;
    const int boxY = 95;
    const int boxW = 635;
    const int boxH = 610;

    const int btnW = 560;
    const int easyH = 110;
    const int hardH = 110;
    const int backH = 95;
    const int btnGap = 35;

    const int btnX = boxX + (boxW - btnW) / 2;
    const int totalBtnH = easyH + hardH + backH + btnGap * 2;
    const int groupStartY = boxY + (boxH - totalBtnH) / 2;
    const int easyY = groupStartY;
    const int hardY = groupStartY + easyH + btnGap;
    const int backY = groupStartY + easyH + btnGap + hardH + btnGap;

    QPushButton *easyBtn = new QPushButton("简单模式\n60 秒 · 基础订单", this);
    easyBtn->setGeometry(btnX, easyY, btnW, easyH);
    easyBtn->setStyleSheet(
        "QPushButton { background-color: #2A9D8F; color: white;"
        "font-size: 36px; font-weight: bold; border-radius: 45px;"
        "border: 2px solid rgba(255,255,255,180);"
        "font-family: 'Microsoft YaHei', sans-serif; }"
        "QPushButton:hover { background-color: #21867A; }"
    );

    QPushButton *hardBtn = new QPushButton("困难模式\n60 秒 · 复杂订单", this);
    hardBtn->setGeometry(btnX, hardY, btnW, hardH);
    hardBtn->setStyleSheet(
        "QPushButton { background-color: #E63946; color: white;"
        "font-size: 36px; font-weight: bold; border-radius: 45px;"
        "border: 2px solid rgba(255,255,255,180);"
        "font-family: 'Microsoft YaHei', sans-serif; }"
        "QPushButton:hover { background-color: #C1121F; }"
    );

    QPushButton *backBtn = new QPushButton("返回大厅", this);
    backBtn->setGeometry(btnX, backY, btnW, backH);
    backBtn->setStyleSheet(
        "QPushButton { background-color: #7B8794; color: white;"
        "font-size: 48px; font-weight: bold; border-radius: 45px;"
        "border: 2px solid rgba(255,255,255,180);"
        "font-family: 'Microsoft YaHei', sans-serif; }"
        "QPushButton:hover { background-color: #66717D; }"
    );

    connect(easyBtn, &QPushButton::clicked, this, &DifficultyWindow::onEasyClicked);
    connect(hardBtn, &QPushButton::clicked, this, &DifficultyWindow::onHardClicked);
    connect(backBtn, &QPushButton::clicked, this, &DifficultyWindow::onBackClicked);
}

DifficultyWindow::~DifficultyWindow() {}

void DifficultyWindow::onEasyClicked()
{
    hide();
    GameWindow *game = new GameWindow(GameDifficulty::Easy);
    game->show();
    close();
}

void DifficultyWindow::onHardClicked()
{
    hide();
    GameWindow *game = new GameWindow(GameDifficulty::Hard);
    game->show();
    close();
}

void DifficultyWindow::onBackClicked()
{
    hide();
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
    close();
}
