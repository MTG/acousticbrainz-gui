#include <QDebug>
#include <QFile>
#include "utils.h"
#include "analyzefiletask.h"
#include "constants.h"

#include <iostream>

AnalyzeFileTask::AnalyzeFileTask(const QString &path)
	: m_path(path)
{
}

void AnalyzeFileTask::doanalyze()
{
    qDebug() << "Analyzing file" << m_path;

    result = new AnalyzeResult();
    result->fileName = m_path;

	QStringList arguments;
	QString program = "./extractor/streaming_extractor_music";
    tmp = new QTemporaryFile();
	if (tmp->open()) {
		arguments<<m_path<<tmp->fileName();
		extractor = new QProcess(this);
        connect(extractor, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(processFinished(int, QProcess::ExitStatus)));
        connect(extractor, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)));
		extractor->start(program, arguments);
	}
}

void AnalyzeFileTask::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    result->exitCode = extractor->exitCode();
    result->outputFileName = tmp->fileName();
    std::cout << "Result: " << result->exitCode;
    emit finished(result);
}

void AnalyzeFileTask::error(QProcess::ProcessError error) {
    qDebug() << "Error running " << error;
    result->error = true;
    emit finished(result);
}

void AnalyzeFileTask::terminate() {
    if (extractor) {
        extractor->terminate();
    }
}

