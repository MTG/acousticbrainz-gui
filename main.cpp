#include <QApplication>
#include "mainwindow.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("AcousticBrainz");
	app.setOrganizationDomain("acousticbrainz.org");
	app.setApplicationName("Submitter");
	app.setApplicationVersion(VERSION);
	MainWindow window;
	window.show();
	return app.exec();
}
