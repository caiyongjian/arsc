#include "arsc.h"

#include <QFileDialog>
#include <QMessageBox>


#include "ArscFile.h"
#include "myapp.h"

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
		Manager::getInstance()->addWork(path, this);
    } else {
        QMessageBox::information(NULL, tr("path"), tr("you didn't select files."));
    }
}

void arsc::onResult(ArscFile* result) {
	if (result == Q_NULLPTR)
	{
		QMessageBox::information(NULL, tr("path"), tr("parse error!!!"));
		return;
	}
	QMessageBox::information(NULL, tr("path"), tr("parse success!!!"));

}

