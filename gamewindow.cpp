/**
 * @file gamewindow.cpp
 * @brief 游戏主窗口实现：简单模式（60 秒计时 + 固定蟹黄堡订单）
 */

#include "gamewindow.h"
#include "mainwindow.h"
#include "menuwindow.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QFile>
#include <QDataStream>
#include <QTime>
#include <QPixmap>
#include <QCoreApplication>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <functional>

// 食材按钮统一样式
static const char *BTN_STYLE =
    "QPushButton { background-color: #FFD166; color: #1D3557;"
    "font-size: 20px; font-weight: bold; border-radius: 8px;"
    "font-family: 'Microsoft YaHei', sans-serif; padding: 10px; }"
    "QPushButton:hover { background-color: #FFC145; }";

// 主要操作按钮样式（提交、菜单）
static const char *ACTION_STYLE =
    "QPushButton { background-color: #E63946; color: white;"
    "font-size: 20px; font-weight: bold; border-radius: 8px;"
    "font-family: 'Microsoft YaHei', sans-serif; padding: 10px; }"
    "QPushButton:hover { background-color: #C1121F; }";

// 背景图上的可读文字样式（半透明深色底 + 高对比文字）
static const char *INFO_PANEL_STYLE =
    "font-size: 25px; font-weight: bold; color: #FFFFFF;"
    "background-color: rgba(29, 53, 87, 220);"
    "border-radius: 10px; padding: 8px 16px;";

static const char *ACCENT_PANEL_STYLE =
    "font-size: 25px; font-weight: bold; color: #FFDD00;"
    "background-color: rgba(29, 53, 87, 220);"
    "border-radius: 10px; padding: 8px 16px;";

static const char *TIMER_STYLE =
    "font-size: 35px; font-weight: bold; color: #FFDD00;"
    "background-color: rgba(29, 53, 87, 220);"
    "border-radius: 10px; padding: 8px 16px;"
    "font-family: 'Courier New', monospace;";

static const char *SECTION_TITLE_STYLE =
    "font-size: 20px; font-weight: bold; color: #FFFFFF;"
    "background-color: rgba(29, 53, 87, 210);"
    "border-radius: 8px; padding: 6px 14px;";

static const char *SCORE_PANEL_STYLE =
    "font-size: 28px; font-weight: bold; color: #FFDD00;"
    "background-color: rgba(29, 53, 87, 220);"
    "border-radius: 10px; padding: 8px 16px;";

static const char *MAX_SCORE_PANEL_STYLE =
    "font-size: 22px; font-weight: bold; color: #FFFFFF;"
    "background-color: rgba(29, 53, 87, 210);"
    "border-radius: 10px; padding: 8px 16px;";

namespace {
QString formatTimerText(int seconds)
{
    int minutes = seconds / 60;
    int secondsPart = seconds % 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(secondsPart, 2, 10, QChar('0'));
}
}

void GameWindow::loadPersistedCoins()
{
    QFile file(QCoreApplication::applicationDirPath() + "/coins.dat");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> totalCoins;
        file.close();
    } else {
        totalCoins = 0;
    }
}

void GameWindow::saveCoins() const
{
    QFile file(QCoreApplication::applicationDirPath() + "/coins.dat");
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << totalCoins;
        file.close();
    }
}

void GameWindow::loadShopState()
{
    cookingSpeedRemainingSeconds = 0;
    pattyCookingSpeedRemainingSeconds = 0;
    friesCookingSpeedRemainingSeconds = 0;
    orderBonusRemainingSeconds = 0;
    currentSkinId = 0;

    QFile file(
        QCoreApplication::applicationDirPath()
        + "/shop_state.dat"
    );

    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);

        // 直接读取实际剩余秒数
        in >> cookingSpeedRemainingSeconds
           >> orderBonusRemainingSeconds
           >> currentSkinId;

        file.close();

        // 文件损坏时恢复默认值
        if (in.status() != QDataStream::Ok) {
            cookingSpeedRemainingSeconds = 0;
            orderBonusRemainingSeconds = 0;
            currentSkinId = 0;
        }
    }

    cookingSpeedRemainingSeconds =
        qMax(0, cookingSpeedRemainingSeconds);

    orderBonusRemainingSeconds =
        qMax(0, orderBonusRemainingSeconds);

    // 困难模式肉饼和薯条使用相同的加速剩余时间
    pattyCookingSpeedRemainingSeconds =
        cookingSpeedRemainingSeconds;

    friesCookingSpeedRemainingSeconds =
        cookingSpeedRemainingSeconds;
}

void GameWindow::saveShopState() const
{
    QFile file(
        QCoreApplication::applicationDirPath()
        + "/shop_state.dat"
    );

    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);

        // 保存实际剩余秒数，而不是只保存0或1
        out << cookingSpeedRemainingSeconds
            << orderBonusRemainingSeconds
            << currentSkinId;

        file.close();
    }
}

void GameWindow::updateCoinDisplay()
{
    if (coinsLabel) {
        coinsLabel->setText("当前金币：" + QString::number(totalCoins));
    }
    if (buyTimeBtn) {
        buyTimeBtn->setEnabled(totalCoins >= 15);
    }
    if (shopBtn) {
        shopBtn->setText("商店(" + QString::number(totalCoins) + ")");
    }
}

void GameWindow::applyTheme()
{
    if (!menuBtn || !buyTimeBtn || !shopBtn || !scoreLabel || !coinsLabel) {
        return;
    }

    QString accentColor = "#FFDD00";
    QString panelColor = "#1D3557";
    QString titleText = "蟹堡王 - 制作蟹黄堡";
    if (currentSkinId == 1) {
        accentColor = "#FF6B6B";
        panelColor = "#1E5F74";
        titleText = "蟹堡王 - 海绵宝宝主题";
    } else if (currentSkinId == 2) {
        accentColor = "#A3E635";
        panelColor = "#7B2CBF";
        titleText = "蟹堡王 - 派大星主题";
    }

    setWindowTitle(titleText);
    menuBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; font-size: 20px; font-weight: bold; border-radius: 8px; padding: 10px; } QPushButton:hover { background-color: %2; }").arg(accentColor).arg(panelColor));
    QString actionButtonStyle =
            QString(
            "QPushButton { background-color: %1; color: white;"
            "font-size: 20px; font-weight: bold;"
            "border-radius: 8px; padding: 10px; }"
            "QPushButton:hover { background-color: %2; }"
        ).arg(accentColor).arg(panelColor);

    menuBtn->setStyleSheet(actionButtonStyle);

    if (submitBtn) {
        submitBtn->setStyleSheet(actionButtonStyle);
    }

    if (submitBurgerBtn) {
        submitBurgerBtn->setStyleSheet(actionButtonStyle);
    }

    if (submitSideBtn) {
        submitSideBtn->setStyleSheet(actionButtonStyle);
    }
    buyTimeBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; font-size: 18px; font-weight: bold; border-radius: 8px; padding: 10px; } QPushButton:hover { background-color: %2; }").arg(accentColor).arg(panelColor));
    shopBtn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; font-size: 18px; font-weight: bold; border-radius: 8px; padding: 10px; } QPushButton:hover { background-color: %2; }").arg(accentColor).arg(panelColor));
    scoreLabel->setStyleSheet(QString("font-size: 28px; font-weight: bold; color: %1; background-color: rgba(29, 53, 87, 220); border-radius: 10px; padding: 8px 16px;").arg(accentColor));
    coinsLabel->setStyleSheet(QString("font-size: 28px; font-weight: bold; color: %1; background-color: rgba(29, 53, 87, 220); border-radius: 10px; padding: 8px 16px;").arg(accentColor));
}

