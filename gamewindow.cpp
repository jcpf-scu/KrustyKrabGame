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

GameWindow::GameWindow(bool advancedMode,QWidget *parent)
    : QMainWindow(parent)
    ,isAdvancedMode(advancedMode)
    , isPaused(false)
    , comboCount(0)
    , maxScore(0)
    , orderSerial(1)
    , score(0)
    , completedOrders(0)
    , timeLeft(60)   // 简单模式：60 秒倒计时
{
    setFixedSize(1125, 812);
    setWindowTitle("蟹堡王 - 制作蟹黄堡");

    // ========= 背景层 =========
    QLabel *bgLabel = new QLabel(this);
    bgLabel->setGeometry(0, 0, 1125, 812);
    bgLabel->setScaledContents(true);
    QPixmap bgPixmap(":/images/game_bg.jpg");
    if (bgPixmap.isNull()) {
        bgPixmap.load("images/game_bg.jpg");
    }
    if (!bgPixmap.isNull()) {
        bgLabel->setPixmap(bgPixmap.scaled(bgLabel->size(),
                                           Qt::KeepAspectRatioByExpanding,
                                           Qt::SmoothTransformation));
    } else {
        bgLabel->setStyleSheet("background-color: #F1FAEE;");
    }

    // 读取本地保存的最高分
    QFile file("maxscore.dat");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> maxScore;
        file.close();
    }

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

    QHBoxLayout *ingredientRow = new QHBoxLayout();
    bottomBunBtn = new QPushButton("底面包", this);
    rawPattyBtn = new QPushButton("生肉饼", this);
    topBunBtn = new QPushButton("顶面包", this);
    lettuceBtn = new QPushButton("生菜",this);
    tomatoBtn= new QPushButton("番茄",this);

    if(isAdvancedMode){
        friesBtn = new QPushButton("薯条", this);
        colaBtn = new QPushButton("可乐", this);
        for (QPushButton *btn : {friesBtn,colaBtn,bottomBunBtn, rawPattyBtn,lettuceBtn,tomatoBtn, topBunBtn}) {
            btn->setFixedHeight(63);
            btn->setStyleSheet(BTN_STYLE);
            ingredientRow->addWidget(btn);
        }
    }
    else{
        for (QPushButton *btn : {bottomBunBtn, rawPattyBtn,lettuceBtn,tomatoBtn, topBunBtn}) {
            btn->setFixedHeight(63);
            btn->setStyleSheet(BTN_STYLE);
            ingredientRow->addWidget(btn);
        }
    }

    mainLayout->addLayout(ingredientRow);

    mainLayout->addSpacing(55);

    // ---- 盘子显示区 ----
    plateLabel = new QLabel("盘子：空", this);
    plateLabel->setAlignment(Qt::AlignCenter);
    plateLabel->setMinimumHeight(150);
    plateLabel->setStyleSheet(
        "font-size: 28px; font-weight: bold; color: #1D3557;"
        "background-color: rgba(255, 255, 255, 235); border: 3px dashed #A8DADC;"
        "border-radius: 15px; padding: 25px;"
    );
    mainLayout->addWidget(plateLabel);


    // ---- 操作按钮区 ----
    QLabel *actionTitle = new QLabel("操作区", this);
    actionTitle->setStyleSheet(SECTION_TITLE_STYLE);
    mainLayout->addWidget(actionTitle);

    QHBoxLayout *actionRow = new QHBoxLayout();
    grillBtn   = new QPushButton("煎制", this);
    discardBtn = new QPushButton("丢弃", this);
    submitBtn  = new QPushButton("提交订单", this);
    grillBtn->setFixedHeight(63);
    discardBtn->setFixedHeight(63);
    submitBtn->setFixedHeight(63);
    grillBtn->setStyleSheet(BTN_STYLE);
    discardBtn->setStyleSheet(
        "QPushButton { background-color: #6C757D; color: white;"
        "font-size: 20px; font-weight: bold; border-radius: 8px; padding: 10px; }"
        "QPushButton:hover { background-color: #495057; }"
    );
    submitBtn->setStyleSheet(ACTION_STYLE);
    actionRow->addWidget(grillBtn);
    actionRow->addWidget(discardBtn);
    actionRow->addWidget(submitBtn, 1);
    mainLayout->addLayout(actionRow);

    mainLayout->addStretch();

    // ---- 底部分数栏 ----
    QHBoxLayout *bottomBar = new QHBoxLayout();
    scoreLabel = new QLabel("本局累计金币：0", this);
    scoreLabel->setStyleSheet(SCORE_PANEL_STYLE);
    maxScoreLabel = new QLabel("最高纪录：" + QString::number(maxScore), this);
    maxScoreLabel->setStyleSheet(MAX_SCORE_PANEL_STYLE);
    maxScoreLabel->setAlignment(Qt::AlignRight);
    bottomBar->addWidget(scoreLabel);
    bottomBar->addWidget(maxScoreLabel, 1);
    mainLayout->addLayout(bottomBar);

    // ========= 信号槽连接 =========
    connect(bottomBunBtn, &QPushButton::clicked, this, &GameWindow::onBottomBunClicked);
    connect(rawPattyBtn,  &QPushButton::clicked, this, &GameWindow::onRawPattyClicked);
    connect(topBunBtn,    &QPushButton::clicked, this, &GameWindow::onTopBunClicked);
    connect(tomatoBtn,    &QPushButton::clicked, this, &GameWindow::onTomatoClicked);
    connect(lettuceBtn,    &QPushButton::clicked, this, &GameWindow::onLettuceClicked);
    connect(colaBtn,  &QPushButton::clicked, this , &GameWindow::onColaClicked);
    connect(friesBtn,  &QPushButton::clicked, this , &GameWindow::onFriesClicked);
    connect(grillBtn,     &QPushButton::clicked, this, &GameWindow::onGrillClicked);
    connect(discardBtn,   &QPushButton::clicked, this, &GameWindow::onDiscardClicked);
    connect(submitBtn,    &QPushButton::clicked, this, &GameWindow::onSubmitClicked);
    connect(menuBtn,      &QPushButton::clicked, this, &GameWindow::onMenuClicked);
    connect(timer,       &QTimer::timeout, this, &GameWindow::updateTimer);


    bgLabel->lower();

    // 生成第一单订单，启动倒计时
    generateNewOrder();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::updateTimer);
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
        currentIngredients.append(item);
        updatePlateDisplay();
    }
    void GameWindow::onFriesClicked()
    {
        IngredientItem item;
        item.name = "薯条";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        currentIngredients.append(item);
        updatePlateDisplay();
    }

    void GameWindow::onBottomBunClicked()
    {
        IngredientItem item;
        item.name = "底面包";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        currentIngredients.append(item);
        updatePlateDisplay();
    }

    void GameWindow::onRawPattyClicked()
    {
        IngredientItem item;
        item.name = "肉饼";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        currentIngredients.append(item);
        updatePlateDisplay();
    }

    void GameWindow::onTopBunClicked()
    {
        IngredientItem item;
        item.name = "顶面包";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        currentIngredients.append(item);
        updatePlateDisplay();
    }

    void GameWindow::onLettuceClicked()
    {
        IngredientItem item;
        item.name = "生菜";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        currentIngredients.append(item);
        updatePlateDisplay();
    }

    void GameWindow::onTomatoClicked()
    {
        IngredientItem item;
        item.name = "番茄";
        item.state = IngredientState::Raw;
        item.cookSeconds = 0;
        item.cookedSeconds = 0;
        currentIngredients.append(item);
        updatePlateDisplay();
    }

