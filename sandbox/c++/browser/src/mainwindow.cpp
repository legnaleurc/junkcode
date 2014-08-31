#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtNetwork/QNetworkReply>
#include <QtWebKitWidgets/QWebFrame>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QTextCodec>
#include <QtNetwork/QNetworkDiskCache>
#include <QtWebKit/QWebSecurityOrigin>
#include <QtWebKitWidgets/QWebInspector>

#include "robot.h"
#include "networkaccessmanagerproxy.h"

QString fromResource(const QString & qrc) {
	QFile fin(":" + qrc);
	if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return "";
	}
	QTextStream rin(&fin);
	rin.setCodec(QTextCodec::codecForName("UTF-8"));
	return rin.readAll();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow),
	_ctrl(new Controller(this))
{
	ui->setupUi(this);

    this->connect(this->ui->action_Start, SIGNAL(triggered()), SLOT(_onActionStart()));
    this->connect(this->ui->actionStop, SIGNAL(triggered()), SLOT(_onActionStop()));

	auto settings = this->ui->webView->settings();
	settings->setAttribute(QWebSettings::PluginsEnabled, true);
	settings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
	settings->setAttribute(QWebSettings::JavascriptEnabled, true);
	QWebSettings::enablePersistentStorage("/tmp/browser");

	auto nam = new NetworkAccessManagerProxy(nullptr);
	auto cache = new QNetworkDiskCache;
	cache->setCacheDirectory("/tmp/browser");
    cache->setMaximumCacheSize(1024 * 1024 * 1024);
	nam->setCache(cache);
	this->_ctrl->setNetworkAccessManager(nam);
	this->_ctrl->setMouseArea(this->ui->webView);
	this->ui->webView->page()->setNetworkAccessManager(nam);

    auto mainFrame = this->ui->webView->page()->mainFrame();
    this->connect(mainFrame, SIGNAL(javaScriptWindowObjectCleared()), SLOT(_onMainFrameObjectCleared()));

//    auto inspector = new QWebInspector(this);
//    inspector->setWindowFlags(Qt::Window);
//    inspector->setPage(this->ui->webView->page());
//    inspector->show();

	this->ui->webView->load(QUrl("http://www.dmm.com/netgame/social/-/gadgets/=/app_id=854854/"));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::_onMainFrameObjectCleared() {
	auto mainFrame = this->ui->webView->page()->mainFrame();
	auto url = mainFrame->url();

	if (url.toString() == "https://www.dmm.com/my/-/login/=/path=Sg9VTQFXDFcXFl5bWlcKGExKUVdUXgFNEU0KSVMVR28MBQ0BUwJZBwxK/") {
		auto script = fromResource("/res/login.js");
		auto result = mainFrame->evaluateJavaScript(script);
		qDebug() << "cleared" << result;
		return;
	}

	if (url.toString() == "http://www.dmm.com/netgame/social/-/gadgets/=/app_id=854854/") {
		auto script = fromResource("/res/outer.js");
		auto result = mainFrame->evaluateJavaScript(script);
		qDebug() << "cleared" << result;
		return;
	}

	if (url.host() == "osapi.dmm.com") {
		auto script = fromResource("/res/inner.js");
		auto result = mainFrame->evaluateJavaScript(script);
		qDebug() << "cleared" << result;
		return;
	}
}

void MainWindow::_onActionStart() {
    this->_ctrl->start();
}

void MainWindow::_onActionStop() {
    this->_ctrl->stop();
}
