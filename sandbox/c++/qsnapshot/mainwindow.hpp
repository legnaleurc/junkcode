#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtGui/QMainWindow>

namespace qsnapshot {
	namespace widget {

		class MainWindow : public QMainWindow {
			Q_OBJECT

		public:
			explicit MainWindow( QWidget * parent = 0 );

		protected:
			virtual void changeEvent( QEvent * e );

		private:
			class Private;
			friend class Private;
			Private * p_;
		};

	}
}

#endif
