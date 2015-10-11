#ifndef FPSUBMIT_ANALYZEFILETASK_H_
#define FPSUBMIT_ANALYZEFILETASK_H_

#include <QRunnable>
#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QTemporaryFile>

class AnalyzeFileTask;

struct AnalyzeResult
{
	AnalyzeResult(AnalyzeFileTask *task)
	: error(false), m_task(task)
	{
		m_tmpFile = new QTemporaryFile();
	}

	QString fileName;
	QString outputFileName;
	int exitCode;
	bool error;
	bool nombid;
	QString errorMessage;
	AnalyzeFileTask *m_task;
	QTemporaryFile *m_tmpFile;

	~AnalyzeResult()
	{
		delete m_tmpFile;
	}
};

class AnalyzeFileTask : public QObject
{
	Q_OBJECT

public:
	AnalyzeFileTask(const QString &path, const QString &file);
	virtual ~AnalyzeFileTask() {
		delete extractor;
	}
	void doanalyze();
	void terminate();
	QString filePath() const { return m_path; }

	bool operator==(const AnalyzeFileTask& rhs) {
		return m_path == rhs.m_path;
	}

signals:
	void finished(AnalyzeResult *result);

private slots:
	void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
	void error(QProcess::ProcessError);

private:
	QString m_path;
	QString m_profile;

	QProcess* extractor;
	AnalyzeResult *result;
};

#endif