// 煎制
void GameWindow::onGrillClicked()
{
    for (IngredientItem &item : currentIngredients) {
        if ((item.name == "肉饼" || item.name == "薯条") && item.state == IngredientState::Raw) {
            item.state = IngredientState::Cooking;
            item.cookSeconds = 0;
            item.cookedSeconds = 0;
            showFeedback(item.name + "开始煎制", "#2A9D8F");
            updatePlateDisplay();
            return;
        }
    }
    showFeedback("没有可煎制的肉饼或薯条", "#E63946");
}
//煎制的定时功能
void GameWindow::updateIngredientTimers()
{
    for (IngredientItem &item : currentIngredients) {
        if (item.state == IngredientState::Cooking) {
            item.cookSeconds++;
            if (item.cookSeconds >= 3) {
                item.state = IngredientState::Cooked;
                item.cookedSeconds = 0;
                if (item.name == "肉饼") {
                        item.name = "熟肉饼";
                    }
            }
        }
        else if (item.state == IngredientState::Cooked) {
            item.cookedSeconds++;
            if (item.cookedSeconds >= 4) {
                item.state = IngredientState::Burnt;
                if (item.name == "熟肉饼") {
                        item.name = "糊肉饼";
                    }
            }
        }
    }
    updatePlateDisplay();
}
// 丢弃：清空盘子，订单不变，可重新制作
void GameWindow::onDiscardClicked()
{
    currentIngredients.clear();
    updatePlateDisplay();
    showFeedback("已清空盘子", "#6C757D");
}

// ========== 提交订单与判分 ==========

