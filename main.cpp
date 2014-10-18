#include <QApplication>
#include "mainwindow.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("Acoustid");
	app.setOrganizationDomain("acoustid.org");
	app.setApplicationName("Fingerprinter");
	app.setApplicationVersion(VERSION);
	MainWindow window;
	window.show();
	return app.exec();
}
