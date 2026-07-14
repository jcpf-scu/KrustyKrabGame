/**
 * @brief 暂停菜单实现
 */

#include "menuwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QPixmap>

MenuWindow::MenuWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(650, 550);
    setWindowTitle("游戏菜单");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // ========= 暂停菜单背景图片 =========
    QLabel *bgLabel = new QLabel(central);
    bgLabel->setGeometry(0, 0, 650, 550);
    bgLabel->setScaledContents(true);

    // 优先从Qt资源文件中读取
    QPixmap bgPixmap(":/images/pause_bg.jpg");

    // 资源文件读取失败时，尝试从项目images文件夹读取
    if (bgPixmap.isNull()) {
        bgPixmap.load("images/pause_bg.jpg");
    }

    if (!bgPixmap.isNull()) {
        bgLabel->setPixmap(
            bgPixmap.scaled(
                bgLabel->size(),
                Qt::KeepAspectRatioByExpanding,
                Qt::SmoothTransformation
            )
        );
    } else {
        // 图片读取失败时使用备用颜色
        bgLabel->setStyleSheet(
            "background-color: #F1FAEE;"
        );
    }

    // 将背景图片放到最底层
    bgLabel->lower();

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setSpacing(19);
    layout->setContentsMargins(50, 38, 50, 38);

    QLabel *title = new QLabel("游戏暂停", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 40px;"
        "font-weight: bold;"
        "color: #1D3557;"
        "background-color: rgba(255, 255, 255, 190);"
        "border-radius: 10px;"
        "padding: 8px;"
    );
    //title->setStyleSheet("font-size: 30px; font-weight: bold; color: #1D3557;");
    layout->addWidget(title);
    layout->addSpacing(13);

    // 创建统一样式按钮的辅助 lambda（hover 必须改背景色，Qt 样式表不支持 opacity）
    auto makeBtn = [this](const QString &text, const QString &color, const QString &hoverColor) {
        QPushButton *btn = new QPushButton(text, this);
        btn->setFixedHeight(63);
        btn->setStyleSheet(QString(
            "QPushButton { background-color: %1; color: white;"
            "font-size: 22px; font-weight: bold; border-radius: 10px; }"
            "QPushButton:hover { background-color: %2; }"
            "QPushButton:pressed { background-color: %2; }"
        ).arg(color, hoverColor));
        return btn;
    };

    QPushButton *continueBtn = makeBtn("继续游戏", "#2A9D8F", "#21867A");
    QPushButton *restartBtn  = makeBtn("重新开始", "#E63946", "#C1121F");
    QPushButton *backBtn     = makeBtn("返回大厅", "#457B9D", "#3A6885");

    layout->addWidget(continueBtn);
    layout->addWidget(restartBtn);
    layout->addWidget(backBtn);

    // 点击后发射信号，由 GameWindow 处理具体逻辑
    connect(continueBtn, &QPushButton::clicked, this, &MenuWindow::onContinueClicked);
    connect(restartBtn,  &QPushButton::clicked, this, &MenuWindow::onRestartClicked);
    connect(backBtn,     &QPushButton::clicked, this, &MenuWindow::onBackClicked);

}

MenuWindow::~MenuWindow() {}

void MenuWindow::onContinueClicked()
{
    // 防止重复点击
    setEnabled(false);

    // 只发送信号，窗口由 GameWindow 统一关闭
    emit continueGame();
}

void MenuWindow::onRestartClicked()
{
    // 防止重复点击
    setEnabled(false);

    // 只发送信号，窗口由 GameWindow 统一关闭
    emit restartGame();
}

void MenuWindow::onBackClicked()
{
    // 防止重复点击
    setEnabled(false);

    // 只发送信号，窗口由 GameWindow 统一关闭
    emit backToLobby();
}
