#include "controller.h"

#include <QtCore/QJsonObject>
#include <QtGui/QPixmap>

Controller::Controller(QObject *parent) :
	QObject(parent),
	_db(),
	_namp(nullptr),
	_eventQueue(new EventQueue(this)),
	_cb()
{
	this->_cb.insert(std::make_pair(QString("/kcsapi/api_start2"), std::bind(&Controller::_api_start2, this, std::placeholders::_1)));
	this->_cb.insert(std::make_pair(QString("/kcsapi/api_port/port"), std::bind(&Controller::_api_port_port, this, std::placeholders::_1)));
}

Controller::~Controller() {
//	this->_mouse->deleteLater();
//	this->_thread->deleteLater();
}

void Controller::setNetworkAccessManager(NetworkAccessManagerProxy *namp) {
	this->_namp = namp;
	this->connect(namp, SIGNAL(requestFinished(QString,QJsonDocument)), SLOT(_onRequestFinished(QString,QJsonDocument)));
}

void Controller::setMouseArea(QWidget *ma) {
	this->_eventQueue->setMouseArea(ma);
}

void Controller::start() {
    this->_eventQueue->waitForStarted();
    qDebug() << "started";
    this->_eventQueue->startMission(0, 0);
}

void Controller::stop() {
    this->_eventQueue->waitForStopped();
    qDebug() << "stopped";
}

void Controller::_onRequestFinished(const QString &path, const QJsonDocument &json) {
	auto it = this->_cb.find(path);
	if (it == this->_cb.end()) {
		qDebug() << "no cb" << path;
		return;
	}
	it->second(json);
}

void Controller::_api_start2(const QJsonDocument & json) {
	if (!json.isObject()) {
		qDebug() << "api_start2 ??";
		return;
	}
	auto root = json.object();
	auto api_result = root.value("api_result").toInt();
	if (api_result != 1) {
		auto api_result_msg = root.value("api_result_msg").toString();
		qDebug() << api_result << api_result_msg;
		return;
	}
	auto api_data = root.value("api_data").toObject();
	this->_db.createShipType(api_data.value("api_mst_ship"));
}

void Controller::_api_port_port(const QJsonDocument & json) {
	if (!json.isObject()) {
		qDebug() << "api_port/port ??";
		return;
	}
	auto root = json.object();
	auto api_result = root.value("api_result").toInt();
	if (api_result != 1) {
		auto api_result_msg = root.value("api_result_msg").toString();
		qDebug() << api_result << api_result_msg;
		return;
	}
	auto api_data = root.value("api_data").toObject();
	auto api_deck_port = api_data.value("api_deck_port");
	auto api_ndock = api_data.value("api_ndock");
	auto api_ship = api_data.value("api_ship");
	this->_db.createShip(api_ship);
	this->_db.createDeck(api_deck_port);
	this->_db.createNDock(api_ndock);

//	emit this->api_port_port();
//	this->start();
}
