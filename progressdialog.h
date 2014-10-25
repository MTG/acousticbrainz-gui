#ifndef FPSUBMIT_PROGRESSDIALOG_H_
#define FPSUBMIT_PROGRESSDIALOG_H_

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include "extractor.h"

class ProgressDialog : public QDialog
{
	Q_OBJECT

public:
	ProgressDialog(QWidget *parent, Extractor *extractor);
	~ProgressDialog();

public slots:
	void setProgress(int value);
	void stop();
	void onFileListLoadingStarted();
	void onExtractionStarted(int count);
	void onCurrentPathChanged(const QString &path);
	void onFinished();
	//void onNetworkError(const QString &message);
	//void onAuthenticationError();
	void onNoFilesError();

protected:
	void closeEvent(QCloseEvent *event);

private:
	void setupUi();

	Extractor *m_extractor;
	QPushButton *m_closeButton;
	QPushButton *m_stopButton;
	QProgressBar *m_progressBar;
	QLabel *m_mainStatusLabel;
	QLabel *m_currentPathLabel;
};

#endif
