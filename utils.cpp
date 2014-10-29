#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

// findExecutable from QT5
#ifdef Q_OS_WIN
static QStringList executableExtensions()
{
    QStringList ret = QString::fromLocal8Bit(qgetenv("PATHEXT")).split(QLatin1Char(';'));
    if (!ret.contains(QLatin1String(".exe"), Qt::CaseInsensitive)) {
        // If %PATHEXT% does not contain .exe, it is either empty, malformed, or distorted in ways that we cannot support, anyway.
        ret.clear();
        ret << QLatin1String(".exe")
            << QLatin1String(".com")
            << QLatin1String(".bat")
            << QLatin1String(".cmd");
    }
    return ret;
}
#endif
static QString checkExecutable(const QString &path)
{
    const QFileInfo info(path);
    if (info.isBundle())
        return info.bundleName();
    if (info.isFile() && info.isExecutable())
        return QDir::cleanPath(path);
    return QString();
}
QString findExecutable(const QString &executableName, const QStringList &paths)
{
    QStringList searchPaths = paths;
    if (paths.isEmpty()) {
        QByteArray pEnv = qgetenv("PATH");
#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
        const QLatin1Char pathSep(';');
#else
        const QLatin1Char pathSep(':');
#endif
        searchPaths = QString::fromLocal8Bit(pEnv.constData()).split(pathSep, QString::SkipEmptyParts);
    }
    if (QFileInfo(executableName).isAbsolute())
        return checkExecutable(executableName);
    QDir currentDir = QDir::current();
    QString absPath;
#ifdef Q_OS_WIN
    static QStringList executable_extensions = executableExtensions();
#endif
    for (QStringList::const_iterator p = searchPaths.constBegin(); p != searchPaths.constEnd(); ++p) {
        const QString candidate = currentDir.absoluteFilePath(*p + QLatin1Char('/') + executableName);
#ifdef Q_OS_WIN
    const QString extension = QLatin1Char('.') + QFileInfo(executableName).suffix();
    if (!executable_extensions.contains(extension, Qt::CaseInsensitive)) {
        foreach (const QString &extension, executable_extensions) {
            absPath = checkExecutable(candidate + extension.toLower());
            if (!absPath.isEmpty())
                break;
        }
    }
#endif
    absPath = checkExecutable(candidate);
    if (!absPath.isEmpty()) {
        break;
    }
}

return absPath;

}
