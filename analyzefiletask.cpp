#include <QDebug>
#include <QFile>
#include "utils.h"
#include "analyzefiletask.h"
#include "constants.h"

#include <iostream>

AnalyzeFileTask::AnalyzeFileTask(const QString &path, const QString &profile)
	: m_path(path), m_profile(profile)
{
}

void AnalyzeFileTask::doanalyze()
{
	qDebug() << "Analyzing file" << m_path;

	result = new AnalyzeResult(this);
	result->fileName = m_path;

	QStringList arguments;
	result->m_tmpFile = new QTemporaryFile();
	if (result->m_tmpFile->open()) {
		arguments << m_path << result->m_tmpFile->fileName() << m_profile;
		extractor = new QProcess(this);
		connect(extractor, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(processFinished(int, QProcess::ExitStatus)));
		connect(extractor, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)));
		extractor->start(extractorPath(), arguments);
	}
}

void AnalyzeFileTask::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
	result->exitCode = extractor->exitCode();
	result->outputFileName = result->m_tmpFile->fileName();
	result->error = !exitCode==0;
	if (exitCode == 2) {
		result->errorMessage = "missing mbid";
	}
	emit finished(result);
}

void AnalyzeFileTask::error(QProcess::ProcessError error) {
	qDebug() << "Error running " << error;
	result->error = true;
	result->errorMessage = "unknown error";
	emit finished(result);
}

void AnalyzeFileTask::terminate() {
	if (extractor) {
		qDebug() << "in analyze task, terminating...";
		extractor->terminate();
	}
}

