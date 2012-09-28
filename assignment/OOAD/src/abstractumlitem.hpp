#ifndef ABSTRACTUMLITEM_HPP
#define ABSTRACTUMLITEM_HPP

#include <QtGui/QAbstractGraphicsShapeItem>

#include <functional>
#include <memory>

class AbstractUMLLink;
class Port;
class UMLItemGroup;

class AbstractUMLItem : public QAbstractGraphicsShapeItem {
public:
	static AbstractUMLItem * create( int id, QGraphicsItem * parent = 0 );
	static bool signup( int id, std::function< AbstractUMLItem * ( QGraphicsItem * ) > creator );
	enum {
		Type = QGraphicsItem::UserType + 16
	};

	AbstractUMLItem( QGraphicsItem * parent = 0 );

	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 ) = 0;
	virtual int type() const;

	const QString & getName() const;
	void setName( const QString & name );

	Port * getAttachPort( const QPointF & scenePosition ) const;

private:
	class Private;
	std::shared_ptr< Private > p_;
};

#endif
