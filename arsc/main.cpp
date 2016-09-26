#include "arsc.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    arsc w;
    w.show();
    return a.exec();
}
