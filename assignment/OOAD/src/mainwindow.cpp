#include "mainwindowprivate.hpp"

#include <QtGui/QApplication>
#include <QtGui/QToolButton>

MainWindow::Private::Private( MainWindow * parent ):
QObject( parent ),
ui(),
scene( new UMLScene( parent ) ),
toolGroup( new QButtonGroup( this ) ) {
	this->ui.setupUi( parent );

	QToolButton * tool = NULL;

	tool = new QToolButton( parent );
	tool->setCheckable( true );
	tool->setChecked( true );
	tool->setText( QObject::tr( "Select" ) );
	tool->setToolTip( QObject::tr( "Select" ) );
	tool->setIcon( QIcon( ":/images/pointer.png" ) );
	this->toolGroup->addButton( tool, 0 );
	this->ui.toolBar->addWidget( tool );

	tool = new QToolButton( parent );
	tool->setCheckable( true );
	tool->setText( QObject::tr( "Association Line" ) );
	tool->setToolTip( QObject::tr( "Association Line" ) );
	tool->setIcon( QIcon( ":/images/association.png" ) );
	this->toolGroup->addButton( tool, 16 + 0 );
	this->ui.toolBar->addWidget( tool );

	tool = new QToolButton( parent );
	tool->setCheckable( true );
	tool->setText( QObject::tr( "Generalization Line" ) );
	tool->setToolTip( QObject::tr( "Generalization Line" ) );
	tool->setIcon( QIcon( ":/images/generalisation.png" ) );
	this->toolGroup->addButton( tool, 16 + 1 );
	this->ui.toolBar->addWidget( tool );

	tool = new QToolButton( parent );
	tool->setCheckable( true );
	tool->setText( QObject::tr( "Composition Line" ) );
	tool->setToolTip( QObject::tr( "Composition Line" ) );
	tool->setIcon( QIcon( ":/images/composition.png" ) );
	this->toolGroup->addButton( tool, 16 + 2 );
	this->ui.toolBar->addWidget( tool );

	tool = new QToolButton( parent );
	tool->setCheckable( true );
	tool->setText( QObject::tr( "Class" ) );
	tool->setToolTip( QObject::tr( "Class" ) );
	tool->setIcon( QIcon( ":/images/class.png" ) );
	this->toolGroup->addButton( tool, 256 );
	this->ui.toolBar->addWidget( tool );

	tool = new QToolButton( parent );
	tool->setCheckable( true );
	tool->setText( QObject::tr( "Use Case" ) );
	tool->setToolTip( QObject::tr( "Use Case" ) );
	tool->setIcon( QIcon( ":/images/usecase.png" ) );
	this->toolGroup->addButton( tool, 256 + 1 );
	this->ui.toolBar->addWidget( tool );

	qApp->connect( this->ui.actionAbout_Qt, SIGNAL( triggered() ), SLOT( aboutQt() ) );
	this->connect( this->toolGroup, SIGNAL( buttonClicked( int ) ), SLOT( onToolGroupClicked( int ) ) );
	this->connect( this->ui.action_Change_Object_Name, SIGNAL( triggered() ), SLOT( onChangeNameClicked() ) );
	this->connect( this->ui.action_Group, SIGNAL( triggered() ), SLOT( onGroupClicked() ) );
	this->connect( this->ui.actionU_ngroup, SIGNAL( triggered() ), SLOT( onUngroupClicked() ) );

	this->scene->setSceneRect( 0, 0, 5000, 5000 );
	this->ui.graphicsView->setScene( this->scene );
}

void MainWindow::Private::onToolGroupClicked( int id ) {
	if( id & 256 ) {
		// is item type
		this->scene->setMode( UMLScene::InsertItem );
		this->scene->setItemType( id ^ 256 );
	} else if( id & 16 ) {
		this->scene->setMode( UMLScene::LinkItem );
		this->scene->setItemType( id ^ 16 );
	} else {
		this->scene->setMode( UMLScene::MoveItem );
	}
}

void MainWindow::Private::onChangeNameClicked() {
	this->scene->changeItemName();
}

void MainWindow::Private::onGroupClicked() {
	this->scene->groupSelectedItems();
}

void MainWindow::Private::onUngroupClicked() {
	this->scene->ungroupSelectedItems();
}

MainWindow::MainWindow():
p_( new Private( this ) ) {
	this->connect( this->p_->ui.actionE_xit, SIGNAL( triggered() ), SLOT( close() ) );
}
