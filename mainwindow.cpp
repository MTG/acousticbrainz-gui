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
#include <QSettings>
#include <QCryptographicHash>
#include "progressdialog.h"
#include "checkabledirmodel.h"
#include "extractor.h"
#include "mainwindow.h"
#include "constants.h"
#include "utils.h"

MainWindow::MainWindow()
{
	setupUi();
	createProfile();
}

void MainWindow::closeEvent(QCloseEvent *event) {
	delete m_profile;
	event->accept();
}

void MainWindow::setupUi()
{

	QTreeView *treeView = new QTreeView();

	m_directoryModel = new CheckableDirModel();
	m_directoryModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
#ifdef Q_WS_MAC
	m_directoryModel->setRootPath("/Volumes");
#else
	m_directoryModel->setRootPath("");
#endif

	treeView->setModel(m_directoryModel);
	treeView->setHeaderHidden(true);
	treeView->hideColumn(1);
	treeView->hideColumn(2);
	treeView->hideColumn(3);

//	QString homePath = QDir::homePath();
	QString homePath = QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
	const QModelIndex homePathIndex = m_directoryModel->index(homePath);
	treeView->expand(homePathIndex);
	treeView->selectionModel()->setCurrentIndex(homePathIndex, QItemSelectionModel::ClearAndSelect);
	treeView->scrollTo(homePathIndex);

	QLabel *treeViewLabel = new QLabel(tr("&Select which folders to analyze:"));
	treeViewLabel->setBuddy(treeView);

	QPushButton *analyzeButton = new QPushButton(tr("&Analyze..."));
	connect(analyzeButton, SIGNAL(clicked()), SLOT(analyze()));

	QPushButton *closeButton = new QPushButton(tr("&Close"));
	connect(closeButton, SIGNAL(clicked()), SLOT(close()));

	QDialogButtonBox *buttonBox = new QDialogButtonBox();
	buttonBox->addButton(analyzeButton, QDialogButtonBox::ActionRole);
	buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(treeViewLabel);
	mainLayout->addWidget(treeView);
	mainLayout->addWidget(buttonBox);

	QWidget *centralWidget = new QWidget();
	centralWidget->setLayout(mainLayout);
	setCentralWidget(centralWidget);
	setWindowTitle(tr("AcousticBrainz Extractor"));
	QIcon icon;
	icon.addFile(":/images/acoustid-fp-16.png", QSize(16, 16));
	icon.addFile(":/images/acoustid-fp-24.png", QSize(24, 24));
	icon.addFile(":/images/acoustid-fp-32.png", QSize(32, 32));
	icon.addFile(":/images/acoustid-fp-48.png", QSize(48, 48));
	setWindowIcon(icon);
	resize(QSize(400, 500));
}

void MainWindow::createProfile() {
	m_profile = new QTemporaryFile();
	if (m_profile->open()) {
		QCryptographicHash *hash = new QCryptographicHash(QCryptographicHash::Sha1);
		QFile app(extractorPath());
		if (app.open(QIODevice::ReadOnly )) {
			QByteArray data = app.readAll();
			hash->addData(data);
		}

		QByteArray finalHash = hash->result();
		QString hstr = finalHash.toHex();

		QTextStream out(m_profile);
		out << "requireMbid: true\n";
		out << "indent: 0\n";
		out << "mergeValues:\n    metadata:\n        version:\n            essentia_build_sha: " << hstr << "\n";
		m_profile->close();
	}
}

void MainWindow::openAcousticbrainzWebsite()
{
	QDesktopServices::openUrl(QUrl::fromPercentEncoding(API_KEY_URL));
}

void MainWindow::analyze()
{
	QList<QString> directories;
	if (!validateFields(directories)) {
		return;
	}
	Extractor *extractor = new Extractor(directories, m_profile);
	ProgressDialog *progressDialog = new ProgressDialog(this, extractor);
	extractor->start();
	progressDialog->setModal(true);
	progressDialog->show();
}

bool MainWindow::validateFields(QList<QString> &directories)
{
	directories = m_directoryModel->selectedDirectories();
	if (directories.isEmpty()) {
		QMessageBox::warning(this, tr("Error"),
			tr("Please select one or more folders with audio files to analyze."));
		return false;
	}
	return true;
}

