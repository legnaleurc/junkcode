#ifndef QSNAPSHOT_WIDGET_ABSTRACTGRABBER_H
#define QSNAPSHOT_WIDGET_ABSTRACTGRABBER_H

#include <QtGui/QWidget>

namespace qsnapshot {
	namespace widget {

		class AbstractGrabber : public QWidget {
			Q_OBJECT
		public:
			AbstractGrabber( QWidget * parent, Qt::WindowFlags f );

		signals:

		public slots:

		};

	}
}

#endif
