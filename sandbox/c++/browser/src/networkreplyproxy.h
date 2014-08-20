#ifndef NETWORKREPLYPROXY_H
#define NETWORKREPLYPROXY_H

#include <QtNetwork/QNetworkReply>

class NetworkReplyProxy : public QNetworkReply
{
	Q_OBJECT
public:
	explicit NetworkReplyProxy(QObject* parent, QNetworkReply* reply);
	virtual ~NetworkReplyProxy();

	virtual void abort();
	virtual void close();
	virtual bool isSequential() const;

	virtual void setReadBufferSize(qint64 size);

	virtual qint64 bytesAvailable() const;

	virtual qint64 bytesToWrite() const;
	virtual bool canReadLine() const;

	virtual bool waitForReadyRead(int);
	virtual bool waitForBytesWritten(int);

	virtual qint64 readData(char* data, qint64 maxlen);

	const QByteArray & data() const;

public slots:
	virtual void ignoreSslErrors();
	void applyMetaData();
	void errorInternal(QNetworkReply::NetworkError _error);
	void readInternal();

private:
	QNetworkReply* m_reply;
	QByteArray m_data;
	QByteArray m_buffer;
};

#endif // NETWORKREPLYPROXY_H
