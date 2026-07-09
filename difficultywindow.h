#ifndef DIFFICULTYWINDOW_H
#define DIFFICULTYWINDOW_H
#include <QPushButton>
#include <QMainWindow>
class DifficultyWindow:public QMainWindow
{
    Q_OBJECT
public:
    DifficultyWindow(QWidget *parent = nullptr);
    ~DifficultyWindow();
private slots:
    void onSimpleClicked();
    void onDifficultClicked();
    void onBackClicked();
private:
    QPushButton *simpleBtn;
    QPushButton *difficultBtn;
    QPushButton *backBtn;
};

#endif // DIFFICULTYWINDOW_H
