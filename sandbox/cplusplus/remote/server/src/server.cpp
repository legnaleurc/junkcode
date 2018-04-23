#include "server.hpp"

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QByteArray>
#include <QtCore/QtEndian>


using remote::Server;


class Server::Private : public QObject {
    Q_OBJECT

public:
    Private(Server * parent);

    void handleSocket(QTcpSocket * socket);
    bool readPacket();

public slots:
    void onNewConnection();
    void onClientReadyRead();

public:
    QTcpServer * server;
    QTcpSocket * socket;
    QByteArray buffer;
};


Server::Server()
    : QObject()
    , _(new Private{this})
{}


void Server::start() {
    _->server->listen(QHostAddress::Any, 1234);
}


Server::Private::Private(Server * parent)
    : QObject(parent)
    , server(new QTcpServer{this})
    , socket(nullptr)
    , buffer()
{
    QObject::connect(this->server,
                     &QTcpServer::newConnection,
                     this,
                     &Server::Private::onNewConnection);
}


void Server::Private::onNewConnection() {
    while (this->server->hasPendingConnections()) {
        auto socket = this->server->nextPendingConnection();
        this->handleSocket(socket);
    }
}


void Server::Private::handleSocket(QTcpSocket * socket) {
    if (this->socket) {
        socket->disconnectFromHost();
        socket->deleteLater();
        return;
    }
    QObject::connect(socket,
                     &QTcpSocket::readyRead,
                     this,
                     &Private::onClientReadyRead);
    this->socket = socket;
}


void Server::Private::onClientReadyRead() {
    auto chunk = this->socket->readAll();
    this->buffer.append(chunk);
    while (this->readPacket());
}


bool Server::Private::readPacket() {
    if (this->buffer.size() < 8) {
        return false;
    }
    quint64 length = qFromLittleEndian<quint64>(this->buffer);
    if (this->buffer.size() < 8 + length) {
        return false;
    }
    auto data = this->buffer.mid(8, length);
    this->buffer = this->buffer.mid(8 + length);
    return true;
}


#include "server.moc"