GameWindow::GameWindow(bool advancedMode,QWidget *parent)
    : QMainWindow(parent)
    , isPaused(false)
    , comboCount(0)
    , maxScore(0)
    , orderSerial(1)
    , score(0)
    , completedOrders(0)
    , timeLeft(60)   // 简单模式：60 秒倒计时
    , totalCoins(0)
    , cookingSpeedRemainingSeconds(0)
    , orderBonusRemainingSeconds(0)
    , pattyCookingSpeedRemainingSeconds(0)
    , friesCookingSpeedRemainingSeconds(0)
    , currentSkinId(0)
    , timer(nullptr)
    , isAdvancedMode(advancedMode)
{
    setFixedSize(1125, 812);
    setWindowTitle("蟹堡王 - 制作蟹黄堡");
    friesBtn = nullptr;
    colaBtn = nullptr;
    fryBtn = nullptr;
    submitBtn = nullptr;
    submitBurgerBtn = nullptr;
    submitSideBtn = nullptr;
    buyTimeBtn = nullptr;
    shopBtn = nullptr;
    coinsLabel = nullptr;
    plateLabel = nullptr;
    burgerPlateLabel = nullptr;
    sidePlateLabel = nullptr;
    timer = new QTimer(this);

    // ========= 背景层 =========
    QLabel *bgLabel = new QLabel(this);
    bgLabel->setGeometry(0, 0, 1125, 812);
    bgLabel->setScaledContents(true);
    const QString bgResourcePath = isAdvancedMode
        ? ":/images/game_hard_bg.jpg"
        : ":/images/game_bg.jpg";
    QPixmap bgPixmap(bgResourcePath);
    if (bgPixmap.isNull()) {
        const QString bgFilePath = isAdvancedMode
            ? "images/game_hard_bg.jpg"
            : "images/game_bg.jpg";
        bgPixmap.load(bgFilePath);
    }
    if (!bgPixmap.isNull()) {
        bgLabel->setPixmap(bgPixmap.scaled(bgLabel->size(),
                                           Qt::KeepAspectRatioByExpanding,
                                           Qt::SmoothTransformation));
    } else {
        bgLabel->setStyleSheet("background-color: #F1FAEE;");
    }

    // 读取本地保存的最高分
    QFile file(
        QCoreApplication::applicationDirPath()
        + "/maxscore.dat"
    );
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> maxScore;
        file.close();
    }
    loadPersistedCoins();
    loadShopState();

    // ========= 搭建界面布局 =========
    QWidget *central = new QWidget(this);
    central->setStyleSheet("background: transparent;");
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(25, 19, 25, 19);

    // ---- 顶部信息栏：序号 | 订单 | 倒计时 | 菜单 ----
    QHBoxLayout *topBar = new QHBoxLayout();
    serialLabel = new QLabel("第 1 单", this);
    serialLabel->setStyleSheet(ACCENT_PANEL_STYLE);

    orderLabel = new QLabel(this);
    orderLabel->setStyleSheet(INFO_PANEL_STYLE);
    orderLabel->setAlignment(Qt::AlignCenter);

    timerLabel = new QLabel("01:00", this);
    timerLabel->setStyleSheet(TIMER_STYLE);
    timerLabel->setAlignment(Qt::AlignRight);

    menuBtn = new QPushButton("菜单", this);
    menuBtn->setFixedSize(75, 45);
    menuBtn->setStyleSheet(ACTION_STYLE);

    topBar->addWidget(serialLabel);
    topBar->addWidget(orderLabel, 1);
    topBar->addWidget(timerLabel);
    topBar->addWidget(menuBtn);
    mainLayout->addLayout(topBar);

    // 操作反馈提示（成功/失败/连击等，有内容时才显示）
    feedbackLabel = new QLabel(this);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    feedbackLabel->setFixedHeight(38);
    feedbackLabel->hide();
    mainLayout->addWidget(feedbackLabel);

    // ---- 食材按钮区 ----
    QLabel *ingredientTitle = new QLabel("食材区", this);
    ingredientTitle->setStyleSheet(SECTION_TITLE_STYLE);
    mainLayout->addWidget(ingredientTitle);

    bottomBunBtn = new QPushButton("底面包", this);
    rawPattyBtn = new QPushButton("生肉饼", this);
    topBunBtn = new QPushButton("顶面包", this);
    lettuceBtn = new QPushButton("生菜",this);
    tomatoBtn= new QPushButton("番茄",this);

    if (isAdvancedMode) {
        QVBoxLayout *ingredientSectionLayout = new QVBoxLayout();
        QLabel *burgerTitle = new QLabel("汉堡区", this);
        burgerTitle->setStyleSheet(SECTION_TITLE_STYLE);
        ingredientSectionLayout->addWidget(burgerTitle);
        QHBoxLayout *burgerRow = new QHBoxLayout();
        for (QPushButton *btn : {bottomBunBtn, rawPattyBtn, topBunBtn, lettuceBtn, tomatoBtn}) {
            btn->setFixedHeight(63);
            btn->setStyleSheet(BTN_STYLE);
            burgerRow->addWidget(btn);
        }
        ingredientSectionLayout->addLayout(burgerRow);

        QLabel *sideTitle = new QLabel("小吃区", this);
        sideTitle->setStyleSheet(SECTION_TITLE_STYLE);
        ingredientSectionLayout->addWidget(sideTitle);
        QHBoxLayout *sideRow = new QHBoxLayout();
        friesBtn = new QPushButton("薯条", this);
        colaBtn = new QPushButton("可乐", this);
        for (QPushButton *btn : {friesBtn, colaBtn}) {
            btn->setFixedHeight(63);
            btn->setStyleSheet(BTN_STYLE);
            sideRow->addWidget(btn);
        }
        ingredientSectionLayout->addLayout(sideRow);
        mainLayout->addLayout(ingredientSectionLayout);
    } else {
        QHBoxLayout *ingredientRow = new QHBoxLayout();
        for (QPushButton *btn : {bottomBunBtn, rawPattyBtn, lettuceBtn, tomatoBtn, topBunBtn}) {
            btn->setFixedHeight(63);
            btn->setStyleSheet(BTN_STYLE);
            ingredientRow->addWidget(btn);
        }
        mainLayout->addLayout(ingredientRow);
    }

    mainLayout->addSpacing(55);

    // ---- 盘子显示区 ----
    if (isAdvancedMode) {
        burgerPlateLabel = new QLabel("汉堡盘：空", this);
        burgerPlateLabel->setAlignment(Qt::AlignCenter);
        burgerPlateLabel->setMinimumHeight(110);
        burgerPlateLabel->setStyleSheet(
            "font-size: 24px; font-weight: bold; color: #1D3557;"
            "background-color: rgba(255, 255, 255, 235); border: 3px dashed #A8DADC;"
            "border-radius: 15px; padding: 18px;"
        );
        sidePlateLabel = new QLabel("小吃盘：空", this);
        sidePlateLabel->setAlignment(Qt::AlignCenter);
        sidePlateLabel->setMinimumHeight(110);
        sidePlateLabel->setStyleSheet(
            "font-size: 24px; font-weight: bold; color: #1D3557;"
            "background-color: rgba(255, 255, 255, 235); border: 3px dashed #A8DADC;"
            "border-radius: 15px; padding: 18px;"
        );
        QVBoxLayout *plateLayout = new QVBoxLayout();
        plateLayout->addWidget(burgerPlateLabel);
        plateLayout->addSpacing(10);
        plateLayout->addWidget(sidePlateLabel);
        mainLayout->addLayout(plateLayout);
    } else {
        plateLabel = new QLabel("盘子：空", this);
        plateLabel->setAlignment(Qt::AlignCenter);
        plateLabel->setMinimumHeight(150);
        plateLabel->setStyleSheet(
            "font-size: 28px; font-weight: bold; color: #1D3557;"
            "background-color: rgba(255, 255, 255, 235); border: 3px dashed #A8DADC;"
            "border-radius: 15px; padding: 25px;"
        );
        mainLayout->addWidget(plateLabel);
    }


    // ---- 操作按钮区 ----
    QLabel *actionTitle = new QLabel("操作区", this);
    actionTitle->setStyleSheet(SECTION_TITLE_STYLE);
    mainLayout->addWidget(actionTitle);

    QHBoxLayout *actionRow = new QHBoxLayout();

    grillBtn = new QPushButton(isAdvancedMode ? "煎肉饼" : "煎制", this);
    discardBtn = new QPushButton("丢弃", this);

    // 可选按钮先初始化为空
    fryBtn = nullptr;
    submitBtn = nullptr;
    submitBurgerBtn = nullptr;
    submitSideBtn = nullptr;

    grillBtn->setFixedHeight(63);
    discardBtn->setFixedHeight(63);

    grillBtn->setStyleSheet(BTN_STYLE);
    discardBtn->setStyleSheet(
        "QPushButton { background-color: #6C757D; color: white;"
        "font-size: 20px; font-weight: bold; border-radius: 8px; padding: 10px; }"
        "QPushButton:hover { background-color: #495057; }"
    );

    actionRow->addWidget(grillBtn);

    if (isAdvancedMode) {
        // 困难模式按钮
        fryBtn = new QPushButton("炸薯条", this);
        submitBurgerBtn = new QPushButton("提交汉堡", this);
        submitSideBtn = new QPushButton("提交小吃", this);

        fryBtn->setFixedHeight(63);
        submitBurgerBtn->setFixedHeight(63);
        submitSideBtn->setFixedHeight(63);

        fryBtn->setStyleSheet(BTN_STYLE);
        submitBurgerBtn->setStyleSheet(ACTION_STYLE);
        submitSideBtn->setStyleSheet(ACTION_STYLE);

        actionRow->addWidget(fryBtn);
        actionRow->addWidget(submitBurgerBtn);
        actionRow->addWidget(submitSideBtn);
        actionRow->addWidget(discardBtn);
    } else {
        // 简单模式才创建“提交订单”
        submitBtn = new QPushButton("提交订单", this);
        submitBtn->setFixedHeight(63);
        submitBtn->setStyleSheet(ACTION_STYLE);

        actionRow->addWidget(discardBtn);
        actionRow->addWidget(submitBtn, 1);
    }

    mainLayout->addLayout(actionRow);

    mainLayout->addStretch();

    // ---- 底部分数栏 ----
    QHBoxLayout *bottomBar = new QHBoxLayout();
    scoreLabel = new QLabel("本局得分：0", this);
    scoreLabel->setStyleSheet(SCORE_PANEL_STYLE);
    coinsLabel = new QLabel("当前金币：0", this);
    coinsLabel->setStyleSheet(SCORE_PANEL_STYLE);
    buyTimeBtn = new QPushButton("购买+10秒(15金币)", this);
    buyTimeBtn->setFixedHeight(50);
    buyTimeBtn->setStyleSheet(BTN_STYLE);
    shopBtn = new QPushButton("商店(0)", this);
    shopBtn->setFixedHeight(50);
    shopBtn->setStyleSheet(BTN_STYLE);
    maxScoreLabel = new QLabel("最高纪录：" + QString::number(maxScore), this);
    maxScoreLabel->setStyleSheet(MAX_SCORE_PANEL_STYLE);
    maxScoreLabel->setAlignment(Qt::AlignCenter);
    bottomBar->addWidget(scoreLabel);
    bottomBar->addWidget(coinsLabel);
    bottomBar->addWidget(buyTimeBtn);
    bottomBar->addWidget(shopBtn);
    bottomBar->addWidget(maxScoreLabel, 1);
    mainLayout->addLayout(bottomBar);

    // ========= 信号槽连接 =========
    connect(bottomBunBtn, &QPushButton::clicked, this, &GameWindow::onBottomBunClicked);
    connect(rawPattyBtn,  &QPushButton::clicked, this, &GameWindow::onRawPattyClicked);
    connect(topBunBtn,    &QPushButton::clicked, this, &GameWindow::onTopBunClicked);
    connect(tomatoBtn,    &QPushButton::clicked, this, &GameWindow::onTomatoClicked);
    connect(lettuceBtn,    &QPushButton::clicked, this, &GameWindow::onLettuceClicked);
    if (isAdvancedMode) {
        connect(colaBtn,  &QPushButton::clicked, this, &GameWindow::onColaClicked);
        connect(friesBtn, &QPushButton::clicked, this, &GameWindow::onFriesClicked);
        connect(fryBtn,   &QPushButton::clicked, this, &GameWindow::onFryClicked);
    }
    connect(grillBtn,     &QPushButton::clicked, this, &GameWindow::onGrillClicked);
    connect(discardBtn,   &QPushButton::clicked, this, &GameWindow::onDiscardClicked);
    if (isAdvancedMode) {
        connect(submitBurgerBtn, &QPushButton::clicked, this, &GameWindow::onSubmitBurgerClicked);
        connect(submitSideBtn,   &QPushButton::clicked, this, &GameWindow::onSubmitSideClicked);
    } else {
        connect(submitBtn,    &QPushButton::clicked, this, &GameWindow::onSubmitClicked);
    }
    connect(menuBtn,      &QPushButton::clicked, this, &GameWindow::onMenuClicked);
    connect(buyTimeBtn,   &QPushButton::clicked, this, &GameWindow::onBuyExtraTimeClicked);
    connect(shopBtn,      &QPushButton::clicked, this, &GameWindow::onOpenShopClicked);
    connect(timer,        &QTimer::timeout, this, &GameWindow::updateTimer);


    bgLabel->lower();

    // 生成第一单订单，启动倒计时
    updateCoinDisplay();
    applyTheme();
    generateNewOrder();
    connect(timer,        &QTimer::timeout, this, &GameWindow::updateIngredientTimers);
    timer->start(1000);  // 每 1000 毫秒（1 秒）触发一次
}

