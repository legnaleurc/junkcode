#include "mainwindowprivate.hpp"

#include <QtGui/QDesktopWidget>
#include <QtCore/QTimer>

using namespace qsnapshot::widget;

MainWindow::Private::Private( MainWindow * host ):
QObject( host ),
host( host ),
ui(),
// NOTE Windows and Mac OS X flag
grabber( new QWidget( host, Qt::X11BypassWindowManagerHint ) ),
grabTimer( new SnapshotTimer( host ) ),
snapshot(),
savedPosition() {
	this->ui.setupUi( host );

	this->ui.snapshotDelay->setSuffix( QObject::tr( " second(s)", "" ) );

	this->grabber->installEventFilter( host );
	this->grabber->show();
	this->grabber->grabMouse();

#ifdef HAVE_X11_EXTENSIONS_XFIXES_H
	{
		int tmp1, tmp2;
		//Check whether the XFixes extension is available
		Display *dpy = QX11Info::display();
		if (!XFixesQueryExtension( dpy, &tmp1, &tmp2 )) {
			mainWidget->cbIncludePointer->hide();
			mainWidget->lblIncludePointer->hide();
		}
	}
#elif !defined(Q_WS_WIN)
//	this->ui.cbIncludePointer->hide();
//	this->ui.lblIncludePointer->hide();
#endif

	if( qApp->desktop()->numScreens() < 2 ) {
//		this->ui->comboMode->removeItem(CurrentScreen);
	}

	this->connect( this->ui.newSnapshot, SIGNAL( clicked() ), SLOT( grab() ) );
	this->connect( this->grabTimer, SIGNAL( timeout() ), SLOT( onGrabTimerTimeout() ) );
}

void MainWindow::Private::grab() {
	savedPosition = this->host->pos();
	this->host->hide();

	if( this->delay() > 0 ) {
		this->grabTimer->start( this->delay() );
	} else {
		QTimer::singleShot( 0, this, SLOT( startUndelayedGrab() ) );
	}
}

// FIXME polymorphithm
void MainWindow::Private::grabRegion() {
	this->rgnGrab = new RegionGrabber;
	this->connect( this->rgnGrab, SIGNAL( regionGrabbed( const QPixmap & ) ), SLOT( onRegionGrabbed( const QPixmap & ) ) );
}

// FIXME polymorphithm
void MainWindow::Private::grabFreeRegion() {
	this->freeRgnGrab = new FreeRegionGrabber;
	this->connect( this->freeRgnGrab, SIGNAL( freeRegionGrabbed( const QPixmap & ) ), SLOT( onRegionGrabbed( const QPixmap & ) ) );
}

void MainWindow::Private::performGrab() {
	int x = 0;
	int y = 0;

	this->grabber->releaseMouse();
	this->grabber->hide();
	this->grabTimer.stop();

//	title.clear();
//	windowClass.clear();

	// TODO command pattern
	if( this->mode() == ChildWindow ) {
		// TODO event scope
		WindowGrabber wndGrab;
		this->connect( &wndGrab, SIGNAL( windowGrabbed( const QPixmap & ) ), SLOT( onWindowGrabbed( const QPixmap & ) ) );
		wndGrab.exec();
		QPoint offset = wndGrab.lastWindowPosition();
		x = offset.x();
		y = offset.y();
		qDebug() << "last window position is" << offset;
	} else if ( this->mode() == WindowUnderCursor ) {
		this->snapshot = WindowGrabber::grabCurrent( this->includeDecorations() );

		QPoint offset = WindowGrabber::lastWindowPosition();
		x = offset.x();
		y = offset.y();

		// If we're showing decorations anyway then we'll add the title and window
		// class to the output image meta data.
		if( this->includeDecorations() ) {
			title = WindowGrabber::lastWindowTitle();
			windowClass = WindowGrabber::lastWindowClass();
		}
	} else if ( this->mode() == CurrentScreen ) {
		QDesktopWidget * desktop = QApplication::desktop();
		int screenId = desktop->screenNumber( QCursor::pos() );
		QRect geom = desktop->screenGeometry( screenId );
		x = geom.x();
		y = geom.y();
		this->snapshot = QPixmap::grabWindow( desktop->winId(), x, y, geom.width(), geom.height() );
	} else {
		this->snapshot = QPixmap::grabWindow( QApplication::desktop()->winId() );
	}
#ifdef HAVE_X11_EXTENSIONS_XFIXES_H
	if( haveXFixes && includePointer() ) {
		grabPointerImage(x, y);
	}
#endif

	this->updatePreview();
	QApplication::restoreOverrideCursor();
	this->modified = true;
//	updateCaption();
	if( this->savedPosition != QPoint( -1, -1 ) ) {
		this->host->move( this->savedPosition );
	}
	this->host->show();
}

void MainWindow::Private::updatePreview() {
	this->setPreview( pixmap );
}

void MainWindow::Private::setPreview( const QPixmap & pixmap ) {
	this->ui.preview->setToolTip( QObject::tr( "Preview of the snapshot image (%1 x %2)" , pixmap.width(), pixmap.height() ) );

//	this->ui.preview->setPreview( pixmap );
	this->ui.preview->adjustSize();
}

int MainWindow::Private::delay() const {
	return this->ui.snapshotDelay->value();
}

int MainWindow::Private::mode() const {
	return this->ui.captureMode->currentIndex();
}

void MainWindow::Private::startUndelayedGrab() {
	if( mode() == Region ) {
		this->grabRegion();
	} else if ( mode() == FreeRegion ) {
		this->grabFreeRegion();
	} else {
		this->grabber->show();
		this->grabber->grabMouse( Qt::CrossCursor );
	}
}

void MainWindow::Private::onRegionGrabbed() {
	if( !pix.isNull() ) {
		this->snapshot = pix;
		this->updatePreview();
		modified = true;
		updateCaption();
	}

	// FIXME polymorphithm
	if( this->mode() == KSnapshotObject::Region ) {
		this->rgnGrab->deleteLater();
	} else if( this->mode() == KSnapshotObject::FreeRegion ) {
		this->freeRgnGrab->deleteLater();
	}

	QApplication::restoreOverrideCursor();
	this->host->show();
}

void MainWindow::Private::onGrabTimerTimeout() {
	if( this->mode() == Region ) {
		this->grabRegion();
	} else if ( mode() == FreeRegion ) {
		this->grabFreeRegion();
	} else {
		this->performGrab();
	}
	// TODO system notify
//	KNotification::beep(i18n("The screen has been successfully grabbed."));
}

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
p_( new Private( this ) ) {
}

void MainWindow::changeEvent( QEvent * e ) {
	this->QMainWindow::changeEvent( e );
	switch( e->type() ) {
	case QEvent::LanguageChange:
		this->p_->ui.retranslateUi( this );
		break;
	default:
		break;
	}
}
