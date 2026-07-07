/**
 * @file mainwindow.h
 * @brief 主菜单窗口：开始游戏、查看说明、退出
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

/**
 * @class MainWindow
 * @brief 游戏开始界面（大厅）
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartClicked();   // 开始游戏 → 进入 GameWindow
    void onIntroClicked();   // 打开游戏说明窗口
    void onExitClicked();    // 退出程序

private:
    QPushButton *startBtn;   // 开始游戏按钮
    QPushButton *introBtn;   // 游戏说明按钮
    QPushButton *exitBtn;    // 退出游戏按钮
};

#endif
