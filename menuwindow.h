/**
 * @file menuwindow.h
 * @brief 游戏暂停菜单：继续 / 重新开始 / 返回大厅
 */

#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QMainWindow>
#include <QPushButton>

/**
 * @class MenuWindow
 * @brief 游戏中按「菜单」弹出的暂停窗口
 *
 * 通过信号通知 GameWindow 执行对应操作
 */
class MenuWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

signals:
    void continueGame();   // 继续当前游戏
    void restartGame();    // 重新开始本局
    void backToLobby();    // 返回主菜单

private slots:
    void onContinueClicked();
    void onRestartClicked();
    void onBackClicked();
};

#endif