GameWindow::~GameWindow() {}
//食物的制作
    void GameWindow::onColaClicked()
    {
        IngredientItem item;
        item.name = "可乐";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        if (isAdvancedMode) {
            sideIngredients.append(item);
        } else {
            currentIngredients.append(item);
        }
        updatePlateDisplay();
    }
    void GameWindow::onFriesClicked()
    {
        IngredientItem item;
        item.name = "薯条";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        if (isAdvancedMode) {
            sideIngredients.append(item);
        } else {
            currentIngredients.append(item);
        }
        updatePlateDisplay();
    }

    void GameWindow::onBottomBunClicked()
    {
        IngredientItem item;
        item.name = "底面包";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        if (isAdvancedMode) {
            burgerIngredients.append(item);
        } else {
            currentIngredients.append(item);
        }
        updatePlateDisplay();
    }

    void GameWindow::onRawPattyClicked()
    {
        IngredientItem item;
        item.name = "肉饼";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        if (isAdvancedMode) {
            burgerIngredients.append(item);
        } else {
            currentIngredients.append(item);
        }
        updatePlateDisplay();
    }

    void GameWindow::onTopBunClicked()
    {
        IngredientItem item;
        item.name = "顶面包";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        if (isAdvancedMode) {
            burgerIngredients.append(item);
        } else {
            currentIngredients.append(item);
        }
        updatePlateDisplay();
    }

    void GameWindow::onLettuceClicked()
    {
        IngredientItem item;
        item.name = "生菜";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        if (isAdvancedMode) {
            burgerIngredients.append(item);
        } else {
            currentIngredients.append(item);
        }
        updatePlateDisplay();
    }

    void GameWindow::onTomatoClicked()
    {
        IngredientItem item;
        item.name = "番茄";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        if (isAdvancedMode) {
            burgerIngredients.append(item);
        } else {
            currentIngredients.append(item);
        }
        updatePlateDisplay();
    }

