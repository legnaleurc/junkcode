#include "webpage.h"

#include <QtGui/QMouseEvent>
#include <QtCore/QDateTime>

WebPage::WebPage(QWidget *parent) :
	QWebView(parent)
{
	qDebug() << "custom webkit";
}

void WebPage::mousePressEvent(QMouseEvent *event) {
	QWebView::mousePressEvent(event);
	qDebug() << "press" << event->pos() << event->globalPos();
	qDebug() << "press" << QDateTime::currentMSecsSinceEpoch();
}

void WebPage::mouseReleaseEvent(QMouseEvent *event) {
	QWebView::mouseReleaseEvent(event);
	qDebug() << "release" << event->pos() << event->globalPos();
	qDebug() << "release" << QDateTime::currentMSecsSinceEpoch();
}
