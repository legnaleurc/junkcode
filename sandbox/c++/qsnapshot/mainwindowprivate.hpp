#ifndef MAINWINDOWPRIVATE_HPP
#define MAINWINDOWPRIVATE_HPP

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "snapshottimer.hpp"
#include "regiongrabber.hpp"
#include "windowgrabber.hpp"

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

			int delay() const;
			int mode() const;
			bool includeDecorations() const;

		public slots:
			void grab();
			void startUndelayedGrab();
			void onRegionGrabbed( const QPixmap & p );
			void onWindowGrabbed( const QPixmap & p );
			void onGrabTimerTimeout();

		public:
			MainWindow * host;
			Ui::MainWindow ui;
			QWidget * grabber;
			SnapshotTimer * grabTimer;
			RegionGrabber * regionGrabber;
			WindowGrabber * windowGrabber;
			QPixmap snapshot;
			QPixmap pixmap;
			QPoint savedPosition;
			bool modified;
		};

	}
}

#endif
