/**
 * @brief 程序入口：创建 Qt 应用并显示主菜单窗口
 */

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // 创建 Qt 应用程序对象（管理整个程序的事件循环）
    QApplication a(argc, argv);

    // 创建并显示主菜单窗口
    MainWindow w;
    w.show();

    // 进入事件循环，等待用户操作
    return a.exec();
}
