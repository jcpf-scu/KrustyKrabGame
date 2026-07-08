/**
 * @brief 暂停菜单实现
 */

#include "menuwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>

MenuWindow::MenuWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(450, 400);
    setWindowTitle("游戏菜单");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setSpacing(19);
    layout->setContentsMargins(50, 38, 50, 38);

    QLabel *title = new QLabel("游戏暂停", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 30px; font-weight: bold; color: #1D3557;");
    layout->addWidget(title);
    layout->addSpacing(13);

    // 创建统一样式按钮的辅助 lambda
    auto makeBtn = [this](const QString &text, const QString &color) {
        QPushButton *btn = new QPushButton(text, this);
        btn->setFixedHeight(63);
        btn->setStyleSheet(QString(
            "QPushButton { background-color: %1; color: white;"
            "font-size: 22px; font-weight: bold; border-radius: 10px; }"
            "QPushButton:hover { opacity: 0.9; }"
        ).arg(color));
        return btn;
    };

    QPushButton *continueBtn = makeBtn("继续游戏", "#2A9D8F");
    QPushButton *restartBtn  = makeBtn("重新开始", "#E63946");
    QPushButton *backBtn     = makeBtn("返回大厅", "#457B9D");

    layout->addWidget(continueBtn);
    layout->addWidget(restartBtn);
    layout->addWidget(backBtn);

    // 点击后发射信号，由 GameWindow 处理具体逻辑
    connect(continueBtn, &QPushButton::clicked, this, &MenuWindow::onContinueClicked);
    connect(restartBtn,  &QPushButton::clicked, this, &MenuWindow::onRestartClicked);
    connect(backBtn,     &QPushButton::clicked, this, &MenuWindow::onBackClicked);

    setStyleSheet("QMainWindow { background-color: #F1FAEE; }");
}

MenuWindow::~MenuWindow() {}

void MenuWindow::onContinueClicked()
{
    emit continueGame();
    close();
}

void MenuWindow::onRestartClicked()
{
    emit restartGame();
    close();
}

void MenuWindow::onBackClicked()
{
    emit backToLobby();
    close();
}
