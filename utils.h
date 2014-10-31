#ifndef FPSUBMIT_UTILS_H_
#define FPSUBMIT_UTILS_H_

#include <QString>
#include <QDesktopServices>
#include <QStringList>
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

QString findExecutable(const QString &executableName, const QStringList &paths = QStringList());

inline QString extractorPath()
{
#if Q_OS_WIN
    QString extractor_name = "streaming_extractor_music.exe";
#else
    QString extractor_name = "streaming_extractor_music";
#endif
    QString path = findExecutable(extractor_name);
    if (path.isEmpty()) {
        QString execdir = QFileInfo( QCoreApplication::applicationFilePath() ).absoluteDir().absolutePath();
        path = findExecutable(extractor_name, QStringList(execdir));
    }
    return path;
}

inline QString userAgentString()
{
	return QString("AcousticBrainzSubmitter/%1 Qt/%2").arg(VERSION).arg(qVersion());
}

inline QString cacheFileName()
{
	return QDesktopServices::storageLocation(QDesktopServices::CacheLocation) + "/submitted.log";
}

inline QString extractExtension(const QString &fileName)
{
	int pos = fileName.lastIndexOf('.');
	if (pos == -1) {
		return "";
	}
	return fileName.mid(pos + 1).toUpper();
}

#endif
