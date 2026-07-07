/**
 * @file introwindow.h
 * @brief 游戏说明窗口
 */

#ifndef INTROWINDOW_H
#define INTROWINDOW_H

#include <QMainWindow>

/**
 * @class IntroWindow
 * @brief 展示玩法规则、得分说明的说明页
 */
class IntroWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit IntroWindow(QWidget *parent = nullptr);
    ~IntroWindow();

private slots:
    void onCloseClicked();  // 关闭说明窗口
};

#endif
