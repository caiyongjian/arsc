#include "Manager.h"

#include <qthread.h>
#include <qrunnable.h>
#include <qthreadpool.h>

#include "myapp.h"

Manager* g_instance = new Manager();

Manager* Manager::getInstance() {
	return g_instance;
}

Manager::Manager()
{
}


Manager::~Manager()
{
}

namespace {
	QEvent::Type myEventType = (QEvent::Type)QEvent::registerEventType();

	class MyEvent : public QEvent {
	public:
		MyEvent(QString path, ArscFile* target)
			: QEvent(myEventType)
			, mPath(path)
			, mTarget(target)
		{
		}

		ArscFile* getTarget() {
			return mTarget;
		}

		QString getPath() {
			return mPath;
		}

	private:
		QString mPath;
		ArscFile* mTarget;
	};

	class ParseTask : public QRunnable {
	public:
		ParseTask(const QString &path)
			: mPath(path)
		{
		}

		void run() {
			ArscFile* arscFile = new ArscFile(mPath);
			if (arscFile->parseFile()) {
			}
			else {
				delete arscFile;
				arscFile = Q_NULLPTR;
			}

			MyEvent* pEvent = new MyEvent(mPath, arscFile);
			QApplication::postEvent(Manager::getInstance(), pEvent);
		}

	private:
		QString mPath;
	};
}

void Manager::addWork(const QString& path, ResultCallback* callback) {
	QMap<QString, QList<ResultCallback*>>::iterator target = mTasks.find(path);
	if (target != mTasks.end()) {
		target->append(callback);
		return;
	}

	QList<ResultCallback*> callbacks;
	callbacks.append(callback);
	mTasks.insert(path, callbacks);

	QThreadPool::globalInstance()->start(new ParseTask(path));
}


void Manager::customEvent(QEvent * event) {
	if (event->type() == myEventType) {
		MyEvent* pEvent = (MyEvent*)event;
		QString path = pEvent->getPath();
		QList<ResultCallback*> callbacks = mTasks[path];
		for (ResultCallback* callback : callbacks) {
			callback->onResult(pEvent->getTarget());
		}
		mTasks.remove(path);
		event->accept();
	}
}