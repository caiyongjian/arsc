#include "arsc.h"

#include <QFileDialog>
#include <QMessageBox>


#include "ArscFile.h"

arsc::arsc(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    connect(ui.actionOpen, SIGNAL(triggered(bool)), this, SLOT(open()));
}

arsc::~arsc()
{

}

void arsc::open()
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("open file");
    fileDialog->setDirectory(".");

    QDir::Filters filter = QDir::Filter::Files;
    fileDialog->setFilter(filter);

    QStringList filters;
    filters << "Arsc Files (*.arsc)"
            << "Any Files (*)";
    fileDialog->setNameFilters(filters);

    if (fileDialog->exec() == QDialog::Accepted) {
        QString path = fileDialog->selectedFiles()[0];
        ArscFile sf(path);
        sf.parseFile();
//        WorkThread *localWorkThread = new WorkThread();
//        localWorkThread->start();
//        QMessageBox::information(NULL, tr("path"), tr("you selected ") + path);
    } else {
        QMessageBox::information(NULL, tr("path"), tr("you didn't select files."));
    }
}
