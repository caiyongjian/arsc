#pragma once
#include "ArscFile.h"

#include <qevent.h>
#include "ResultCallback.h"

class Manager: public QObject
{
	Q_OBJECT

public:
	static Manager* getInstance();

	Manager();
	~Manager();

	void addWork(const QString& path, ResultCallback* callback);

	void customEvent(QEvent *);
private:
	QMap<QString, QList<ResultCallback*>> mTasks;
};

