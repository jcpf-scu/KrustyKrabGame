/**
 * @brief 难度选择窗口：简单 / 困难
 */

#ifndef DIFFICULTYWINDOW_H
#define DIFFICULTYWINDOW_H

#include <QMainWindow>

enum class GameDifficulty {
    Easy,
    Hard
};

/**
 * @class DifficultyWindow
 * @brief 选择游戏难度后进入 GameWindow
 */
class DifficultyWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DifficultyWindow(QWidget *parent = nullptr);
    ~DifficultyWindow();

private slots:
    void onEasyClicked();
    void onHardClicked();
    void onBackClicked();
};

#endif
