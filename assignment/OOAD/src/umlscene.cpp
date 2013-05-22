#include "umlsceneprivate.hpp"
#include "abstractumllink.hpp"
#include "umlitemgroup.hpp"

#include <QtCore/QSet>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsView>
#include <QtGui/QInputDialog>

#include <algorithm>

UMLScene::Private::Private( UMLScene * host ):
host( host ),
pressCommands(),
moveCommands(),
releaseCommands(),
currentPressCommand(),
currentMoveCommand(),
currentReleaseCommand(),
itemType( -1 ),
guideLine( nullptr ),
beginItem( nullptr ),
guidePath( nullptr ),
guideRect( nullptr ),
beginDragPoint(),
grouping( false ) {
	this->pressCommands.insert( std::make_pair( UMLScene::InsertItem, std::bind( &UMLScene::Private::onInsertPressed, this, std::placeholders::_1 ) ) );
	this->pressCommands.insert( std::make_pair( UMLScene::LinkItem, std::bind( &UMLScene::Private::onLinkPressed, this, std::placeholders::_1 ) ) );
	this->pressCommands.insert( std::make_pair( UMLScene::MoveItem, std::bind( &UMLScene::Private::onMovePressed, this, std::placeholders::_1 ) ) );
	this->pressCommands.insert( std::make_pair( UMLScene::DrawItem, std::bind( &UMLScene::Private::onDrawPressed, this, std::placeholders::_1 ) ) );

	this->moveCommands.insert( std::make_pair( UMLScene::LinkItem, std::bind( &UMLScene::Private::onLinkMoved, this, std::placeholders::_1 ) ) );
	this->moveCommands.insert( std::make_pair( UMLScene::MoveItem, std::bind( &UMLScene::Private::onMoveMoved, this, std::placeholders::_1 ) ) );
	this->moveCommands.insert( std::make_pair( UMLScene::DrawItem, std::bind( &UMLScene::Private::onDrawMoved, this, std::placeholders::_1 ) ) );

	this->releaseCommands.insert( std::make_pair( UMLScene::LinkItem, std::bind( &UMLScene::Private::onLinkReleased, this, std::placeholders::_1 ) ) );
	this->releaseCommands.insert( std::make_pair( UMLScene::MoveItem, std::bind( &UMLScene::Private::onMoveReleased, this, std::placeholders::_1 ) ) );
	this->releaseCommands.insert( std::make_pair( UMLScene::DrawItem, std::bind( &UMLScene::Private::onDrawReleased, this, std::placeholders::_1 ) ) );

	this->setMode( UMLScene::MoveItem );
}

QList< QGraphicsItem * > UMLScene::Private::getSelectedRoots() const {
	QList< QGraphicsItem * > selected( this->host->selectedItems() );
	QSet< QGraphicsItem * > roots;
	std::for_each( selected.begin(), selected.end(), [&roots]( QGraphicsItem * item )->void {
		QGraphicsItemGroup * group = item->group();
		while( group && group->group() ) {
			group = group->group();
		}
		roots.insert( group ? group : item );
	} );
	return roots.toList();
}

void UMLScene::Private::setMode( Mode mode ) {
	auto it = this->pressCommands.find( mode );
	this->currentPressCommand = ( ( it != this->pressCommands.end() ) ? ( it->second ) : ( []( QGraphicsSceneMouseEvent * )->void {} ) );

	it = this->moveCommands.find( mode );
	this->currentMoveCommand = ( ( it != this->moveCommands.end() ) ? ( it->second ) : ( []( QGraphicsSceneMouseEvent * )->void {} ) );

	it = this->releaseCommands.find( mode );
	this->currentReleaseCommand = ( ( it != this->releaseCommands.end() ) ? ( it->second ) : ( []( QGraphicsSceneMouseEvent * )->void {} ) );
}

void UMLScene::Private::onInsertPressed( QGraphicsSceneMouseEvent * event ) {
	AbstractUMLItem * item = AbstractUMLItem::create( this->itemType );
	this->host->addItem( item );
	item->setPos( event->scenePos() );
}

void UMLScene::Private::onLinkPressed( QGraphicsSceneMouseEvent * event ) {
	AbstractUMLItem * item = dynamic_cast< AbstractUMLItem * >( this->host->itemAt( event->scenePos(), QTransform() ) );
	if( !item ) {
		return;
	}
	// if this item has a group, skip
	if( item->group() ) {
		return;
	}
	this->beginPort = item->getAttachPort( event->scenePos() );
	this->guideLine = new QGraphicsLineItem( QLineF( event->scenePos(), event->scenePos() ) );
	this->guideLine->setZValue( -1000 );
	QPen pen( this->guideLine->pen() );
	pen.setColor( QColor( "red" ) );
	pen.setStyle( Qt::DotLine );
	this->guideLine->setPen( pen );
	this->host->addItem( this->guideLine );
}

void UMLScene::Private::onLinkMoved( QGraphicsSceneMouseEvent * event ) {
	if( this->guideLine ) {
		this->guideLine->setLine( QLineF( this->guideLine->line().p1(), event->scenePos() ) );
	}
}

void UMLScene::Private::onLinkReleased( QGraphicsSceneMouseEvent * event ) {
	if( !this->guideLine ) {
		return;
	}
	AbstractUMLItem * endItem = dynamic_cast< AbstractUMLItem * >( this->host->itemAt( this->guideLine->line().p2(), QTransform() ) );
	this->host->removeItem( this->guideLine );
	delete this->guideLine;
	this->guideLine = NULL;
	if( !endItem || endItem == this->beginItem ) {
		this->beginItem = NULL;
		return;
	}
	Port * endPort = endItem->getAttachPort( event->scenePos() );
	AbstractUMLLink * link = AbstractUMLLink::create( this->itemType, this->beginPort, endPort );
	this->host->addItem( link );
	link->updatePosition();
}

