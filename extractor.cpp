#include <QDebug>
#include <QDir>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QDesktopServices>
#include <QMutexLocker>
#include <QThreadPool>
#include "loadfilelisttask.h"
#include "analyzefiletask.h"
#include "updatelogfiletask.h"
#include "extractor.h"
#include "constants.h"
#include "utils.h"
#include "gzip.h"


Extractor::Extractor(const QStringList &directories)
	: m_directories(directories), m_paused(false), m_cancelled(false),
	  m_finished(false), m_reply(0), m_activeFiles(0), m_extractedFiles(0), m_submittedFiles(0)
{

}

Extractor::~Extractor()
{
}

void Extractor::start()
{
	m_time.start();
	LoadFileListTask *task = new LoadFileListTask(m_directories);
	connect(task, SIGNAL(finished(const QStringList &)), SLOT(onFileListLoaded(const QStringList &)), Qt::QueuedConnection);
	connect(task, SIGNAL(currentPathChanged(const QString &)), SIGNAL(currentPathChanged(const QString &)), Qt::QueuedConnection);
	task->setAutoDelete(true);
	emit fileListLoadingStarted();
	QThreadPool::globalInstance()->start(task);
}

void Extractor::pause()
{
	m_paused = true;
}

void Extractor::resume()
{
	m_paused = false;
	while (!m_files.isEmpty() && m_activeFiles < MAX_ACTIVE_FILES) {
		extractNextFile();
	}
	//maybeSubmit();
}

void Extractor::cancel()
{
	m_cancelled = true;
	m_files.clear();
	m_submitQueue.clear();
	if (m_reply) {
		m_reply->abort();
	}
	for (int i = 0; i < m_activeProcesses.size(); i++) {
        qDebug() << "process " << i << " terminating";
		m_activeProcesses[i]->terminate();
	}
}

bool Extractor::isPaused()
{
	return m_paused;
}

bool Extractor::hasErrors()
{
	return m_numErrors > 0;
}

bool Extractor::isCancelled()
{
	return m_cancelled;
}

bool Extractor::isFinished()
{
	return m_finished;
}

bool Extractor::isRunning()
{
	return !isPaused() && !isCancelled() && !isFinished();
}

void Extractor::onFileListLoaded(const QStringList &files)
{
	m_files = files;
	if (m_files.isEmpty()) {
		m_finished = true;
		emit noFilesError();
		emit finished();
		return;
	}
	emit extractionStarted(files.size());
	while (!m_files.isEmpty() && m_activeFiles < MAX_ACTIVE_FILES) {
		extractNextFile();
	}
}

void Extractor::extractNextFile()
{
	if (m_files.isEmpty()) {
		return;
	}
	m_activeFiles++;
	QString path = m_files.takeFirst();
	emit currentPathChanged(path);
	AnalyzeFileTask *task = new AnalyzeFileTask(path);
    // TODO: How to remove this from the list when done
    m_activeProcesses.append(task);
	connect(task, SIGNAL(finished(AnalyzeResult *)), SLOT(onFileAnalyzed(AnalyzeResult *)), Qt::QueuedConnection);
	task->doanalyze();
}

void Extractor::onFileAnalyzed(AnalyzeResult *result)
{
	qDebug() << result->exitCode;
	qDebug() << result->outputFileName;
	m_activeFiles--;
	emit progress(++m_extractedFiles);
	if (!result->error) {
		/*if (!isCancelled()) {
			m_submitQueue.append(result);
		}
		if (isRunning()) {
			maybeSubmit();
		}*/
	}
	else {
		qDebug() << "Error" << result->errorMessage << "while processing" << result->fileName;
		m_numErrors++;
	}
	if (isRunning()) {
		extractNextFile();
	}
	if (m_activeFiles == 0 && m_files.isEmpty()) {
		if (m_submitQueue.isEmpty()) {
			m_finished = true;
			emit finished();
			return;
		}
		//maybeSubmit(true);
	}
}

