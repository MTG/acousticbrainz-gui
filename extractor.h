#ifndef FPSUBMIT_EXTRACTOR_H_
#define FPSUBMIT_EXTRACTOR_H_

#include <QObject>
#include <QDir>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QTime>

class AnalyzeResult;
class AnalyzeFileTask;
class QNetworkReply;

class Extractor : public QObject
{
    Q_OBJECT

public:
    Extractor(const QStringList &directories);
    ~Extractor();

	bool isPaused();
	bool isCancelled();
	bool isRunning();
	bool isFinished();
    bool hasErrors();

	int submittedExtractions() const { return m_submittedFiles; }
    int numErrors() const { return m_numErrors; }

signals:
    void statusChanged(const QString &message);
    void currentPathChanged(const QString &path);
	void fileListLoadingStarted();
	void extractionStarted(int fileCount);
	void progress(int i);
	void finished();
	void noFilesError();

public slots:
    void start();
    void pause();
    void resume();
    void cancel();

private slots:
	void onFileListLoaded(const QStringList &files);
	void onFileAnalyzed(AnalyzeResult *);
	//void onRequestFinished(QNetworkReply *reply);

private:
	void extractNextFile();
	//bool maybeSubmit(bool force=false);

    QString m_apiKey;
    QStringList m_files;
    QStringList m_directories;
	QNetworkAccessManager *m_networkAccessManager;
	QList<AnalyzeResult *> m_submitQueue;
	QStringList m_submitting;
	QStringList m_submitted;
	QNetworkReply *m_reply;
    QList<AnalyzeFileTask *> m_activeProcesses;

	QTime m_time;
	int m_extractedFiles;
	int m_submittedFiles;
	int m_activeFiles;
	bool m_cancelled;
	bool m_paused;
	bool m_finished;
    int m_numErrors;
};

#endif
