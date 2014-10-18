#include <QDebug>
#include <QFile>
#include <QTemporaryFile>
#include <QProcess>
#include "utils.h"
#include "analyzefiletask.h"
#include "constants.h"

#include <iostream>

AnalyzeFileTask::AnalyzeFileTask(const QString &path)
	: m_path(path)
{
}

void AnalyzeFileTask::run()
{
    qDebug() << "Analyzing file" << m_path;

    AnalyzeResult *result = new AnalyzeResult();
    result->fileName = m_path;

	QProcess * extractor;
	QTemporaryFile tmp;
	QStringList arguments;
	QString program = "./extractor/streaming_extractor_music";
	
	if (tmp.open()) {
		arguments<<m_path<<tmp.fileName();
		extractor = new QProcess(this);
		extractor->start(program, arguments);
		result->exitCode = extractor->exitCode();
		result->outputFileName = tmp.fileName();
		//std::count << "Result: " << result->exitCode;
		emit finished(result);
	}

	
}

