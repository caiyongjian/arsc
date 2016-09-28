#ifndef MYAPP_H
#define MYAPP_H

#include <QApplication>

#include "Manager.h"

class MyApp : public QApplication
{
	Q_OBJECT

public:
	MyApp(int argc, char *argv[]);
	~MyApp();

};

MyApp& getMyApp();
MyApp* getMyAppPtr();

#endif // MYAPP_H
