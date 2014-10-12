#include "controller.h"

#include <cassert>

#include <QtCore/QJsonObject>
#include <QtGui/QPixmap>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QTextCodec>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>

Controller::Controller(QObject *parent) :
    QObject(parent),
    _db(),
    _namp(nullptr),
    _cb(),
    _robot(),
    _eventQueue(),
    _httpServer(new QTcpServer(this)),
    _api_data(),
    _api_port()
{
    this->_cb.insert(std::make_pair(QString("/gadgets/makeRequest"), std::bind(&Controller::_gadgets_make_request, this, std::placeholders::_1)));
    this->_cb.insert(std::make_pair(QString("/kcsapi/api_start2"), std::bind(&Controller::_api_start2, this, std::placeholders::_1)));
    this->_cb.insert(std::make_pair(QString("/kcsapi/api_get_member/basic"), std::bind(&Controller::_api_get_member_basic, this, std::placeholders::_1)));
    this->_cb.insert(std::make_pair(QString("/kcsapi/api_port/port"), std::bind(&Controller::_api_port_port, this, std::placeholders::_1)));
//    this->_cb.insert(std::make_pair(QString("/kcsapi/api_req_mission/start"), std::bind(&Controller::_api_req_mission_start, this, std::placeholders::_1)));

    this->connect(this->_httpServer, SIGNAL(newConnection()), SLOT(_onNewConnection()));
}

Controller::~Controller() {
}

void Controller::setNetworkAccessManager(NetworkAccessManagerProxy *namp) {
    this->_namp = namp;
    this->connect(namp, SIGNAL(requestFinished(QString,QJsonDocument)), SLOT(_onRequestFinished(QString,QJsonDocument)));
}

void Controller::setMouseArea(QWidget *ma) {
    this->_robot = Robot::create(ma);
}

void Controller::start() {
    this->_httpServer->listen(QHostAddress::LocalHost, 1080);
}

void Controller::stop() {
    this->_httpServer->close();
}

void Controller::startMission(int api_deck_id, int api_mission_id) {
    auto task = new QtCoroutine([&](const QtCoroutine::Yield & yield)->void {
        this->_charge(yield, api_deck_id);
        this->_startMission(yield, api_deck_id, api_mission_id);
    }, this);
    task->connect(task, SIGNAL(finished()), SLOT(deleteLater()));
    task->start();
}

void Controller::_onRequestFinished(const QString &path, const QJsonDocument &json) {
    auto it = this->_cb.find(path);
    if (it == this->_cb.end()) {
        qDebug() << "no cb" << path;
    } else {
        it->second(json);
    }
    emit this->responsed(path);
}

void Controller::_gadgets_make_request(const QJsonDocument & json) {
    if (!json.isObject()) {
        return;
    }
    auto root = json.object();
    auto api_result = root.value("api_result").toInt();
    if (api_result != 1) {
        auto api_result_msg = root.value("api_result_msg").toString();
        qDebug() << api_result << api_result_msg;
        return;
    }
    this->_api_data.insert("api_starttime", root.value("api_starttime").toVariant().toLongLong());
    this->_api_data.insert("api_token", root.value("api_token").toString());
}