// 煎制
void GameWindow::onBuyExtraTimeClicked()
{
    if (totalCoins < 15) {
        showFeedback("金币不足，无法购买额外 10 秒", "#E63946");
        return;
    }

    totalCoins -= 15;
    timeLeft += 10;
    saveCoins();
    updateCoinDisplay();
    timerLabel->setText(formatTimerText(timeLeft));
    showFeedback("购买成功，额外获得 10 秒时间", "#2A9D8F");
}

void GameWindow::onOpenShopClicked()
{
    QDialog shopDialog(this);
    shopDialog.setWindowTitle("商店");
    shopDialog.setModal(true);
    shopDialog.setFixedSize(760, 620);

    QLabel *shopBgLabel = new QLabel(&shopDialog);
    shopBgLabel->setGeometry(shopDialog.rect());
    shopBgLabel->setScaledContents(true);

    QPixmap shopBgPixmap(":/images/shop_bg.jpg");

    if (shopBgPixmap.isNull()) {
        shopBgPixmap.load("images/shop_bg.jpg");
    }

    if (!shopBgPixmap.isNull()) {
        shopBgLabel->setPixmap(
            shopBgPixmap.scaled(
                shopBgLabel->size(),
                Qt::KeepAspectRatioByExpanding,
                Qt::SmoothTransformation
            )
        );
    } else {
        shopBgLabel->setStyleSheet(
            "background-color: #F1FAEE;"
        );
    }

    shopBgLabel->lower();

    QVBoxLayout *layout =
        new QVBoxLayout(&shopDialog);

    layout->setContentsMargins(25, 25, 25, 25);
    layout->setSpacing(12);
    QLabel *titleLabel = new QLabel(QString("当前金币：%1").arg(totalCoins), &shopDialog);
    titleLabel->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #1D3557;"
        "background-color: rgba(255, 255, 255, 210);"
        "border-radius: 8px;"
        "padding: 10px;"
    );
    layout->addWidget(titleLabel);

    QLabel *hintLabel = new QLabel("购买后效果会永久生效，直到你重新开始游戏。", &shopDialog);
    hintLabel->setWordWrap(true);
    hintLabel->setStyleSheet(
        "font-size: 18px;"
        "color: #4D4D4D;"
        "background-color: rgba(255, 255, 255, 195);"
        "border-radius: 8px;"
        "padding: 8px;"
    );
    layout->addWidget(hintLabel);

    auto addShopRow = [&](const QString &name, const QString &description, int cost, bool unlocked, const QString &actionText, std::function<void()> action) {
        QHBoxLayout *row = new QHBoxLayout();
        row->setSpacing(18);
        QLabel *infoLabel = new QLabel(name + "\n" + description, &shopDialog);
        infoLabel->setWordWrap(true);
        infoLabel->setStyleSheet(
            "font-size: 18px;"
            "color: #1D3557;"
            "background-color: rgba(255, 255, 255, 195);"
            "border-radius: 8px;"
            "padding: 8px;"
        );
        QPushButton *actionBtn = new QPushButton(actionText, &shopDialog);
        actionBtn->setEnabled(!unlocked && totalCoins >= cost);
        actionBtn->setFixedSize(190, 58);
        actionBtn->setStyleSheet(
            "QPushButton {"
            "font-size: 22px;"
            "font-weight: bold;"
            "color: #1D3557;"
            "background-color: rgba(255, 255, 255, 235);"
            "border: 2px solid #4F86C6;"
            "border-radius: 10px;"
            "padding: 6px;"
            "}"
            "QPushButton:hover {"
            "background-color: #DCEEFF;"
            "}"
            "QPushButton:disabled {"
            "color: #888888;"
            "background-color: rgba(230, 230, 230, 220);"
            "border-color: #AAAAAA;"
            "}"
        );
        connect(actionBtn, &QPushButton::clicked, this, [this, cost, action, &shopDialog]() {
            if (totalCoins < cost) {
                QMessageBox::warning(&shopDialog, "金币不足", "你的金币不足，无法购买该技能。", QMessageBox::Ok);
                return;
            }
            totalCoins -= cost;
            saveCoins();
            action();
            updateCoinDisplay();
            applyTheme();
            QMessageBox::information(&shopDialog, "购买成功", "购买成功，效果已生效。", QMessageBox::Ok);
            shopDialog.accept();
        });
        row->addWidget(infoLabel, 1);
        row->addWidget(actionBtn);
        layout->addLayout(row);
    };

    addShopRow(
        "加速煎制",
        "购买后 20 秒内让肉饼和薯条更快变熟。",
        25,
        cookingSpeedRemainingSeconds > 0,
        cookingSpeedRemainingSeconds > 0 ? "生效中" : "购买(25)",
        [this]() {
            cookingSpeedRemainingSeconds = 20;
            pattyCookingSpeedRemainingSeconds = 20;
            friesCookingSpeedRemainingSeconds = 20;
            saveShopState();
        }
    );

    addShopRow("订单奖励提升", "购买后 20 秒内每完成一个订单额外获得 5 金币。", 30, orderBonusRemainingSeconds > 0, orderBonusRemainingSeconds > 0 ? "生效中" : "购买(30)", [this]() {
        orderBonusRemainingSeconds = 20;
        saveShopState();
    });

    addShopRow("额外 10 秒", "立即为当前局增加 10 秒时间。", 15, false, "购买(15)", [this]() {
        timeLeft += 10;
        timerLabel->setText(formatTimerText(timeLeft));
        showFeedback("已获得额外 10 秒时间", "#2A9D8F");
    });

    addShopRow("海绵宝宝主题", "解锁一套更可爱的游戏主题。", 35, currentSkinId == 1, currentSkinId == 1 ? "已使用" : "购买(35)", [this]() {
        currentSkinId = 1;
        saveShopState();
        applyTheme();
    });

    addShopRow("派大星主题", "解锁一套更酷的游戏主题。", 35, currentSkinId == 2, currentSkinId == 2 ? "已使用" : "购买(35)", [this]() {
        currentSkinId = 2;
        saveShopState();
        applyTheme();
    });

    layout->addStretch();
    shopDialog.exec();
}

