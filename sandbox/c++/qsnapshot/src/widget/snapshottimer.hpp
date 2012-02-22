#ifndef SNAPSHOTTIMER_HPP
#define SNAPSHOTTIMER_HPP

#include <QtGui/QWidget>

namespace qsnapshot {
	namespace widget {

		class SnapshotTimer : public QWidget {
			Q_OBJECT
		public:
			explicit SnapshotTimer( QWidget * parent );

			void start( int seconds );
			void stop();

		protected:
			virtual void paintEvent( QPaintEvent * event );
			virtual void enterEvent( QEvent * event );

		signals:
			void timeout();

		protected slots:
			void bell();

		private:
			class Private;
			friend class Private;
			Private * p_;
		};

	}
}

#endif
