#ifndef FPSUBMIT_ANALYZEFILETASK_H_
#define FPSUBMIT_ANALYZEFILETASK_H_

#include <QRunnable>
#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QTemporaryFile>

struct AnalyzeResult
{
    AnalyzeResult() : error(false)
    {
    }

    QString fileName;
	QString outputFileName;
	int exitCode;
    bool error;
    QString errorMessage;
};

class AnalyzeFileTask : public QObject
{
	Q_OBJECT

public:
	AnalyzeFileTask(const QString &path);
	void doanalyze();
    void terminate();

signals:
	void finished(AnalyzeResult *result);

private slots:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void error(QProcess::ProcessError);

private:
	QString m_path;

	QProcess* extractor;
	QTemporaryFile* tmp;
    AnalyzeResult *result;
};

#endif