void GameWindow::onGrillClicked()
{
    QVector<IngredientItem> *target = isAdvancedMode ? &burgerIngredients : &currentIngredients;
    for (IngredientItem &item : *target) {
        if (item.name == "肉饼" && item.state == IngredientState::Raw) {
            item.state = IngredientState::Cooking;
            item.cookSeconds = 0;
            item.cookedSeconds = 0;
            showFeedback("肉饼开始煎制", "#2A9D8F");
            updatePlateDisplay();
            return;
        }
    }
    showFeedback("没有可煎制的肉饼", "#E63946");
}

void GameWindow::onFryClicked()
{
    QVector<IngredientItem> *target = isAdvancedMode ? &sideIngredients : &currentIngredients;
    for (IngredientItem &item : *target) {
        if (item.name == "薯条" && item.state == IngredientState::Raw) {
            item.state = IngredientState::Cooking;
            item.cookSeconds = 0;
            item.cookedSeconds = 0;
            showFeedback("薯条开始炸制", "#2A9D8F");
            updatePlateDisplay();
            return;
        }
    }
    showFeedback("没有可炸制的薯条", "#E63946");
}
//煎制的定时功能
void GameWindow::updateIngredientTimers()
{
    auto processCollection = [&](QVector<IngredientItem> &items, int &speedRemainingSeconds, bool isSide) {
        const int cookThreshold = speedRemainingSeconds > 0 ? 2 : 3;
        const int burnThreshold = speedRemainingSeconds > 0 ? 3 : 4;
        bool burnedInCollection = false;
        for (IngredientItem &item : items) {
            if (item.state == IngredientState::Cooking) {
                item.cookSeconds++;
                if (item.cookSeconds >= cookThreshold) {
                    item.state = IngredientState::Cooked;
                    item.cookedSeconds = 0;
                    if (item.name == "肉饼") {
                        item.name = "熟肉饼";
                    }
                }
            }
            else if (item.state == IngredientState::Cooked) {
                item.cookedSeconds++;
                if (item.cookedSeconds >= burnThreshold) {
                    item.state = IngredientState::Burnt;
                    if (item.name == "熟肉饼") {
                        item.name = "糊肉饼";
                    } else if (item.name == "薯条") {
                        item.name = "糊薯条";
                    }
                    burnedInCollection = true;
                }
            }
        }
        if (burnedInCollection) {
            items.clear();
            score -= 10;
            scoreLabel->setText("本局得分：" + QString::number(score));
            if (isSide) {
                showFeedback("小吃盘有食材糊掉了，已清空，扣除 10 金币", "#E63946");
            } else {
                showFeedback("汉堡盘有食材糊掉了，已清空，扣除 10 金币", "#E63946");
            }
        }
    };

    if (isAdvancedMode) {
        processCollection(burgerIngredients, pattyCookingSpeedRemainingSeconds, false);
        processCollection(sideIngredients, friesCookingSpeedRemainingSeconds, true);
    } else {
        const int cookThreshold = cookingSpeedRemainingSeconds > 0 ? 2 : 3;
        const int burnThreshold = cookingSpeedRemainingSeconds > 0 ? 3 : 4;
        bool burnedThisTick = false;
        for (IngredientItem &item : currentIngredients) {
            if (item.state == IngredientState::Cooking) {
                item.cookSeconds++;
                if (item.cookSeconds >= cookThreshold) {
                    item.state = IngredientState::Cooked;
                    item.cookedSeconds = 0;
                    if (item.name == "肉饼") {
                        item.name = "熟肉饼";
                    }
                }
            }
            else if (item.state == IngredientState::Cooked) {
                item.cookedSeconds++;
                if (item.cookedSeconds >= burnThreshold) {
                    item.state = IngredientState::Burnt;
                    if (item.name == "熟肉饼") {
                        item.name = "糊肉饼";
                    }
                    burnedThisTick = true;
                }
            }
        }
        if (burnedThisTick) {
            currentIngredients.clear();
            score -= 10;
            scoreLabel->setText("本局得分：" + QString::number(score));
            showFeedback("有食材糊掉了，盘子已清空，扣除 10 金币", "#E63946");
        }
    }

    updatePlateDisplay();
}
// 丢弃：清空盘子，订单不变，可重新制作
void GameWindow::onDiscardClicked()
{
    if (isAdvancedMode) {
        burgerIngredients.clear();
        sideIngredients.clear();
    } else {
        currentIngredients.clear();
    }
    updatePlateDisplay();
    showFeedback("已清空盘子", "#6C757D");
}

