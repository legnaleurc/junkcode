#include "networkreplyproxy.h"

#include <QNetworkCookie>

NetworkReplyProxy::NetworkReplyProxy(QObject* parent, QNetworkReply* reply) :
	QNetworkReply(parent)
			, m_reply(reply)
{
	qWarning("Starting network job: %p %s", this, qPrintable(m_reply->url().toString()));
	 // apply attributes...
	 setOperation(m_reply->operation());
	 setRequest(m_reply->request());
	 setUrl(m_reply->url());

	 // handle these to forward
	 connect(m_reply, SIGNAL(metaDataChanged()), SLOT(applyMetaData()));
	 connect(m_reply, SIGNAL(readyRead()), SLOT(readInternal()));
	 connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(errorInternal(QNetworkReply::NetworkError)));

	 // forward signals
	 connect(m_reply, SIGNAL(finished()), SIGNAL(finished()));
	 connect(m_reply, SIGNAL(uploadProgress(qint64,qint64)), SIGNAL(uploadProgress(qint64,qint64)));
	 connect(m_reply, SIGNAL(downloadProgress(qint64,qint64)), SIGNAL(downloadProgress(qint64,qint64)));

	 // for the data proxy...
	 setOpenMode(ReadOnly);
}

NetworkReplyProxy::~NetworkReplyProxy() {
	delete m_reply;
}

void NetworkReplyProxy::abort() {
	m_reply->abort();
}

void NetworkReplyProxy::close() {
	m_reply->close();
}

bool NetworkReplyProxy::isSequential() const {
	return m_reply->isSequential();
}

void NetworkReplyProxy::setReadBufferSize(qint64 size) {
	QNetworkReply::setReadBufferSize(size); m_reply->setReadBufferSize(size);
}

qint64 NetworkReplyProxy::bytesAvailable() const {
	return m_buffer.size() + QIODevice::bytesAvailable();
}

qint64 NetworkReplyProxy::bytesToWrite() const {
	return -1;
}

bool NetworkReplyProxy::canReadLine() const {
	qFatal("not implemented");
	return false;
}

bool NetworkReplyProxy::waitForReadyRead(int) {
	qFatal("not implemented");
	return false;
}

bool NetworkReplyProxy::waitForBytesWritten(int) {
	qFatal("not implemented");
	return false;
}

qint64 NetworkReplyProxy::readData(char* data, qint64 maxlen) {
	qint64 size = qMin(maxlen, qint64(m_buffer.size()));
	memcpy(data, m_buffer.constData(), size);
	m_buffer.remove(0, size);
	return size;
}

void NetworkReplyProxy::ignoreSslErrors() {
	m_reply->ignoreSslErrors();
}

void NetworkReplyProxy::applyMetaData() {
	QList<QByteArray> headers = m_reply->rawHeaderList();
	foreach(QByteArray header, headers) {
		setRawHeader(header, m_reply->rawHeader(header));
	}

	setHeader(QNetworkRequest::ContentTypeHeader, m_reply->header(QNetworkRequest::ContentTypeHeader));
	setHeader(QNetworkRequest::ContentLengthHeader, m_reply->header(QNetworkRequest::ContentLengthHeader));
	setHeader(QNetworkRequest::LocationHeader, m_reply->header(QNetworkRequest::LocationHeader));
	setHeader(QNetworkRequest::LastModifiedHeader, m_reply->header(QNetworkRequest::LastModifiedHeader));
	setHeader(QNetworkRequest::SetCookieHeader, m_reply->header(QNetworkRequest::SetCookieHeader));

	setAttribute(QNetworkRequest::HttpStatusCodeAttribute, m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute));
	setAttribute(QNetworkRequest::HttpReasonPhraseAttribute, m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute));
	setAttribute(QNetworkRequest::RedirectionTargetAttribute, m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute));
	setAttribute(QNetworkRequest::ConnectionEncryptedAttribute, m_reply->attribute(QNetworkRequest::ConnectionEncryptedAttribute));
	setAttribute(QNetworkRequest::CacheLoadControlAttribute, m_reply->attribute(QNetworkRequest::CacheLoadControlAttribute));
	setAttribute(QNetworkRequest::CacheSaveControlAttribute, m_reply->attribute(QNetworkRequest::CacheSaveControlAttribute));
	setAttribute(QNetworkRequest::SourceIsFromCacheAttribute, m_reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute));
	setAttribute(QNetworkRequest::DoNotBufferUploadDataAttribute, m_reply->attribute(QNetworkRequest::DoNotBufferUploadDataAttribute));
	emit metaDataChanged();
}

void NetworkReplyProxy::errorInternal(QNetworkReply::NetworkError _error) {
	setError(_error, errorString());
	emit error(_error);
}

void NetworkReplyProxy::readInternal() {
	QByteArray data = m_reply->readAll();
	m_data += data;
	m_buffer += data;
	emit readyRead();
}

const QByteArray & NetworkReplyProxy::data() const {
	return this->m_data;
}
