#ifndef QSNAPSHOT_WIDGET_ABSTRACTGRABBER_H
#define QSNAPSHOT_WIDGET_ABSTRACTGRABBER_H

#include <QtGui/QWidget>

namespace qsnapshot {
	namespace widget {

		class AbstractGrabber : public QWidget {
			Q_OBJECT
		public:
			explicit AbstractGrabber( QWidget * parent );

		signals:

		public slots:

		};

	}
}

#endif