// ========== 提交订单与判分 ==========
void GameWindow::onSubmitClicked()
{
    // 检查盘子里的食材数量是否与订单一致
    bool match =
        (currentIngredients.size() == currentOrder.size());

    QStringList plateNames;

    for (const IngredientItem &item : currentIngredients) {
        plateNames << item.name;
    }

    // 检查每种食材是否都与订单一致
    for (const QString &need : currentOrder) {
        if (!match || !plateNames.removeOne(need)) {
            match = false;
            break;
        }
    }

    // 检查是否存在煎糊食材
    for (const IngredientItem &item : currentIngredients) {
        if (item.state == IngredientState::Burnt) {
            showFeedback(
                "有煎糊食材，不能提交，只能丢掉",
                "#E63946"
            );
            return;
        }
    }

    if (match) {
        // 基础订单奖励
        score += 10;

        // 商店额外订单奖励
        if (orderBonusRemainingSeconds > 0) {
            score += 5;
        }

        comboCount++;

        // 连击奖励
        if (comboCount >= 5) {
            score += 10;

            showFeedback(
                "五连击！额外 +10 金币！",
                "#E63946"
            );
        }
        else if (comboCount >= 3) {
            score += 5;

            showFeedback(
                "三连击！额外 +5 金币！",
                "#F4A261"
            );
        }
        else {
            if (orderBonusRemainingSeconds > 0) {
                showFeedback(
                    "美味！订单完成 +15 金币",
                    "#2A9D8F"
                );
            }
            else {
                showFeedback(
                    "美味！订单完成 +10 金币",
                    "#2A9D8F"
                );
            }
        }

        completedOrders++;

        currentIngredients.clear();
        updatePlateDisplay();

        // 更新订单序号后生成下一单
        orderSerial++;
        serialLabel->setText(
            QString("第 %1 单").arg(orderSerial)
        );

        generateNewOrder();

        scoreLabel->setText(
            "本局得分：" + QString::number(score)
        );
    }
    else {
        score -= 5;
        comboCount = 0;

        scoreLabel->setText(
            "本局得分：" + QString::number(score)
        );

        showFeedback(
            "订单错误！消耗原材料，扣除 5 金币，请重新制作",
            "#E63946"
        );

        currentIngredients.clear();
        updatePlateDisplay();
    }
}
void GameWindow::onSubmitBurgerClicked()
{
    bool match = (burgerIngredients.size() == currentBurgerOrder.size());

    QStringList plateNames;
    for (const IngredientItem &item : burgerIngredients) {
        plateNames << item.name;
    }

    for (const QString &need : currentBurgerOrder) {
        if (!match || !plateNames.removeOne(need)) {
            match = false;
            break;
        }
    }

    // 检查是否有煎糊的食材
    for (const IngredientItem &item : burgerIngredients) {
        if (item.state == IngredientState::Burnt) {
            showFeedback("汉堡盘里有煎糊食材，不能提交", "#E63946");
            return;
        }
    }

    if (!match) {
        score -= 5;
        comboCount = 0;

        burgerIngredients.clear();
        updatePlateDisplay();

        scoreLabel->setText(
            "本局得分：" + QString::number(score)
        );

        showFeedback("汉堡订单错误，请重新制作", "#E63946");
        return;
    }

    // 汉堡部分完成
    score += 10;
    burgerIngredients.clear();

    // 禁止重复提交汉堡
    submitBurgerBtn->setEnabled(false);

    updatePlateDisplay();
    scoreLabel->setText(
        "本局得分：" + QString::number(score)
    );

    // 汉堡和小吃都提交完成后，才生成下一张订单
    if (!submitBurgerBtn->isEnabled()
        && !submitSideBtn->isEnabled()) {

        // 有小吃时基础奖励为20，没有小吃时为10
        const int baseReward =
            currentSideOrder.isEmpty() ? 10 : 20;
        int extraReward = 0;

        // 商店订单奖励
        if (orderBonusRemainingSeconds > 0) {
            extraReward += 5;
        }

        comboCount++;

        // 连击奖励
        if (comboCount >= 5) {
            extraReward += 10;
        } else if (comboCount >= 3) {
            extraReward += 5;
        }

        score += extraReward;
        completedOrders++;
        orderSerial++;

        // 下一单重新允许提交
        submitBurgerBtn->setEnabled(true);
        submitSideBtn->setEnabled(true);

        generateNewOrder();

        serialLabel->setText(
            QString("第 %1 单").arg(orderSerial)
        );

        scoreLabel->setText(
            "本局得分：" + QString::number(score)
        );

        showFeedback(
            QString("整单完成，共获得 %1 金币")
                .arg(baseReward + extraReward),
            "#2A9D8F"
        );
    } else {
        showFeedback(
            "汉堡部分完成 +10 金币，请继续制作小吃",
            "#2A9D8F"
        );
    }
}


