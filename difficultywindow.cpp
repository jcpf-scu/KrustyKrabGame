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
    //难度选择按钮
    simpleBtn = new QPushButton(this);
    simpleBtn->setIcon(QIcon(":/images/di-simple.png"));
    simpleBtn->setIconSize(QSize(240,70));
    simpleBtn->setFixedSize(240,70);
    simpleBtn->setGeometry(550,200,240,70);
    simpleBtn->setStyleSheet("border:none; background: transparent;");
    connect(simpleBtn,&QPushButton::clicked,this,&DifficultyWindow::onSimpleClicked);

    difficultBtn = new QPushButton(this);
    difficultBtn->setIcon(QIcon(":/images/di-hard.png"));
    difficultBtn->setIconSize(QSize(240,70));
    difficultBtn->setFixedSize(240,70);
    difficultBtn->setGeometry(550,300,240,70);
    difficultBtn->setStyleSheet("border:none; background: transparent;");
    connect(difficultBtn,&QPushButton::clicked,this,&DifficultyWindow::onDifficultClicked);

    //返回按钮
    backBtn = new QPushButton(this);
    backBtn->setIcon(QIcon(":/images/di-return.png"));
    backBtn->setIconSize(QSize(240,70));
    backBtn->setFixedSize(240,70);
    backBtn->setGeometry(550,400,220,70);
    backBtn->setStyleSheet("border:none; background: transparent;");
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