void Controller::_api_start2(const QJsonDocument & json) {
    if (!json.isObject()) {
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
    this->_api_data.insert("api_start", api_data.toVariantMap());
}

void Controller::_api_get_member_basic(const QJsonDocument & json) {
    if (!json.isObject()) {
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
    this->_api_data.insert("api_basic", api_data.toVariantMap());
}

void Controller::_api_port_port(const QJsonDocument & json) {
    if (!json.isObject()) {
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

//    this->_db.clearCache();

    this->_db.createShip(api_ship);
    this->_db.createDeck(api_deck_port);
    this->_db.createNDock(api_ndock);
    this->_api_port = api_data.toVariantMap();
}

void Controller::_api_req_mission_start(const QJsonDocument &json) {
    if (!json.isObject()) {
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
//    auto api_complatetime = api_data.value("api_complatetime").toInt();
    qDebug() << api_data;

//    this->_db.updateMission(api_deck_id, api_mission_id, api_complatetime);
}

void Controller::_charge(const QtCoroutine::Yield &yield, int api_deck_id) {
    assert((api_deck_id > 0 && api_deck_id <= 4) || !"invalid deck");

    if (!this->_db.needCharge(api_deck_id)) {
        return;
    }

    this->_click(yield, "main_menu_button_reload");
    if (api_deck_id == 1) {
    } else if (api_deck_id == 2) {
        this->_click(yield, "select_menu_second_team");
    } else if (api_deck_id == 3) {
        this->_click(yield, "select_menu_third_team");
    } else if (api_deck_id == 4) {
        this->_click(yield, "select_menu_forth_team");
    }
    this->_click(yield, 120, 125);
    this->_click(yield, 700, 445);
    this->_click(yield, "submenu_button_back");
}

void Controller::_startMission(const QtCoroutine::Yield &yield, int api_deck_id, int api_mission_id) {
    this->_click(yield, "main_menu_button_go");
    this->_click(yield, "go_menu_button_long_trip");
    this->_click(yield, QString("long_trip_menu_mission_%1").arg(api_mission_id));
    this->_click(yield, "long_trip_menu_button_ok");
    if (api_deck_id == 2) {
//        this->_click(yield, "select_menu_second_team");
    } else if (api_deck_id == 3) {
        this->_click(yield, "select_menu_third_team");
    } else if (api_deck_id == 4) {
        this->_click(yield, "select_menu_forth_team");
    }
    this->_moveBy(yield, 0, -100);
    this->_click(yield, "long_trip_menu_button_confirm");
    yield(5000);
    this->_click(yield, "submenu_button_back");
}

QPoint Controller::_wait(const QtCoroutine::Yield &yield, const QPixmap &target) {
    auto center = this->_robot->find(target);
    while (center.isNull()) {
        yield(1000);
        center = this->_robot->find(target);
    }
    return center;
}

QPoint Controller::_wait(const QtCoroutine::Yield &yield, const QString &target) {
    QPixmap p(QString(":/ui/res/%1.png").arg(target));
    return this->_wait(yield, p);
}

void Controller::_click(const QtCoroutine::Yield &yield, const QString &target) {
    QPixmap p(QString(":/ui/res/%1.png").arg(target));
    if (p.isNull()) {
        qDebug() << "ui" << target << "not found";
        return;
    }
    auto center = this->_wait(yield, p);
    yield(1000); // prevent UI crash
    this->_robot->click(center);
}

void Controller::_click(const QtCoroutine::Yield &yield, int x, int y) {
    this->_click(yield, QPoint(x, y));
}

void Controller::_click(const QtCoroutine::Yield &yield, const QPoint & target) {
    yield(1000); // prevent UI crash
    this->_robot->click(target);
}

void Controller::_moveBy(const QtCoroutine::Yield &yield, int x, int y) {
    yield(1000);
    this->_robot->moveBy(x, y);
}

QPoint Controller::_find(const QString &target) {
    QPixmap p(QString(":/ui/res/%1.png").arg(target));
    return this->_robot->find(p);
}

void Controller::_onNewConnection() {
    while (this->_httpServer->hasPendingConnections()) {
        auto socket = this->_httpServer->nextPendingConnection();
        this->connect(socket, SIGNAL(readyRead()), SLOT(_onHttpClientReadyRead()));
        this->connect(socket, SIGNAL(disconnected()), SLOT(_onHttpClientDisconnected()));
    }
}

void Controller::_onHttpClientReadyRead() {
    auto socket = qobject_cast<QTcpSocket *>(this->sender());
    this->_handleHttpRequest(socket);
}

void Controller::_onHttpClientDisconnected() {
    auto socket = qobject_cast<QTcpSocket *>(this->sender());
    socket->deleteLater();
}

void Controller::_handleHttpRequest(QIODevice *io) {
    auto task = new QtCoroutine([=](const QtCoroutine::Yield & yield)->void {
        QTextStream sio(io);
        sio.setCodec(QTextCodec::codecForName("UTF-8"));
        auto line = sio.readLine();
        auto parts = line.split(" ");
        auto method = parts[0];
        auto rawResource = parts[1];
        auto protocol = parts[2];
        QUrl resource(rawResource);
        QUrlQuery query(resource);

        qDebug() << method;
        qDebug() << resource.path();
        qDebug() << query.queryItems();

        QByteArray response;
        if (resource.path() == "/api_start") {
            response = QJsonDocument::fromVariant(this->_api_data).toJson();
        } else if (resource.path() == "/api_port") {
            this->_click(yield, "main_menu_button_go");
            auto target = this->_wait(yield, "submenu_button_back");
            while (!target.isNull()) {
                this->_click(yield, target);
                this->_moveBy(yield, 0, -100);
                yield(1000);
                target = this->_find("submenu_button_back");
            }
            this->_wait(yield, "main_menu_button_go");
            yield(1000);
            target = this->_find("main_menu_label_long_trip_done");
            while (!target.isNull()) {
                this->_click(yield, target.x(), target.y());
                target = this->_wait(yield, "long_trip_screen_button_next");
                this->_click(yield, target);
                target = this->_find("long_trip_screen_button_next");
                while (!target.isNull()) {
                    this->_click(yield, target);
                    yield(1000);
                    target = this->_find("long_trip_screen_button_next");
                }
                this->_wait(yield, "main_menu_button_go");
                yield(1000);
                target = this->_find("main_menu_label_long_trip_done");
            }
            response = QJsonDocument::fromVariant(this->_api_port).toJson();
        }

        sio << protocol << " 200 OK\r\n";
        sio << "Content-Type: text/plain; charset=\"utf-8\"\r\n";
        sio << "Content-Length: " << response.size() << "\r\n";
        sio << "\r\n";
        sio << response;
        io->close();
    }, this);
    task->connect(task, SIGNAL(finished()), SLOT(deleteLater()));
    task->start();
}
