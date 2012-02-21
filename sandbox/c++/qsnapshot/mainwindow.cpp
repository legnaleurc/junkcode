#include "mainwindowprivate.hpp"

using namespace qsnapshot::widget;

MainWindow::Private::Private( MainWindow * host ):
QObject( host ),
host( host ),
ui() {
	this->ui.setupUi( host );
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
