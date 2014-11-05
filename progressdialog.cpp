#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include "progressdialog.h"
#include "constants.h"

ProgressDialog::ProgressDialog(QWidget *parent, Extractor *extractor)
	: QDialog(parent), m_extractor(extractor)
{
	setupUi();
	connect(extractor, SIGNAL(fileListLoadingStarted()), SLOT(onFileListLoadingStarted()));
	connect(extractor, SIGNAL(extractionStarted(int)), SLOT(onExtractionStarted(int)));
	connect(extractor, SIGNAL(currentPathChanged(const QString &)), SLOT(onCurrentPathChanged(const QString &)));
	connect(extractor, SIGNAL(finished()), SLOT(onFinished()));
	connect(extractor, SIGNAL(noFilesError()), SLOT(onNoFilesError()));
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::setupUi()
{
	m_mainStatusLabel = new QLabel(tr("Starting..."));
	m_currentPathLabel = new QLabel();

	m_closeButton = new QPushButton(tr("&Close"));
	connect(m_closeButton, SIGNAL(clicked()), SLOT(close()));

	m_stopButton = new QPushButton(tr("&Stop"));
	connect(m_stopButton, SIGNAL(clicked()), SLOT(stop()));

	QDialogButtonBox *buttonBox = new QDialogButtonBox();
	buttonBox->addButton(m_stopButton, QDialogButtonBox::RejectRole);
	buttonBox->addButton(m_closeButton, QDialogButtonBox::RejectRole);
	m_closeButton->setVisible(false);

	m_progressBar = new QProgressBar();
	m_progressBar->setMinimum(0);
	m_progressBar->setMaximum(0);
	m_progressBar->setFormat(tr("%v of %m"));
	m_progressBar->setTextVisible(false);
	connect(m_extractor, SIGNAL(progress(int)), SLOT(setProgress(int)));

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(m_mainStatusLabel);
	mainLayout->addWidget(m_progressBar);
	mainLayout->addWidget(m_currentPathLabel);
	mainLayout->addStretch();
	mainLayout->addWidget(buttonBox);

	setLayout(mainLayout);
	setWindowTitle(tr("Audio Feature Extractor"));
	resize(QSize(450, 200));
}

void ProgressDialog::onFileListLoadingStarted()
{
	m_progressBar->setTextVisible(false);
	m_progressBar->setMaximum(0);
	m_progressBar->setValue(0);
	m_mainStatusLabel->setText(tr("Collecting files..."));
}

void ProgressDialog::onExtractionStarted(int count)
{
	m_count = count;
	m_progressBar->setTextVisible(true);
	m_progressBar->setMaximum(0);
	m_progressBar->setMinimum(0);
	m_progressBar->setValue(0);
	m_mainStatusLabel->setText(tr("Extracting..."));
}

void ProgressDialog::onFinished()
{
	QString result = QString("Submitted %1 feature file(s), thank you!").arg(m_extractor->submittedExtractions());
	if (m_extractor->numNoMbid() > 0) {
		result += QString("\n%1 file(s) had no MBID and were skipped").arg(m_extractor->numNoMbid());
	}
	if (m_extractor->hasErrors()) {
		result += QString("\nIgnored %1 files(s) because of errors").arg(m_extractor->numErrors());
	}
	m_mainStatusLabel->setText(result);
	m_progressBar->setVisible(false);
	m_currentPathLabel->setVisible(false);
	m_closeButton->setVisible(true);
	m_stopButton->setVisible(false);
}

void ProgressDialog::onCurrentPathChanged(const QString &path)
{
	QString elidedPath =
		m_currentPathLabel->fontMetrics().elidedText(
			path, Qt::ElideMiddle, m_currentPathLabel->width());
	m_currentPathLabel->setText(elidedPath);
}

void ProgressDialog::setProgress(int value)
{
	if (value > 0) {
		m_progressBar->setMaximum(m_count);
	}
	m_progressBar->setValue(value);
}

void ProgressDialog::stop()
{
	m_progressBar->setMaximum(m_count);
	m_extractor->cancel();
	m_stopButton->setEnabled(false);
}

void ProgressDialog::onNetworkError(const QString &message)
{
	stop();
	QMessageBox::critical(this, tr("Network Error"), message);
}

void ProgressDialog::onNoFilesError()
{
	QMessageBox::critical(this, tr("Error"),
		tr("There are no audio files in the selected folder(s)."));
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
	if (!m_extractor->isFinished()) {
		stop();
		event->ignore();
	}
	else {
		event->accept();
	}
}

