#ifndef MAINWINDOWPRIVATE_HPP
#define MAINWINDOWPRIVATE_HPP

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "snapshottimer.hpp"
#include "regiongrabber.hpp"

namespace qsnapshot {
	namespace widget {

		class MainWindow::Private : public QObject {
			Q_OBJECT
		public:
			explicit Private( MainWindow * host );

			void grabRegion();
			void performGrab();
			void updatePreview();
			void setPreview( const QPixmap & pixmap );
//			void updateCaption();
//			void setCaption();

			int delay() const;
			int mode() const;

		public slots:
			void grab();
			void startUndelayedGrab();
			void onRegionGrabbed();
			void onGrabTimerTimeout();

		public:
			MainWindow * host;
			Ui::MainWindow ui;
			QWidget * grabber;
			SnapshotTimer * grabTimer;
			RegionGrabber * regionGrabber;
			QPixmap snapshot;
			QPoint savedPosition;
		};

	}
}

#endif
