#ifndef ARSC_H
#define ARSC_H

#include <QtWidgets/QMainWindow>
#include "ui_arsc.h"

class arsc : public QMainWindow
{
    Q_OBJECT

public:
    arsc(QWidget *parent = 0);
    ~arsc();

protected slots:
    void open();
private:
    Ui::arscClass ui;
};

#endif // ARSC_H
