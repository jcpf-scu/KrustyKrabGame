/**
 * @brief 游戏主窗口：汉堡制作、计时、计分（核心玩法）
 */

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "difficultywindow.h"
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QStringList>
#include <QVector>

struct PlateItem {
    QString name;
    int grillElapsed;  // -1=无需煎制；0~6=煎制计时中；>=7=已煎糊

    QString displayName() const;
    QString orderName() const;  // 用于订单匹配，无效食材返回空串
};

/**
 * @class GameWindow
 * @brief 蟹黄堡制作游戏主界面
 *
 * 玩法流程：取料 → 煎制生肉饼 → 按订单组合 → 提交得分
 */
class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(GameDifficulty difficulty = GameDifficulty::Easy,
                        QWidget *parent = nullptr);
    ~GameWindow();

private slots:
    // ---- 食材按钮 ----
    void onBottomBunClicked();
    void onRawPattyClicked();
    void onTopBunClicked();
    void onLettuceClicked();
    void onTomatoClicked();
    void onRawFriesClicked();
    void onColaClicked();

    // ---- 操作按钮 ----
    void onGrillClicked();
    void onDiscardClicked();
    void onSubmitClicked();
    void onMenuClicked();

    // ---- 计时器 ----
    void updateTimer();
    void onGrillTimerTick();

    // ---- 暂停菜单回调 ----
    void onContinueGame();
    void onRestartGame();
    void onBackToLobby();

private:
    void updatePlateDisplay();
    void generateNewOrder();
    void endGame();
    void showStarRating();
    void showFeedback(const QString &message, const QString &color);
    int initialTimeForDifficulty() const;
    void addIngredient(const QString &name);
    bool startGrilling();
    bool hasInvalidIngredients() const;
    void resetGrillTimer();
    QStringList plateOrderNames() const;

    GameDifficulty difficulty;
    bool isPaused;
    int comboCount;
    int maxScore;
    int orderSerial;
    int score;
    int completedOrders;
    int timeLeft;

    QVector<PlateItem> currentIngredients;
    QStringList currentOrder;

    QPushButton *bottomBunBtn;
    QPushButton *rawPattyBtn;
    QPushButton *topBunBtn;
    QPushButton *lettuceBtn;
    QPushButton *tomatoBtn;
    QPushButton *rawFriesBtn;
    QPushButton *colaBtn;
    QWidget *hardIngredientRow;
    QPushButton *grillBtn;
    QPushButton *discardBtn;
    QPushButton *submitBtn;
    QPushButton *menuBtn;

    QLabel *plateLabel;
    QLabel *timerLabel;
    QLabel *scoreLabel;
    QLabel *serialLabel;
    QLabel *orderLabel;
    QLabel *feedbackLabel;
    QLabel *maxScoreLabel;

    QTimer *timer;
    QTimer *grillTimer;
    QTimer *feedbackTimer;
};

#endif
