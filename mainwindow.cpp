/**
 * @brief 主菜单界面实现
 */

#include "mainwindow.h"
#include "introwindow.h"
#include "difficultywindow.h"
#include <QLabel>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(1000, 750);
    setWindowTitle("蟹堡王 - Krusty Krab");

    QLabel *bgLabel = new QLabel(this);
    bgLabel->setGeometry(0, 0, 1000, 750);
    bgLabel->setScaledContents(true);
    QPixmap bg(":/images/start_bg.jpg");
    if (bg.isNull()) {
        bg.load("images/start_bg.jpg");
    }
    if (!bg.isNull()) {
        bgLabel->setPixmap(bg.scaled(bgLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::FastTransformation));
        bgLabel->setAttribute(Qt::WA_StaticContents, true);
        bgLabel->setAttribute(Qt::WA_OpaquePaintEvent, true);
    } else {
        bgLabel->setStyleSheet("background-color: #8EE7E1;");
    }
    bgLabel->lower();

    const int btnW = 375;
    const int btnH = 73;
    const int btnX = (1000 - btnW) / 2;
    const int btnY = 420;

    QLabel *titleImage = new QLabel(this);
    titleImage->setGeometry(0, 8, 1000, 400);
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
                               Qt::FastTransformation));
        titleImage->setAttribute(Qt::WA_StaticContents, true);
    }

    startBtn = new QPushButton("开始游戏", this);
    startBtn->setGeometry(btnX, btnY, btnW, btnH);
    startBtn->setStyleSheet(
        "QPushButton { background-color: #FF5A5F; color: white;"
        "font-size: 38px; font-weight: bold; border-radius: 35px;"
        "border: 2px solid rgba(255,255,255,180); }"
        "QPushButton:hover { background-color: #F24950; }"
    );

    introBtn = new QPushButton("游戏说明", this);
    introBtn->setGeometry(btnX, btnY + 92, btnW, btnH);
    introBtn->setStyleSheet(
        "QPushButton { background-color: #4F86C6; color: white;"
        "font-size: 38px; font-weight: bold; border-radius: 35px;"
        "border: 2px solid rgba(255,255,255,180); }"
        "QPushButton:hover { background-color: #3F74B4; }"
    );

    exitBtn = new QPushButton("退出游戏", this);
    exitBtn->setGeometry(btnX, btnY + 184, btnW, btnH);
    exitBtn->setStyleSheet(
        "QPushButton { background-color: #7B8794; color: white;"
        "font-size: 38px; font-weight: bold; border-radius: 35px;"
        "border: 2px solid rgba(255,255,255,180); }"
        "QPushButton:hover { background-color: #66717D; }"
    );

    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartClicked);
    connect(introBtn, &QPushButton::clicked, this, &MainWindow::onIntroClicked);
    connect(exitBtn,  &QPushButton::clicked, this, &MainWindow::onExitClicked);
}

MainWindow::~MainWindow() {}

void MainWindow::onStartClicked()
{
    hide();
    DifficultyWindow *diffWin = new DifficultyWindow();
    diffWin->show();
    close();
}

void MainWindow::onIntroClicked()
{
    IntroWindow *intro = new IntroWindow(this);
    intro->show();
}

void MainWindow::onExitClicked()
{
    close();
}