void UMLScene::Private::onMovePressed( QGraphicsSceneMouseEvent * event ) {
	if( this->host->itemAt( event->scenePos(), QTransform() ) ) {
		this->host->QGraphicsScene::mousePressEvent( event );
	} else {
		// deselect all items
		QList< QGraphicsItem * > selected( this->host->selectedItems() );
		std::for_each( selected.begin(), selected.end(), []( QGraphicsItem * item_ )->void {
			item_->setSelected( false );
		} );

		// setting up guiding rect
		this->grouping = true;
		this->guideRect = new QGraphicsRectItem( QRectF( event->scenePos(), QSizeF() ) );
		this->beginDragPoint = event->scenePos();
		QPen pen( this->guideRect->pen() );
		pen.setColor( "red" );
		pen.setStyle( Qt::DotLine );
		this->guideRect->setPen( pen );
		this->host->addItem( this->guideRect );
	}
}

void UMLScene::Private::onMoveMoved( QGraphicsSceneMouseEvent * event ) {
	if( this->grouping && this->guideRect ) {
		QPointF start( this->beginDragPoint );
		QPointF end( event->scenePos() );
		QRectF rect( start, end );
		this->guideRect->setRect( rect.normalized() );
	} else {
		this->host->QGraphicsScene::mouseMoveEvent( event );
	}
}

void UMLScene::Private::onMoveReleased( QGraphicsSceneMouseEvent * event ) {
	if( this->grouping && this->guideRect ) {
		// select bound
		this->host->setSelectionArea( this->guideRect->mapToScene( this->guideRect->shape() ), Qt::ContainsItemBoundingRect, QTransform() );

		// clean up guiding rect
		this->host->removeItem( this->guideRect );
		delete this->guideRect;
		this->guideRect = NULL;
		this->grouping = false;
	} else {
		this->host->QGraphicsScene::mouseReleaseEvent( event );
	}
}

void UMLScene::Private::onDrawPressed( QGraphicsSceneMouseEvent * event ) {
	this->guidePath = new QGraphicsPathItem;
	QPen pen = this->guidePath->pen();
	pen.setColor( QColor( "red" ) );
	pen.setStyle( Qt::DotLine );
	this->guidePath->setPen( pen );
	this->host->addItem( this->guidePath );
	this->guidePath->setPos( event->scenePos() );
}

void UMLScene::Private::onDrawMoved( QGraphicsSceneMouseEvent * event ) {
	auto path = this->guidePath->path();
	path.lineTo( this->guidePath->mapFromScene( event->scenePos() ) );
	this->guidePath->setPath( path );
}

void UMLScene::Private::onDrawReleased( QGraphicsSceneMouseEvent * ) {
	auto pen = this->guidePath->pen();
	pen.setStyle( Qt::SolidLine );
	this->guidePath->setPen( pen );
	this->guidePath = nullptr;
}

UMLScene::UMLScene( QObject * parent ):
QGraphicsScene( parent ),
p_( new Private( this ) ) {
}

void UMLScene::changeItemName() {
	QList< QGraphicsItem * > selected( this->selectedItems() );
	if( selected.size() != 1 ) {
		return;
	}
	AbstractUMLItem * item = dynamic_cast< AbstractUMLItem * >( selected.at( 0 ) );
	if( !item ) {
		return;
	}
	QString name = QInputDialog::getText( this->views().first(), QObject::tr( "Change object name" ), QObject::tr( "Enter a name" ), QLineEdit::Normal, item->getName() );
	if( name.isEmpty() ) {
		return;
	}
	item->setName( name );
}

void UMLScene::groupSelectedItems() {
	QList< QGraphicsItem * > selected( this->p_->getSelectedRoots() );
	if( selected.size() <= 1 ) {
		return;
	}
	UMLItemGroup * group = new UMLItemGroup;
	this->addItem( group );
	group->setSelected( true );
	std::for_each( selected.begin(), selected.end(), [group]( QGraphicsItem * item )->void {
		group->addToGroup( item );
	} );
}

void UMLScene::ungroupSelectedItems() {
	QList< QGraphicsItem * > selected( this->p_->getSelectedRoots() );
	if( selected.size() != 1 ) {
		return;
	}
	UMLItemGroup * group = dynamic_cast< UMLItemGroup * >( selected.at( 0 ) );
	if( !group ) {
		return;
	}
	group->setSelected( false );
	selected = group->childItems();
	this->destroyItemGroup( group );
	std::for_each( selected.begin(), selected.end(), []( QGraphicsItem * item )->void {
		item->setSelected( false );
	} );
}

void UMLScene::setItemType( int id ) {
	this->p_->itemType = id;
}

void UMLScene::setMode( Mode mode ) {
	this->p_->setMode( mode );
}

void UMLScene::mousePressEvent( QGraphicsSceneMouseEvent * event ) {
	if( event->button() != Qt::LeftButton ) {
		return;
	}

	this->p_->currentPressCommand( event );
}

void UMLScene::mouseMoveEvent( QGraphicsSceneMouseEvent * event ) {
	this->p_->currentMoveCommand( event );
}

void UMLScene::mouseReleaseEvent( QGraphicsSceneMouseEvent * event ) {
	this->p_->currentReleaseCommand( event );
}
