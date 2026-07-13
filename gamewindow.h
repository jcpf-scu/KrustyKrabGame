/**
 * @brief 游戏主窗口：汉堡制作、计时、计分（核心玩法）
 */

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QStringList>
#include <QVector>
#include <QDialog>

/**
 * @class GameWindow
 * @brief 蟹黄堡制作游戏主界面
 *
 * 玩法流程：取料 → 煎制生肉饼 → 按订单组合 → 提交得分
 */
enum class IngredientState {  //原材料类
    Raw,
    Cooking,
    Cooked,
    Burnt
};
struct IngredientItem {
    QString name;          // 底面包 / 肉饼 / 薯条 / 可乐等等
    IngredientState state; // 当前状态
    int cookSeconds;       // 煎了几秒（针对肉饼/薯条）
    int cookedSeconds;     // 煎好后放了几秒
};
class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(bool advancedMode = false , QWidget *parent = nullptr);
    ~GameWindow();

private slots:
    // ---- 食材按钮 ----
    void onBottomBunClicked();  // 添加底面包
    void onRawPattyClicked();   // 添加生肉饼
    void onTopBunClicked();     // 添加顶面包
    void onLettuceClicked();    // 添加生菜
    void onTomatoClicked();    // 添加番茄
    void onFriesClicked();     // 薯条
    void onColaClicked();       // 可乐
    void updateIngredientTimers();  // 每秒更新煎制状态

    // ---- 操作按钮 ----
    void onGrillClicked();      // 煎制：生肉饼 → 肉饼
    void onFryClicked();        // 炸薯条
    void onDiscardClicked();    // 丢弃：清空盘子
    void onSubmitClicked();     // 提交订单并判分（简单模式）
    void onSubmitBurgerClicked(); // 提交汉堡订单（困难模式）
    void onSubmitSideClicked();   // 提交小吃订单（困难模式）
    void onMenuClicked();       // 打开暂停菜单
    void onBuyExtraTimeClicked(); // 购买额外时间技能
    void onOpenShopClicked();   // 打开商店界面

    // ---- 计时器 ----
    void updateTimer();         // 每秒更新倒计时

    // ---- 暂停菜单回调 ----
    void onContinueGame();      // 继续游戏
    void onRestartGame();       // 重新开始本局
    void onBackToLobby();       // 返回主菜单

private:
    void updatePlateDisplay();              // 刷新盘子上的食材显示
    void generateNewOrder();                // 生成新订单（当前为固定蟹黄堡）
    void endGame();                         // 时间到，结算并返回大厅
    void showStarRating();                  // 弹出星级评价对话框
    void showFeedback(const QString &message, const QString &color);  // 顶部反馈提示
    void loadPersistedCoins();              // 读取本地金币余额
    void saveCoins() const;                 // 保存金币余额
    void loadShopState();                   // 读取商店解锁状态
    void saveShopState() const;             // 保存商店解锁状态
    void updateCoinDisplay();               // 刷新金币显示
    void applyTheme();                      // 应用当前皮肤主题

    // ---- 游戏状态数据 ----
    bool isPaused;              // 是否暂停
    int comboCount;             // 当前连击数
    int maxScore;               // 历史最高分
    int orderSerial;            // 当前订单序号
    int score;                  // 本局得分
    int completedOrders;        // 本局完成订单数
    int timeLeft;               // 剩余秒数
    int totalCoins;             // 当前累计金币余额
    int cookingSpeedRemainingSeconds; // 加速煎制剩余生效秒数
    int orderBonusRemainingSeconds;   // 订单奖励提升剩余生效秒数
    int pattyCookingSpeedRemainingSeconds; // 肉饼加速剩余秒数
    int friesCookingSpeedRemainingSeconds; // 薯条加速剩余秒数
    int currentSkinId;          // 当前使用的皮肤编号：0=默认，1=海绵宝宝，2=派大星

    QVector<IngredientItem> currentIngredients;  // 盘子里的食材（简单模式）
    QVector<IngredientItem> burgerIngredients;   // 汉堡盘（困难模式）
    QVector<IngredientItem> sideIngredients;     // 小吃盘（困难模式）
    QStringList currentOrder;        // 当前订单要求的食材（简单模式）
    QStringList currentBurgerOrder;  // 当前汉堡订单（困难模式）
    QStringList currentSideOrder;    // 当前小吃订单（困难模式）

    // ---- UI 控件 ----
    QPushButton *bottomBunBtn;
    QPushButton *rawPattyBtn;
    QPushButton *topBunBtn;
    QPushButton *lettuceBtn;
    QPushButton *tomatoBtn;
    QPushButton *friesBtn;
    QPushButton *colaBtn;
    QPushButton *grillBtn;
    QPushButton *fryBtn;
    QPushButton *discardBtn;
    QPushButton *submitBtn;
    QPushButton *submitBurgerBtn;
    QPushButton *submitSideBtn;
    QPushButton *menuBtn;
    QPushButton *buyTimeBtn;
    QPushButton *shopBtn;

    QLabel *plateLabel;         // 盘子显示（简单模式）
    QLabel *burgerPlateLabel;   // 汉堡盘显示（困难模式）
    QLabel *sidePlateLabel;     // 小吃盘显示（困难模式）
    QLabel *timerLabel;         // 倒计时
    QLabel *scoreLabel;         // 本局得分
    QLabel *coinsLabel;         // 当前金币余额
    QLabel *serialLabel;        // 订单序号
    QLabel *orderLabel;         // 订单内容
    QLabel *feedbackLabel;      // 操作反馈
    QLabel *maxScoreLabel;      // 最高纪录

    QTimer *timer;              // 1 秒定时器

    bool isAdvancedMode;  //模块标记

};


#endif
