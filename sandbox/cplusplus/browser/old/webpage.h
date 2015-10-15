#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QtWebKitWidgets/QWebView>

class WebPage : public QWebView
{
	Q_OBJECT
public:
	explicit WebPage(QWidget *parent = 0);

protected:
	virtual void mousePressEvent(QMouseEvent * event);
	virtual void mouseReleaseEvent(QMouseEvent * event);

signals:

public slots:

};

#endif // WEBPAGE_H
