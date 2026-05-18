#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>

class MainPage : public QWidget
{
    Q_OBJECT

public:
    MainPage(QWidget *parent = nullptr);
    ~MainPage();

private:
    QTextEdit *value;
    QLineEdit *p;
    QLineEdit *q;
    QTextEdit *answer;
};
#endif // MAINPAGE_H
