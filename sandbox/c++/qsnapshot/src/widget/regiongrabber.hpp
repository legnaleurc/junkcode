#ifndef QSNAPSHOT_WIDGET_REGIONGRABBER_HPP
#define QSNAPSHOT_WIDGET_REGIONGRABBER_HPP

#include "abstractgrabber.hpp"

namespace qsnapshot {
	namespace widget {

		class RegionGrabber : public AbstractGrabber {
			Q_OBJECT
		public:
			explicit RegionGrabber( QWidget * widget );

			void grab();

		protected:
			virtual void paintEvent( QPaintEvent * event );
			virtual void resizeEvent( QResizeEvent * event );
			virtual void mousePressEvent( QMouseEvent * event );
			virtual void mouseMoveEvent( QMouseEvent * event );
			virtual void mouseReleaseEvent( QMouseEvent * event );
			virtual void mouseDoubleClickEvent( QMouseEvent * event );
			virtual void keyPressEvent( QKeyEvent * event );

		signals:
			void regionGrabbed( const QPixmap & pixmap );

		private:
			class Private;
			Private * p_;
		};

	}
}

#endif