void GameWindow::onSubmitSideClicked()
{
    // 当前订单没有小吃时，不进行小吃提交
    if (currentSideOrder.isEmpty()) {
        showFeedback(
            "本单没有小吃，不需要提交",
            "#6C757D"
        );
        return;
    }
    bool match =
        (sideIngredients.size() == currentSideOrder.size());

    QStringList plateNames;

    for (const IngredientItem &item : sideIngredients) {
        plateNames << item.name;
    }

    for (const QString &need : currentSideOrder) {
        if (!match || !plateNames.removeOne(need)) {
            match = false;
            break;
        }
    }

    // 检查小吃盘是否有炸糊的食材
    for (const IngredientItem &item : sideIngredients) {
        if (item.state == IngredientState::Burnt) {
            showFeedback(
                "小吃盘里有炸糊食材，不能提交",
                "#E63946"
            );
            return;
        }
    }

    // 小吃订单错误
    if (!match) {
        score -= 5;
        comboCount = 0;

        sideIngredients.clear();
        updatePlateDisplay();

        scoreLabel->setText(
            "本局得分：" + QString::number(score)
        );

        showFeedback(
            "小吃订单错误，请重新制作",
            "#E63946"
        );

        return;
    }

    // 小吃部分提交成功
    score += 10;
    sideIngredients.clear();

    // 防止同一张订单重复提交小吃
    submitSideBtn->setEnabled(false);

    updatePlateDisplay();

    scoreLabel->setText(
        "本局得分：" + QString::number(score)
    );

    // 汉堡和小吃都完成后，才进入下一单
    if (!submitBurgerBtn->isEnabled()
        && !submitSideBtn->isEnabled()) {

        const int baseReward =
            currentSideOrder.isEmpty() ? 10 : 20;
        int extraReward = 0;

        // 商店订单额外奖励
        if (orderBonusRemainingSeconds > 0) {
            extraReward += 5;
        }

        comboCount++;

        // 连击奖励
        if (comboCount >= 5) {
            extraReward += 10;
        }
        else if (comboCount >= 3) {
            extraReward += 5;
        }

        score += extraReward;
        completedOrders++;
        orderSerial++;

        // 新订单重新启用两个提交按钮
        submitBurgerBtn->setEnabled(true);
        submitSideBtn->setEnabled(true);

        generateNewOrder();

        serialLabel->setText(
            QString("第 %1 单").arg(orderSerial)
        );

        scoreLabel->setText(
            "本局得分：" + QString::number(score)
        );

        showFeedback(
            QString("整单完成，共获得 %1 金币")
                .arg(baseReward + extraReward),
            "#2A9D8F"
        );
    }
    else {
        showFeedback(
            "小吃部分完成 +10 金币，请继续制作汉堡",
            "#2A9D8F"
        );
    }
}
// ========== 订单与显示 ==========
//订单的两种显示
void GameWindow::generateNewOrder()
{
    currentOrder.clear();
    currentBurgerOrder.clear();
    currentSideOrder.clear();
    if (isAdvancedMode) {
        // 三种汉堡订单
        QStringList burgerOrder0 = {
            "底面包", "熟肉饼", "顶面包"
        };

        QStringList burgerOrder1 = {
            "底面包", "生菜", "熟肉饼", "顶面包"
        };

        QStringList burgerOrder2 = {
            "底面包", "生菜", "熟肉饼", "番茄", "顶面包"
        };

        // 四种小吃订单
        QStringList sideOrder0;                    // 无
        QStringList sideOrder1 = {"薯条"};         // 只有薯条
        QStringList sideOrder2 = {"可乐"};         // 只有可乐
        QStringList sideOrder3 = {"薯条", "可乐"}; // 薯条和可乐

        static bool seeded = false;

        if (!seeded) {
            qsrand(QTime::currentTime().msec());
            seeded = true;
        }

        // 随机生成汉堡订单
        const int burgerRandom = qrand() % 3;

        if (burgerRandom == 0) {
            currentBurgerOrder = burgerOrder0;
        }
        else if (burgerRandom == 1) {
            currentBurgerOrder = burgerOrder1;
        }
        else {
            currentBurgerOrder = burgerOrder2;
        }

        // 随机生成四种小吃订单
        const int sideRandom = qrand() % 4;

        if (sideRandom == 0) {
            currentSideOrder = sideOrder0;
        }
        else if (sideRandom == 1) {
            currentSideOrder = sideOrder1;
        }
        else if (sideRandom == 2) {
            currentSideOrder = sideOrder2;
        }
        else {
            currentSideOrder = sideOrder3;
        }

        // 小吃订单显示文字
        const QString sideOrderText =
            currentSideOrder.isEmpty()
            ? "无"
            : currentSideOrder.join(" + ");

        orderLabel->setText(
            "汉堡订单："
            + currentBurgerOrder.join(" + ")
            + "\n小吃订单："
            + sideOrderText
        );

        // 如果本单没有小吃，自动禁用小吃提交按钮
        if (submitSideBtn) {
            const bool hasSideOrder =
                !currentSideOrder.isEmpty();

            submitSideBtn->setEnabled(hasSideOrder);

            submitSideBtn->setText(
                hasSideOrder
                ? "提交小吃"
                : "无需小吃"
            );
        }

        return;
    }

    QStringList order0 = {"底面包", "熟肉饼", "顶面包"};
    QStringList order1 = {"底面包", "生菜", "熟肉饼", "顶面包"};
    QStringList order2 = {"底面包", "生菜", "熟肉饼", "番茄", "顶面包"};


    static bool seeded = false;
    if (!seeded) {
        qsrand(QTime::currentTime().msec());
        seeded = true;
    }
    const int r = qrand() % 3;

    if (r == 0) currentOrder = order0;
    else if (r == 1) currentOrder = order1;
    else currentOrder = order2;

    orderLabel->setText("订单：" + currentOrder.join(" + "));
}
//盘子中内容的显示
void GameWindow::updatePlateDisplay()
{
    auto formatItemText = [](const IngredientItem &item) {
        const bool isPatty =
            item.name == "肉饼"
            || item.name == "熟肉饼"
            || item.name == "糊肉饼";

        const bool isFries =
            item.name == "薯条"
            || item.name == "糊薯条";

        // 面包、生菜、番茄、可乐直接显示名称
        if (!isPatty && !isFries) {
            return item.name;
        }

        const QString actionText =
            isFries ? "炸制" : "煎制";

        if (item.state == IngredientState::Raw) {
            return QString("%1(未%2)")
                .arg(item.name)
                .arg(actionText);
        }

        if (item.state == IngredientState::Cooking) {
            return QString("%1(%2中 %3秒)")
                .arg(item.name)
                .arg(actionText)
                .arg(item.cookSeconds);
        }

        if (item.state == IngredientState::Cooked) {
            return QString("%1(已熟 %2秒)")
                .arg(item.name)
                .arg(item.cookedSeconds);
        }

        return QString("%1(已糊)")
            .arg(item.name);
    };

    if (!isAdvancedMode) {
        if (!plateLabel) {
            return;
        }
        if (currentIngredients.isEmpty()) {
            plateLabel->setText("盘子：空");
            return;
        }
        QStringList texts;
        for (const IngredientItem &item : currentIngredients) {
            texts.append(formatItemText(item));
        }
        plateLabel->setText("盘子：" + texts.join(" + "));
        return;
    }

    if (burgerPlateLabel) {
        QStringList burgerTexts;
        for (const IngredientItem &item : burgerIngredients) {
            burgerTexts.append(formatItemText(item));
        }
        burgerPlateLabel->setText(
            "汉堡盘：" + (burgerTexts.isEmpty() ? "空" : burgerTexts.join(" + ")));
    }

    if (sidePlateLabel) {
        QStringList sideTexts;
        for (const IngredientItem &item : sideIngredients) {
            sideTexts.append(formatItemText(item));
        }
        sidePlateLabel->setText(
            "小吃盘：" + (sideTexts.isEmpty() ? "空" : sideTexts.join(" + ")));
    }
}

// ========== 计时与结算 ==========
void GameWindow::updateTimer()
{
    if (isPaused) {
        return;
    }

    // 记录这一秒之前是否存在商店限时效果
    const bool hadActiveShopEffect =
        cookingSpeedRemainingSeconds > 0
        || pattyCookingSpeedRemainingSeconds > 0
        || friesCookingSpeedRemainingSeconds > 0
        || orderBonusRemainingSeconds > 0;

    if (cookingSpeedRemainingSeconds > 0) {
        cookingSpeedRemainingSeconds--;
    }

    if (pattyCookingSpeedRemainingSeconds > 0) {
        pattyCookingSpeedRemainingSeconds--;
    }

    if (friesCookingSpeedRemainingSeconds > 0) {
        friesCookingSpeedRemainingSeconds--;
    }

    if (orderBonusRemainingSeconds > 0) {
        orderBonusRemainingSeconds--;
    }

    // 保存最新的技能剩余时间
    if (hadActiveShopEffect) {
        saveShopState();
    }

    if (timeLeft > 0) {
        timeLeft--;
    }

    timerLabel->setText(
        formatTimerText(timeLeft)
    );

    if (timeLeft <= 0) {
        timer->stop();
        endGame();
    }
}

