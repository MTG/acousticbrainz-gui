#ifndef FPSUBMIT_MAINWINDOW_H_
#define FPSUBMIT_MAINWINDOW_H_

#include <QMainWindow>
#include <QCloseEvent>
#include <QLineEdit>
#include <QTextStream>
#include <QTemporaryFile>
#include <QTextStream>
#include "checkabledirmodel.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void openAcousticbrainzWebsite();
	void analyze();

private:
	void setupUi();
	void createProfile();
	bool validateFields(QList<QString> &directories);

	QLineEdit *m_apiKeyEdit;
	CheckableDirModel *m_directoryModel;
	QTemporaryFile *m_profile;
};

#endif
