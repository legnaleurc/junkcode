#ifndef QSNAPSHOT_WIDGET_WINDOWGRABBER_HPP
#define QSNAPSHOT_WIDGET_WINDOWGRABBER_HPP

#include "abstractgrabber.hpp"

#include <QtGui/QPixmap>
#include <QtCore/QPoint>

#include <tuple>

namespace qsnapshot {
	namespace widget {

		class WindowGrabber : public AbstractGrabber {
			Q_OBJECT
		public:
			static std::tuple< QPixmap, QPoint > grabCurrent( bool includeDecorations );

			WindowGrabber( QWidget * parent );

			void grab();

		protected:
			virtual void mousePressEvent( QMouseEvent * event );
			virtual void mouseReleaseEvent( QMouseEvent * event );
			virtual void mouseMoveEvent( QMouseEvent * event );
			virtual void wheelEvent( QWheelEvent * event );
			virtual void paintEvent( QPaintEvent * event );

		signals:
			void windowGrabbed( const QPixmap & pixmap );

		private:
			class Private;
			Private * p_;
		};

	}
}

#endif