void GameWindow::onSubmitClicked()
{
    // 检查食材种类和数量是否与订单一致
    bool match = true;
    QStringList plateNames;
    for (const IngredientItem &item : currentIngredients) {
        plateNames << item.name;
    }
    for (const QString &need : currentOrder) {
        if (!plateNames.contains(need)) {
            match = false;
            break;
        }
    }
    //检测是否煎糊
    for (const IngredientItem &item : currentIngredients) {
           if (item.state == IngredientState::Burnt) {
               showFeedback("有煎糊食材，不能提交，只能丢掉", "#E63946");
               return;
           }
       }

    if (match) {
        // 正确：加分 + 连击奖励
        score += 10;
        comboCount++;
        if (comboCount >= 5) {
            score += 10;
            showFeedback("五连击！额外 +10 金币！", "#E63946");
        } else if (comboCount >= 3) {
            score += 5;
            showFeedback("三连击！额外 +5 金币！", "#F4A261");
        } else {
            showFeedback("美味！订单完成 +10 金币", "#2A9D8F");
        }
        completedOrders++;
        currentIngredients.clear();
        updatePlateDisplay();
        generateNewOrder();
        orderSerial++;
        serialLabel->setText(QString("第 %1 单").arg(orderSerial));
        scoreLabel->setText("得分：" + QString::number(score));
    } else {
        // 错误：扣分，连击清零，订单不变
        score -= 5;
        comboCount = 0;
        scoreLabel->setText("得分：" + QString::number(score));
        showFeedback("订单错误！消耗原材料-5金币，请重新制作", "#E63946");
        currentIngredients.clear();
        updatePlateDisplay();
    }
}

// ========== 订单与显示 ==========
//订单的两种显示
void GameWindow::generateNewOrder()
{
    currentOrder.clear();

    QStringList order0 = {"底面包", "熟肉饼", "顶面包"};
    QStringList order1 = {"底面包", "生菜", "熟肉饼", "顶面包"};
    QStringList order2 = {"底面包", "生菜", "熟肉饼", "番茄", "顶面包"};

    QStringList order3 = {"底面包", "熟肉饼", "薯条", "可乐", "顶面包"};
    QStringList order4 = {"底面包", "生菜", "熟肉饼", "薯条", "可乐", "顶面包"};

    qsrand(QTime::currentTime().msec());
    int r = qrand() % ( isAdvancedMode? 5 : 3);

    if (r == 0) currentOrder = order0;
    else if (r == 1) currentOrder = order1;
    else if (r == 2) currentOrder = order2;
    else if (r == 3) currentOrder = order3;
    else currentOrder = order4;

    orderLabel->setText("订单：" + currentOrder.join(" + "));
}
//盘子中内容的显示
void GameWindow::updatePlateDisplay()
{
    if (currentIngredients.isEmpty()) {
        plateLabel->setText("盘子：空");
        return;
    }

    QStringList texts;
    for (const IngredientItem &item : currentIngredients) {
        QString text;
        if (item.name == "肉饼" || item.name == "薯条") {
            if (item.state == IngredientState::Raw) {
                text = QString("%1(0/3s)").arg(item.name);
            } else if (item.state == IngredientState::Cooking) {
                text = QString("%1(%2/3s)").arg(item.name).arg(item.cookSeconds);
            } else if (item.state == IngredientState::Cooked) {
                text = QString("%1(已熟 %2/4s)").arg(item.name).arg(item.cookedSeconds);
            } else {
                text = QString("%1(煎糊)").arg(item.name);
            }
        } else {
            text = item.name;
        }
        texts.append(text);
    }

    plateLabel->setText("盘子：" + texts.join(" + "));
}

// ========== 计时与结算 ==========

void GameWindow::updateTimer()
{
    if (isPaused) return;  // 暂停时不倒计时

    timeLeft--;
    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;
    timerLabel->setText(QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));

    if (timeLeft <= 0) {
        timer->stop();
        endGame();
    }
}

void GameWindow::endGame()
{
    // 刷新最高分并保存到本地文件
    if (score > maxScore) {
        maxScore = score;
        maxScoreLabel->setText("最高纪录：" + QString::number(maxScore));
        QFile file("maxscore.dat");
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out << maxScore;
            file.close();
        }
    }
    showStarRating();
    close();
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
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

    QMessageBox::information(nullptr, "时间到！",
        QString("完成订单：%1 单\n本次获得金币：%2\n最高纪录：%3\n\n%4")
        .arg(completedOrders).arg(score).arg(maxScore).arg(starMsg));
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
    isPaused = true;
    timer->stop();

    MenuWindow *menuWin = new MenuWindow(this);
    connect(menuWin, &MenuWindow::continueGame, this, &GameWindow::onContinueGame);
    connect(menuWin, &MenuWindow::restartGame,  this, &GameWindow::onRestartGame);
    connect(menuWin, &MenuWindow::backToLobby,  this, &GameWindow::onBackToLobby);
    menuWin->setAttribute(Qt::WA_DeleteOnClose);
    menuWin->show();
}

void GameWindow::onContinueGame()
{
    isPaused = false;
    timer->start(1000);
}

// 重置本局所有数据，重新开始 60 秒挑战
void GameWindow::onRestartGame()
{
    isPaused = false;
    score = 0;
    comboCount = 0;
    completedOrders = 0;
    timeLeft = 60;
    orderSerial = 1;
    currentIngredients.clear();

    scoreLabel->setText("积累金币：0");
    serialLabel->setText("第 1 单");
    timerLabel->setText("01:00");
    feedbackLabel->hide();
    updatePlateDisplay();
    generateNewOrder();

    if (!timer->isActive()) timer->start(1000);
}

void GameWindow::onBackToLobby()
{
    close();
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
}
