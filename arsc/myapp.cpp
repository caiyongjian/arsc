#include "myapp.h"

MyApp* g_myApp = Q_NULLPTR;
MyApp& getMyApp() {
	return *g_myApp;
}

MyApp* getMyAppPtr() {
	return g_myApp;
}

MyApp::MyApp(int argc, char *argv[])
	: QApplication(argc, argv)
{
	g_myApp = this;
}

MyApp::~MyApp()
{

}