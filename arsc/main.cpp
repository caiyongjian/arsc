#include "arsc.h"
#include <QtWidgets/QApplication>

#include "myapp.h"

int main(int argc, char *argv[])
{
	MyApp a(argc, argv);
    arsc w;
    w.show();
    return a.exec();
}
