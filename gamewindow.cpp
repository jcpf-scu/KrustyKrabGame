/**
 * @file gamewindow.cpp
 * @brief 游戏主窗口实现
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

static const char *BTN_STYLE =
    "QPushButton { background-color: #FFD166; color: #1D3557;"
    "font-size: 20px; font-weight: bold; border-radius: 8px;"
    "font-family: 'Microsoft YaHei', sans-serif; padding: 10px; }"
    "QPushButton:hover { background-color: #FFC145; }";

static const char *ACTION_STYLE =
    "QPushButton { background-color: #E63946; color: white;"
    "font-size: 20px; font-weight: bold; border-radius: 8px;"
    "font-family: 'Microsoft YaHei', sans-serif; padding: 10px; }"
    "QPushButton:hover { background-color: #C1121F; }";

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

QString PlateItem::displayName() const
{
    if (name == "肉饼" && grillElapsed >= 0) {
        if (grillElapsed < 3) return "煎制中·肉饼";
        if (grillElapsed < 7) return "肉饼";
        return "糊肉饼";
    }
    if (name == "薯条" && grillElapsed >= 0) {
        if (grillElapsed < 3) return "煎制中·薯条";
        if (grillElapsed < 7) return "薯条";
        return "糊薯条";
    }
    return name;
}

QString PlateItem::orderName() const
{
    const QString shown = displayName();
    if (shown.startsWith("煎制中") || shown.startsWith("糊")
        || shown == "生肉饼" || shown == "生薯条") {
        return QString();
    }
    return shown;
}

GameWindow::GameWindow(GameDifficulty difficulty, QWidget *parent)
    : QMainWindow(parent)
    , difficulty(difficulty)
    , isPaused(false)
    , comboCount(0)
    , maxScore(0)
    , orderSerial(1)
    , score(0)
    , completedOrders(0)
    , timeLeft(initialTimeForDifficulty())
    , rawFriesBtn(nullptr)
    , colaBtn(nullptr)
    , hardIngredientRow(nullptr)
    , grillTimer(nullptr)
{
    setFixedSize(1125, 812);
    setWindowTitle(difficulty == GameDifficulty::Hard
        ? "蟹堡王 - 制作蟹黄堡（困难）"
        : "蟹堡王 - 制作蟹黄堡（简单）");

    const char *bgPath = difficulty == GameDifficulty::Hard
        ? ":/images/game_hard_bg.jpg" : ":/images/game_bg.jpg";
    const char *bgFallback = difficulty == GameDifficulty::Hard
        ? "images/game_hard_bg.jpg" : "images/game_bg.jpg";

    QLabel *bgLabel = new QLabel(this);
    bgLabel->setGeometry(0, 0, 1125, 812);
    bgLabel->setScaledContents(true);
    QPixmap bgPixmap(bgPath);
    if (bgPixmap.isNull()) {
        bgPixmap.load(bgFallback);
    }
    if (!bgPixmap.isNull()) {
        bgLabel->setPixmap(bgPixmap.scaled(bgLabel->size(),
                                           Qt::KeepAspectRatioByExpanding,
                                           Qt::FastTransformation));
        bgLabel->setAttribute(Qt::WA_StaticContents, true);
        bgLabel->setAttribute(Qt::WA_OpaquePaintEvent, true);
    } else {
        bgLabel->setStyleSheet("background-color: #F1FAEE;");
    }

    QFile file("maxscore.dat");
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> maxScore;
        file.close();
    }

    QWidget *central = new QWidget(this);
    central->setStyleSheet("background: transparent;");
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(25, 19, 25, 19);

    QHBoxLayout *topBar = new QHBoxLayout();
    serialLabel = new QLabel("第 1 单", this);
    serialLabel->setStyleSheet(ACCENT_PANEL_STYLE);

    orderLabel = new QLabel(this);
    orderLabel->setStyleSheet(INFO_PANEL_STYLE);
    orderLabel->setAlignment(Qt::AlignCenter);

    timerLabel = new QLabel(
        QString("%1:%2")
            .arg(initialTimeForDifficulty() / 60, 2, 10, QChar('0'))
            .arg(initialTimeForDifficulty() % 60, 2, 10, QChar('0')),
        this);
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

    feedbackLabel = new QLabel(this);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    feedbackLabel->setFixedHeight(38);
    feedbackLabel->hide();
    mainLayout->addWidget(feedbackLabel);

    feedbackTimer = new QTimer(this);
    feedbackTimer->setSingleShot(true);
    connect(feedbackTimer, &QTimer::timeout, this, [this]() {
        feedbackLabel->clear();
        feedbackLabel->hide();
    });

    QLabel *ingredientTitle = new QLabel("食材区", this);
    ingredientTitle->setStyleSheet(SECTION_TITLE_STYLE);
    mainLayout->addWidget(ingredientTitle);

    QHBoxLayout *ingredientRow = new QHBoxLayout();
    bottomBunBtn = new QPushButton("底面包", this);
    rawPattyBtn = new QPushButton("生肉饼", this);
    topBunBtn = new QPushButton("顶面包", this);
    lettuceBtn = new QPushButton("生菜", this);
    tomatoBtn = new QPushButton("番茄", this);
    for (QPushButton *btn : {bottomBunBtn, rawPattyBtn, lettuceBtn, tomatoBtn, topBunBtn}) {
        btn->setFixedHeight(63);
        btn->setStyleSheet(BTN_STYLE);
        ingredientRow->addWidget(btn);
    }
    mainLayout->addLayout(ingredientRow);

    if (difficulty == GameDifficulty::Hard) {
        hardIngredientRow = new QWidget(this);
        QHBoxLayout *hardRow = new QHBoxLayout(hardIngredientRow);
        hardRow->setContentsMargins(0, 0, 0, 0);
        rawFriesBtn = new QPushButton("生薯条", this);
        colaBtn = new QPushButton("可乐", this);
        for (QPushButton *btn : {rawFriesBtn, colaBtn}) {
            btn->setFixedHeight(63);
            btn->setStyleSheet(BTN_STYLE);
            hardRow->addWidget(btn);
        }
        hardRow->addStretch();
        mainLayout->addWidget(hardIngredientRow);
    }

    mainLayout->addSpacing(difficulty == GameDifficulty::Hard ? 35 : 55);

    plateLabel = new QLabel("盘子：空", this);
    plateLabel->setAlignment(Qt::AlignCenter);
    plateLabel->setMinimumHeight(150);
    plateLabel->setStyleSheet(
        "font-size: 28px; font-weight: bold; color: #1D3557;"
        "background-color: rgba(255, 255, 255, 235); border: 3px dashed #A8DADC;"
        "border-radius: 15px; padding: 25px;"
    );
    mainLayout->addWidget(plateLabel);

    QLabel *actionTitle = new QLabel("操作区", this);
    actionTitle->setStyleSheet(SECTION_TITLE_STYLE);
    mainLayout->addWidget(actionTitle);

    QHBoxLayout *actionRow = new QHBoxLayout();
    grillBtn = new QPushButton(
        difficulty == GameDifficulty::Hard ? "煎制(3秒)" : "煎制", this);
    discardBtn = new QPushButton("丢弃", this);
    submitBtn = new QPushButton("提交订单", this);
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

    QHBoxLayout *bottomBar = new QHBoxLayout();
    scoreLabel = new QLabel("本局累计金币：0", this);
    scoreLabel->setStyleSheet(SCORE_PANEL_STYLE);
    maxScoreLabel = new QLabel("最高纪录：" + QString::number(maxScore), this);
    maxScoreLabel->setStyleSheet(MAX_SCORE_PANEL_STYLE);
    maxScoreLabel->setAlignment(Qt::AlignRight);
    bottomBar->addWidget(scoreLabel);
    bottomBar->addWidget(maxScoreLabel, 1);
    mainLayout->addLayout(bottomBar);

    connect(bottomBunBtn, &QPushButton::clicked, this, &GameWindow::onBottomBunClicked);
    connect(rawPattyBtn,  &QPushButton::clicked, this, &GameWindow::onRawPattyClicked);
    connect(topBunBtn,    &QPushButton::clicked, this, &GameWindow::onTopBunClicked);
    connect(tomatoBtn,    &QPushButton::clicked, this, &GameWindow::onTomatoClicked);
    connect(lettuceBtn,   &QPushButton::clicked, this, &GameWindow::onLettuceClicked);
    if (rawFriesBtn) {
        connect(rawFriesBtn, &QPushButton::clicked, this, &GameWindow::onRawFriesClicked);
        connect(colaBtn,     &QPushButton::clicked, this, &GameWindow::onColaClicked);
    }
    connect(grillBtn,     &QPushButton::clicked, this, &GameWindow::onGrillClicked);
    connect(discardBtn,   &QPushButton::clicked, this, &GameWindow::onDiscardClicked);
    connect(submitBtn,    &QPushButton::clicked, this, &GameWindow::onSubmitClicked);
    connect(menuBtn,      &QPushButton::clicked, this, &GameWindow::onMenuClicked);

    bgLabel->lower();

    generateNewOrder();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::updateTimer);
    timer->start(1000);

    if (difficulty == GameDifficulty::Hard) {
        grillTimer = new QTimer(this);
        connect(grillTimer, &QTimer::timeout, this, &GameWindow::onGrillTimerTick);
    }
}

GameWindow::~GameWindow() {}

void GameWindow::addIngredient(const QString &name)
{
    PlateItem item;
    item.name = name;
    item.grillElapsed = -1;
    currentIngredients.append(item);
    updatePlateDisplay();
}

void GameWindow::onBottomBunClicked() { addIngredient("底面包"); }
void GameWindow::onRawPattyClicked()  { addIngredient("生肉饼"); }
void GameWindow::onTopBunClicked()    { addIngredient("顶面包"); }
void GameWindow::onLettuceClicked()   { addIngredient("生菜"); }
void GameWindow::onTomatoClicked()    { addIngredient("番茄"); }
void GameWindow::onRawFriesClicked()  { addIngredient("生薯条"); }
void GameWindow::onColaClicked()      { addIngredient("可乐"); }

bool GameWindow::startGrilling()
{
    bool started = false;
    for (PlateItem &item : currentIngredients) {
        if (item.name == "生肉饼") {
            item.name = "肉饼";
            item.grillElapsed = 0;
            started = true;
        } else if (item.name == "生薯条") {
            item.name = "薯条";
            item.grillElapsed = 0;
            started = true;
        }
    }
    if (started && grillTimer && !grillTimer->isActive()) {
        grillTimer->start(1000);
    }
    return started;
}

void GameWindow::onGrillClicked()
{
    if (difficulty == GameDifficulty::Easy) {
        bool found = false;
        for (int i = 0; i < currentIngredients.size(); ++i) {
            if (currentIngredients[i].name == "生肉饼") {
                currentIngredients[i].name = "肉饼";
                currentIngredients[i].grillElapsed = -1;
                found = true;
            }
        }
        if (found) {
            updatePlateDisplay();
            showFeedback("煎制成功！生肉饼 → 肉饼", "#2A9D8F");
        } else {
            showFeedback("盘子里没有生肉饼，无法煎制", "#E63946");
        }
        return;
    }

    if (startGrilling()) {
        updatePlateDisplay();
        showFeedback("开始煎制！3 秒后完成，再 4 秒后会煎糊", "#2A9D8F");
    } else {
        showFeedback("盘子里没有生肉饼或生薯条，无法煎制", "#E63946");
    }
}

void GameWindow::onGrillTimerTick()
{
    if (isPaused) return;

    bool keepTimer = false;
    for (PlateItem &item : currentIngredients) {
        if (item.grillElapsed < 0 || item.grillElapsed >= 7) {
            continue;
        }

        item.grillElapsed++;
        if (item.grillElapsed == 7) {
            showFeedback(
                item.name == "肉饼" ? "肉饼煎糊了！请丢弃" : "薯条煎糊了！请丢弃",
                "#E63946");
        } else {
            keepTimer = true;
        }
    }

    updatePlateDisplay();
    if (!keepTimer) {
        resetGrillTimer();
    }
}

void GameWindow::resetGrillTimer()
{
    if (grillTimer) {
        grillTimer->stop();
    }
}

bool GameWindow::hasInvalidIngredients() const
{
    for (const PlateItem &item : currentIngredients) {
        const QString shown = item.displayName();
        if (shown.startsWith("煎制中") || shown.startsWith("糊")
            || shown == "生肉饼" || shown == "生薯条") {
            return true;
        }
    }
    return false;
}

QStringList GameWindow::plateOrderNames() const
{
    QStringList names;
    for (const PlateItem &item : currentIngredients) {
        const QString n = item.orderName();
        if (!n.isEmpty()) {
            names.append(n);
        }
    }
    return names;
}

void GameWindow::onDiscardClicked()
{
    currentIngredients.clear();
    resetGrillTimer();
    updatePlateDisplay();
    showFeedback("已清空盘子", "#6C757D");
}

void GameWindow::onSubmitClicked()
{
    if (hasInvalidIngredients()) {
        score -= 5;
        comboCount = 0;
        scoreLabel->setText("得分：" + QString::number(score));
        showFeedback("有未煎好或煎糊的食材！无法提交，请丢弃重做", "#E63946");
        return;
    }

    const QStringList plateNames = plateOrderNames();
    bool match = (plateNames.size() == currentOrder.size());
    if (match) {
        for (const QString &need : currentOrder) {
            if (!plateNames.contains(need)) {
                match = false;
                break;
            }
        }
    }

    if (match) {
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
        resetGrillTimer();
        updatePlateDisplay();
        generateNewOrder();
        orderSerial++;
        serialLabel->setText(QString("第 %1 单").arg(orderSerial));
        scoreLabel->setText("得分：" + QString::number(score));
    } else {
        score -= 5;
        comboCount = 0;
        scoreLabel->setText("得分：" + QString::number(score));
        showFeedback("订单错误！消耗原材料-5金币，请重新制作", "#E63946");
        currentIngredients.clear();
        resetGrillTimer();
        updatePlateDisplay();
    }
}

void GameWindow::generateNewOrder()
{
    currentOrder.clear();
    QStringList order0 = {"底面包", "肉饼", "顶面包"};
    QStringList order1 = {"底面包", "生菜", "肉饼", "顶面包"};
    QStringList order2 = {"底面包", "生菜", "肉饼", "番茄", "顶面包"};
    QStringList order3 = {"薯条", "可乐"};
    QStringList order4 = {"底面包", "肉饼", "顶面包", "薯条", "可乐"};
    QStringList order5 = {"底面包", "生菜", "肉饼", "顶面包", "薯条", "可乐"};

    qsrand(QTime::currentTime().msec());
    if (difficulty == GameDifficulty::Hard) {
        QStringList orders[] = {order1, order2, order3, order4, order5};
        currentOrder = orders[qrand() % 5];
    } else {
        int r = qrand() % 3;
        if (r == 0)      currentOrder = order0;
        else if (r == 1) currentOrder = order1;
        else             currentOrder = order2;
    }
    orderLabel->setText("订单：" + currentOrder.join(" + "));
}

void GameWindow::updatePlateDisplay()
{
    QString text;
    if (currentIngredients.isEmpty()) {
        text = "盘子：空";
    } else {
        QStringList shown;
        for (const PlateItem &item : currentIngredients) {
            shown.append(item.displayName());
        }
        text = "盘子：" + shown.join(" + ");
    }
    if (plateLabel->text() != text) {
        plateLabel->setText(text);
    }
}

void GameWindow::updateTimer()
{
    if (isPaused) return;

    timeLeft--;
    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;
    timerLabel->setText(QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0')));

    if (timeLeft <= 0) {
        timer->stop();
        resetGrillTimer();
        endGame();
    }
}

void GameWindow::endGame()
{
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

void GameWindow::showFeedback(const QString &message, const QString &color)
{
    static const char *STYLE_GREEN =
        "font-size: 22px; font-weight: bold; color: #2A9D8F;"
        "background-color: rgba(29, 53, 87, 220);"
        "border-radius: 8px; padding: 4px 12px;";
    static const char *STYLE_RED =
        "font-size: 22px; font-weight: bold; color: #E63946;"
        "background-color: rgba(29, 53, 87, 220);"
        "border-radius: 8px; padding: 4px 12px;";
    static const char *STYLE_ORANGE =
        "font-size: 22px; font-weight: bold; color: #F4A261;"
        "background-color: rgba(29, 53, 87, 220);"
        "border-radius: 8px; padding: 4px 12px;";
    static const char *STYLE_GRAY =
        "font-size: 22px; font-weight: bold; color: #6C757D;"
        "background-color: rgba(29, 53, 87, 220);"
        "border-radius: 8px; padding: 4px 12px;";

    feedbackLabel->setText(message);
    if (color == "#2A9D8F")      feedbackLabel->setStyleSheet(STYLE_GREEN);
    else if (color == "#E63946") feedbackLabel->setStyleSheet(STYLE_RED);
    else if (color == "#F4A261") feedbackLabel->setStyleSheet(STYLE_ORANGE);
    else                         feedbackLabel->setStyleSheet(STYLE_GRAY);
    feedbackLabel->show();
    feedbackTimer->start(2000);
}

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
    if (grillTimer) {
        for (const PlateItem &item : currentIngredients) {
            if (item.grillElapsed >= 0 && item.grillElapsed < 7) {
                grillTimer->start(1000);
                break;
            }
        }
    }
}

void GameWindow::onRestartGame()
{
    isPaused = false;
    score = 0;
    comboCount = 0;
    completedOrders = 0;
    timeLeft = initialTimeForDifficulty();
    orderSerial = 1;
    currentIngredients.clear();
    resetGrillTimer();

    scoreLabel->setText("积累金币：0");
    serialLabel->setText("第 1 单");
    timerLabel->setText(
        QString("%1:%2")
            .arg(timeLeft / 60, 2, 10, QChar('0'))
            .arg(timeLeft % 60, 2, 10, QChar('0')));
    feedbackLabel->hide();
    updatePlateDisplay();
    generateNewOrder();

    if (!timer->isActive()) timer->start(1000);
}

void GameWindow::onBackToLobby()
{
    resetGrillTimer();
    close();
    MainWindow *mainWin = new MainWindow();
    mainWin->show();
}

int GameWindow::initialTimeForDifficulty() const
{
    return 60;
}
