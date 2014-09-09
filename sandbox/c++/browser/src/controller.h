#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QtNetwork/QTcpServer>

#include <queue>

#include "networkaccessmanagerproxy.h"
#include "database.h"
#include "qtcoroutine.hpp"
#include "robot.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    virtual ~Controller();

    void setNetworkAccessManager(NetworkAccessManagerProxy * namp);
    void setMouseArea(QWidget * ma);

    void start();
    void stop();

    void startMission(int api_deck_id, int api_mission_id);

signals:
    void api_port_port();
    void responsed(const QString & response);

private slots:
    void _onRequestFinished(const QString & path, const QJsonDocument & json);
    void _onNewConnection();
    void _onHttpClientReadyRead();
    void _onHttpClientDisconnected();

private:
    typedef std::function<void (const QJsonDocument &)> ApiCallback;

    void _gadgets_make_request(const QJsonDocument & json);
    void _api_start2(const QJsonDocument & json);
    void _api_get_member_basic(const QJsonDocument & json);
    void _api_port_port(const QJsonDocument & json);
    void _api_req_mission_start(const QJsonDocument & json);
    void _charge(const QtCoroutine::Yield & yield, int api_deck_id);
    void _startMission(const QtCoroutine::Yield & yield, int api_deck_id, int api_mission_id);

    QPoint _wait(const QtCoroutine::Yield & yield, const QPixmap & target);
    QPoint _wait(const QtCoroutine::Yield & yield, const QString & target);
    void _click(const QtCoroutine::Yield & yield, const QString &target);
    void _click(const QtCoroutine::Yield & yield, int x, int y);
    void _click(const QtCoroutine::Yield & yield, const QPoint & target);
    void _moveBy(const QtCoroutine::Yield & yield, int x, int y);
    QPoint _find(const QString & target);

    void _handleHttpRequest(QIODevice * io);

    Database _db;
    NetworkAccessManagerProxy * _namp;
    std::map<QString, ApiCallback> _cb;
    std::shared_ptr<Robot> _robot;
    std::queue<QtCoroutine *> _eventQueue;
    QTcpServer * _httpServer;
    QVariantMap _api_data;
    QVariantMap _api_port;
};

#endif // CONTROLLER_H
