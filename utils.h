#ifndef FPSUBMIT_UTILS_H_
#define FPSUBMIT_UTILS_H_

#include <QString>
#include <QDesktopServices>

inline QString extractorPath()
{
#ifdef Q_OS_WIN32
    return "\\extractor\\streaming_extractor_music";
#else
    return "./extractor/streaming_extractor_music";
#endif
}

inline QString userAgentString()
{
	return QString("AcousticbrainzSubmitter/%1 Qt/%2").arg(VERSION).arg(qVersion());
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
