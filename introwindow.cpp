/**
 * @brief 游戏说明窗口实现
 */

#include "introwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

IntroWindow::IntroWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(1000, 750);
    setWindowTitle("游戏说明");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setSpacing(11);
    layout->setContentsMargins(40, 26, 40, 20);

    // 标题
    QLabel *title = new QLabel("蟹堡王烹饪挑战 - 游戏说明", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 36px; font-weight: bold; color: #E63946;");
    layout->addWidget(title);

    // 说明正文
    QLabel *text = new QLabel(this);
    text->setWordWrap(true);
    text->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    text->setStyleSheet(
        "font-size: 25px; color: #1D3557; line-height: 1.5;"
        "font-family: 'Microsoft YaHei', sans-serif;"
    );
    text->setText(
        "【游戏目标】\n"
        "在 60 秒内尽可能多地完成蟹黄堡订单！\n\n"
        "【如何制作】\n"
        "① 点击【底面包】【生肉饼】【顶面包】加入盘子\n"
        "② 点击【煎制】将生肉饼变成肉饼（必须先有生肉饼）\n"
        "③ 组合成「底面包 + 肉饼 + 顶面包」后点击【提交】\n"
        "④ 做错可点【丢弃】清空盘子重新制作\n\n"
        "【得分规则】\n"
        "正确提交：+10 分\n"
        "错误提交：-5 分，连击清零\n"
        "连续成功 3 次额外 +5 分，5 次额外 +10 分\n\n"
        "【星级评定】\n"
        "完成 1 单 = 一星  |  3 单 = 二星  |  5 单 = 三星"
    );
    layout->addWidget(text, 1);

    // 关闭按钮
    QPushButton *closeBtn = new QPushButton("知道了", this);
    closeBtn->setFixedHeight(55);
    closeBtn->setStyleSheet(
        "QPushButton { background-color: #E63946; color: white;"
        "font-size: 26px; font-weight: bold; border-radius: 10px;"
        "border: 2px solid rgba(255,255,255,180);"
        "font-family: 'Microsoft YaHei', sans-serif; }"
        "QPushButton:hover { background-color: #C1121F; }"
    );
    layout->addWidget(closeBtn);

    connect(closeBtn, &QPushButton::clicked, this, &IntroWindow::onCloseClicked);
    setStyleSheet("QMainWindow { background-color: #F1FAEE; }");
}

IntroWindow::~IntroWindow() {}

void IntroWindow::onCloseClicked()
{
    close();
}
