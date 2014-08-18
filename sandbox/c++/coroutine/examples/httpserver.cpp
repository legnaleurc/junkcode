#include "httpserver.hpp"

#include <QtNetwork/QTcpSocket>
#include <QtCore/QTextCodec>
#include <QtCore/QUrl>
#include <QtCore/QUrlQuery>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "qtcoroutine.hpp"

namespace {

void handleRequest (QTextStream & sio, const QByteArray & data) {
    auto line = sio.readLine();
    auto parts = line.split(" ");
    auto method = parts[0];
    auto rawResource = parts[1];
    auto protocol = parts[2];

    QUrl resource(rawResource);
    QUrlQuery query(resource);

    qDebug() << method << resource.path() << query.queryItems();

    sio << protocol << " 200 OK\r\n";
    sio << "Content-Type: text/plain; charset=\"utf-8\"\r\n";
    sio << "Content-Length: " << data.size() << "\r\n";
    sio << "\r\n";
    sio << data;
}

}

HttpServer::HttpServer(QObject *parent) :
    QObject(parent),
    _server(new QTcpServer(this))
{
    this->connect(this->_server, SIGNAL(newConnection()), SLOT(_onNewConnection()));
}

bool HttpServer::listen(quint16 port) {
    return this->_server->listen(QHostAddress::LocalHost, port);
}

void HttpServer::_onNewConnection() {
    while (this->_server->hasPendingConnections()) {
        auto socket = this->_server->nextPendingConnection();
        this->connect(socket, SIGNAL(readyRead()), SLOT(_onClientReadyRead()));
        this->connect(socket, SIGNAL(disconnected()), SLOT(_onClientDisconnected()));
    }
}

void HttpServer::_onClientReadyRead() {
    auto socket = qobject_cast<QTcpSocket *>(this->sender());

    QtCoroutine * task = new QtCoroutine([=](const QtCoroutine::Yield & yield)->void {
        QNetworkAccessManager nasm;
        QUrl a("https://www.google.com/");
        QNetworkRequest b(a);
        auto reply = nasm.get(b);
        yield(reply, SIGNAL(finished()));
        auto c = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        while (c.isValid()) {
            QUrl d = c.toUrl();
            if (d.isRelative()) {
                d = a.resolved(d);
            }
            b.setUrl(d);
            reply = nasm.get(b);
            yield(reply, SIGNAL(finished()));
            c = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        }
        auto d = reply->readAll();

        QTextStream sio(socket);
        sio.setCodec(QTextCodec::codecForName("UTF-8"));
        handleRequest(sio, d);
        socket->close();
    });
    task->connect(task, SIGNAL(finished()), SLOT(deleteLater()));
    task->start();
}

void HttpServer::_onClientDisconnected() {
    auto socket = qobject_cast<QTcpSocket *>(this->sender());
    socket->deleteLater();
    this->_server->close();
}
