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
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "loadfilelisttask.h"
#include "analyzefiletask.h"
#include "updatelogfiletask.h"
#include "extractor.h"
#include "constants.h"
#include "utils.h"
#include "gzip.h"


class NetworkProxyFactory : public QNetworkProxyFactory
{
public:
	NetworkProxyFactory() : m_httpProxy(QNetworkProxy::NoProxy)
	{
		char* httpProxyUrl = getenv("http_proxy");
		if (httpProxyUrl) {
			QUrl url = QUrl::fromEncoded(QByteArray(httpProxyUrl));
			if (url.isValid() && !url.host().isEmpty()) {
				m_httpProxy = QNetworkProxy(QNetworkProxy::HttpProxy, url.host(), url.port(80));
				if (!url.userName().isEmpty())  {
					m_httpProxy.setUser(url.userName());
					if (!url.password().isEmpty()) {
						m_httpProxy.setPassword(url.password());
					}
				}
			}
		}
	}

	QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery& query)
	{
		QList<QNetworkProxy> proxies = QNetworkProxyFactory::systemProxyForQuery(query);
		QString protocol = query.protocolTag().toLower();
		if (m_httpProxy.type() != QNetworkProxy::NoProxy && protocol == QLatin1String("http")) {
			QMutableListIterator<QNetworkProxy> i(proxies);
			while (i.hasNext()) {
				QNetworkProxy& proxy = i.next();
				if (proxy.type() == QNetworkProxy::NoProxy) {
					i.remove();
				}
			}
			proxies.append(m_httpProxy);
			proxies.append(QNetworkProxy::NoProxy);
		}
		return proxies;
	}

private:
	QNetworkProxy m_httpProxy;
};

Extractor::Extractor(const QStringList &directories, QTemporaryFile *profile)
	: m_profile(profile), m_directories(directories), m_cancelled(false),
	  m_finished(false), m_reply(0), m_activeFiles(0), m_extractedFiles(0), m_submittedFiles(0), m_numErrors(0), m_numNoMbid(0)
{
	m_networkAccessManager = new QNetworkAccessManager(this);
	m_networkAccessManager->setProxyFactory(new NetworkProxyFactory());
	connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply *)), SLOT(onRequestFinished(QNetworkReply*)));

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

void Extractor::cancel()
{
	m_cancelled = true;
	m_files.clear();
	m_submitQueue.clear();
	if (m_reply) {
		m_reply->abort();
	}
	for (int i = 0; i < m_activeProcesses.size(); i++) {
        AnalyzeFileTask *task = m_activeProcesses[i];
		task->terminate();
        delete task;
	}
	m_activeProcesses.clear();
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
	return !isCancelled() && !isFinished();
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
	while (!m_files.isEmpty() && m_activeFiles < QThread::idealThreadCount()) {
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
	AnalyzeFileTask *task = new AnalyzeFileTask(path, m_profile->fileName());
	m_activeProcesses.append(task);
	connect(task, SIGNAL(finished(AnalyzeResult *)), SLOT(onFileAnalyzed(AnalyzeResult *)), Qt::QueuedConnection);
	task->doanalyze();
}

void Extractor::onFileAnalyzed(AnalyzeResult *result)
{
	qDebug() << result->exitCode;
	qDebug() << result->fileName << " -> " << result->outputFileName;
	m_activeFiles--;
	emit progress(++m_extractedFiles);
	if (!result->error) {
		if (!isCancelled()) {
			m_submitQueue.append(result);
		}
		if (isRunning()) {
			maybeSubmit();
		}
	}
	else {
		qDebug() << "Error " << result->errorMessage << "while processing " << result->fileName;
		if (result->exitCode == 2) {
			m_numNoMbid++;
		} else {
			m_numErrors++;
		}
	}
	AnalyzeFileTask *task = result->m_task;
	bool removed = m_activeProcesses.removeOne(task);
	if (removed) {
		delete task;
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
		maybeSubmit();
	}
}


bool Extractor::maybeSubmit()
{
	int size = m_submitQueue.size();
	qDebug() << "got" << size << "to submit, going to send 1";
	if (!m_reply && size > 0) {
		qDebug() << "Submitting 1 code";
		AnalyzeResult *result = m_submitQueue.takeFirst();

		QString filename = result->outputFileName;
		QFile thejson(filename);
		if (thejson.open(QIODevice::ReadOnly)) {
			QByteArray jsonContents = thejson.readAll();

			QJsonDocument d = QJsonDocument::fromJson(jsonContents);
			if (d.isEmpty()) {
				return false;
			}

			QJsonObject doc = d.object();
			if (doc.empty()) {
				return false;
			}
			QJsonObject metadata = doc.value(QString("metadata")).toObject();
			if (metadata.empty()) {
				return false;
			}
			QJsonObject tags = metadata.value(QString("tags")).toObject();
			if (tags.empty()) {
				return false;
			}
			QJsonArray mbids = tags.value("musicbrainz_trackid").toArray();
			QString uuid;
			if (mbids.size()) {
				uuid = mbids[0].toString();
			}

			m_submitting.append(result->fileName);

			QString submit = QString(SUBMIT_URL).arg(uuid);
			int bytes = jsonContents.length();
			qDebug() << "Submitting " << bytes << " b to " << submit;
			QNetworkRequest request = QNetworkRequest(QUrl(submit));
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
			request.setHeader(QNetworkRequest::UserAgentHeader, userAgentString());
			m_reply = m_networkAccessManager->post(request, jsonContents);
		} else {
			qDebug() << "Failed to open source file";
		}

		delete result;
	} else if (m_reply && size > 0) {
        qDebug() << "Other query in progress, skipping for now";
    } else if (size == 0) {
        qDebug() << "No files to submit";
    }

	return true;
}

void Extractor::onRequestFinished(QNetworkReply *reply)
{
    bool stop = false;
    QNetworkReply::NetworkError error = reply->error();

    if (m_cancelled) {
        stop = true;
    }
    else if (error != QNetworkReply::NoError) {
        qWarning() << "Submission failed with network error" << error;
        qWarning() << reply->errorString();
        stop = true;
    }

    if (!stop && error == QNetworkReply::NoError) {
        m_submitted.append(m_submitting);
        m_submittedFiles += m_submitting.size();
        qDebug() << "Submission finished";
    }

    if (m_submitted.size() > 0) {
        UpdateLogFileTask *task = new UpdateLogFileTask(m_submitted);
        task->setAutoDelete(true);
        QThreadPool::globalInstance()->start(task);
        m_submitted.clear();
    }

    m_submitting.clear();
    reply->deleteLater();
    m_reply = 0;

    if (m_submitQueue.isEmpty() && m_files.isEmpty() && m_activeFiles == 0) {
        m_finished = true;
        emit finished();
        return;
    }

    if (isRunning() || !m_submitQueue.isEmpty()) {
        if (!isRunning()) {
            qDebug() << "Finished running, but queue is not empty";
        }
        qDebug() << "Request finished, submitting again";
        maybeSubmit();
    }
}