void GameWindow::endGame()
{
    const int sessionEarnedCoins = qMax(0, score);
    totalCoins += sessionEarnedCoins;
    saveCoins();
    updateCoinDisplay();

    // 刷新最高分并保存到本地文件
    if (score > maxScore) {
        maxScore = score;
        maxScoreLabel->setText("最高纪录：" + QString::number(maxScore));
        QFile file(
            QCoreApplication::applicationDirPath()
            + "/maxscore.dat"
        );
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out << maxScore;
            file.close();
        }
    }
    showStarRating();

    hide();

    MainWindow *mainWin = new MainWindow(nullptr);
    mainWin->setAttribute(Qt::WA_DeleteOnClose);
    mainWin->show();
    mainWin->raise();
    mainWin->activateWindow();

    QTimer::singleShot(
        0,
        this,
        [this]() {
            deleteLater();
        }
    );
}

// 根据完成订单数评定 1~3 星
void GameWindow::showStarRating()
{
    int star = 0;
    if (completedOrders >= 5)      star = 3;
    else if (completedOrders >= 3) star = 2;
    else if (completedOrders >= 1) star = 1;

    QString starMsg;
    if (star == 3)      starMsg = "三星！海绵宝宝为你骄傲！";
    else if (star == 2) starMsg = "二星，做得不错！";
    else if (star == 1) starMsg = "一星，继续努力！";
    else                starMsg = "没有星星，下次加油！";

    const int sessionEarnedCoins = qMax(0, score);
    QMessageBox::information(nullptr, "时间到！",
        QString("完成订单：%1 单\n本次获得金币：%2\n当前总金币：%3\n最高纪录：%4\n\n%5")
        .arg(completedOrders).arg(sessionEarnedCoins).arg(totalCoins).arg(maxScore).arg(starMsg));
}

// 顶部短暂显示操作反馈，2 秒后自动清除
void GameWindow::showFeedback(const QString &message, const QString &color)
{
    feedbackLabel->setText(message);
    feedbackLabel->setStyleSheet(
        QString("font-size: 22px; font-weight: bold; color: %1;"
                "background-color: rgba(29, 53, 87, 220);"
                "border-radius: 8px; padding: 4px 12px;").arg(color));
    feedbackLabel->show();
    QTimer::singleShot(2000, feedbackLabel, [this]() {
        feedbackLabel->clear();
        feedbackLabel->hide();
    });
}

// ========== 暂停菜单 ==========

void GameWindow::onMenuClicked()
{
    // 防止连续打开多个暂停菜单
    if (isPaused) {
        return;
    }

    isPaused = true;

    if (timer) {
        timer->stop();
    }

    if (menuBtn) {
        menuBtn->setEnabled(false);
    }

    /*
     * 暂停菜单不再作为 GameWindow 的子对象。
     * 这样删除游戏窗口时，不会在父子对象销毁过程中产生冲突。
     */
    MenuWindow *menuWin = new MenuWindow(nullptr);

    menuWin->setAttribute(Qt::WA_DeleteOnClose);
    menuWin->setWindowModality(Qt::ApplicationModal);

    // 禁止点击右上角关闭，避免关闭后游戏仍然停在暂停状态
    Qt::WindowFlags flags = menuWin->windowFlags();
    flags &= ~Qt::WindowCloseButtonHint;
    menuWin->setWindowFlags(flags);

    // 继续游戏
    connect(
        menuWin,
        &MenuWindow::continueGame,
        this,
        [this, menuWin]() {
            menuWin->hide();
            menuWin->deleteLater();

            onContinueGame();
        }
    );

    // 重新开始
    connect(
        menuWin,
        &MenuWindow::restartGame,
        this,
        [this, menuWin]() {
            menuWin->hide();
            menuWin->deleteLater();

            onRestartGame();
        }
    );

    // 返回大厅
    connect(
        menuWin,
        &MenuWindow::backToLobby,
        this,
        [this, menuWin]() {
            // 先处理暂停菜单
            menuWin->hide();
            menuWin->deleteLater();

            /*
             * 等当前按钮点击和信号发送结束后，
             * 再执行游戏窗口的删除操作。
             */
            QTimer::singleShot(
                0,
                this,
                &GameWindow::onBackToLobby
            );
        }
    );

    menuWin->show();
}
void GameWindow::onContinueGame()
{
    // 恢复游戏状态
    isPaused = false;

    // 恢复菜单按钮
    if (menuBtn) {
        menuBtn->setEnabled(true);
    }

    // 恢复倒计时
    if (timer && !timer->isActive()) {
        timer->start(1000);
    }
}
// 重置本局所有数据，重新开始 60 秒挑战
void GameWindow::onRestartGame()
{
    isPaused = false;

    // 恢复菜单按钮
    if (menuBtn) {
        menuBtn->setEnabled(true);
    }

    score = 0;
    comboCount = 0;
    completedOrders = 0;
    timeLeft = 60;
    cookingSpeedRemainingSeconds = 0;
    pattyCookingSpeedRemainingSeconds = 0;
    friesCookingSpeedRemainingSeconds = 0;
    orderBonusRemainingSeconds = 0;
    saveShopState();
    orderSerial = 1;
    currentIngredients.clear();
    burgerIngredients.clear();
    sideIngredients.clear();
    if (isAdvancedMode) {
        if (submitBurgerBtn) {
            submitBurgerBtn->setEnabled(true);
        }

        if (submitSideBtn) {
            submitSideBtn->setEnabled(true);
        }
    }

    scoreLabel->setText("本局得分：0");
    serialLabel->setText("第 1 单");
    timerLabel->setText("01:00");
    updateCoinDisplay();
    feedbackLabel->hide();
    updatePlateDisplay();
    generateNewOrder();

    if (!timer->isActive()) timer->start(1000);
}

void GameWindow::onBackToLobby()
{
    // 立即停止游戏逻辑
    isPaused = true;

    if (timer) {
        timer->stop();
    }

    // 先隐藏游戏窗口
    hide();

    // 创建并显示新的大厅
    MainWindow *mainWin = new MainWindow(nullptr);
    mainWin->setAttribute(Qt::WA_DeleteOnClose);
    mainWin->show();
    mainWin->raise();
    mainWin->activateWindow();

    /*
     * 再延迟删除游戏窗口。
     * 确保返回大厅的信号、按钮点击事件均已处理完毕。
     */
    QTimer::singleShot(
        0,
        this,
        [this]() {
            deleteLater();
        }
    );
}
