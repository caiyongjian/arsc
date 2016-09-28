#ifndef ARSC_H
#define ARSC_H

#include <QtWidgets/QMainWindow>
#include "ui_arsc.h"
#include "ResultCallback.h"

class arsc : public QMainWindow, public ResultCallback
{
    Q_OBJECT

public:
    arsc(QWidget *parent = 0);
    ~arsc();
	void onResult(ArscFile* result);
protected slots:
    void open();
private:
    Ui::arscClass ui;
};

#endif // ARSC_H
