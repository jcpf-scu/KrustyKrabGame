#include "difficultywindow.h"
#include "mainwindow.h"
#include "gamewindow.h"

#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QSize>

DifficultyWindow::DifficultyWindow(QWidget *parent):QMainWindow(parent){
    setFixedSize(1000,750);
    //背景图
    QPixmap bg(":/images/bg-difficult.png");
    bg = bg.scaled(1000,750,Qt::KeepAspectRatioByExpanding);
    QLabel *bgLabel = new QLabel(this);
    bgLabel->setPixmap(bg);
    bgLabel->setGeometry(0,0,1000,750);
    bgLabel->lower();

    const int btnX = 550;
    const int btnW = 240;
    const int btnH = 70;
    auto setupOptionButton = [&](QPushButton *btn, const QString &imagePath, int y) {
        QPixmap pix;
        if (!pix.load(imagePath)) {
            // 资源路径加载失败时，回退到本地文件路径，避免 qrc 未刷新导致按钮不显示
            QString filePath = imagePath;
            filePath.remove(":/");
            pix.load(filePath);
        }

        btn->setFixedSize(btnW, btnH);
        btn->setGeometry(btnX, y, btnW, btnH);
        btn->setCursor(Qt::PointingHandCursor);
        if (!pix.isNull()) {
            btn->setIcon(QIcon(pix));
            btn->setIconSize(QSize(btnW, btnH));
            btn->setText("");
        }
        btn->setStyleSheet(
            QString(
            "QPushButton {"
            "border: 2px solid transparent;"
            "border-radius: 12px;"
            "background-color: transparent;"
            "}"
            "QPushButton:hover {"
            "border: 2px solid rgba(255, 255, 255, 220);"
            "background-color: rgba(255, 255, 255, 24);"
            "}"
            "QPushButton:pressed {"
            "border: 2px solid rgba(255, 255, 255, 255);"
            "background-color: rgba(255, 255, 255, 45);"
            "}"
        ));
    };

    //难度选择按钮
    simpleBtn = new QPushButton(this);
    setupOptionButton(simpleBtn,":/images/di-simple.png",200);
    connect(simpleBtn,&QPushButton::clicked,this,&DifficultyWindow::onSimpleClicked);

    difficultBtn = new QPushButton(this);
    setupOptionButton(difficultBtn,":/images/di-hard.jpg",300);
    connect(difficultBtn,&QPushButton::clicked,this,&DifficultyWindow::onDifficultClicked);

    //返回按钮
    backBtn = new QPushButton(this);
    setupOptionButton(backBtn,":/images/di-return.png",400);
    connect(backBtn,&QPushButton::clicked,this,&DifficultyWindow::onBackClicked);
}
DifficultyWindow::~DifficultyWindow(){}

void DifficultyWindow::onSimpleClicked()
{
    GameWindow *game = new GameWindow(false);
    game->show();
    this->close();
}
void DifficultyWindow::onDifficultClicked()
{
    GameWindow *game = new GameWindow(true);
    game->show();
    this->close();
}
void DifficultyWindow::onBackClicked()
{
    this->close();
    MainWindow *mainWin = new MainWindow;
    mainWin->show();
}
