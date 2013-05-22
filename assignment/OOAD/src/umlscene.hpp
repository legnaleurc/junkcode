#ifndef UMLSCENE_HPP
#define UMLSCENE_HPP

#include <QtGui/QGraphicsScene>

#include <memory>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class UMLScene : public QGraphicsScene {
public:
	enum Mode {
		InsertItem,
		LinkItem,
		MoveItem,
		DrawItem,
		Unknown
	};

	explicit UMLScene( QObject * parent );

	void changeItemName();
	void groupSelectedItems();
	void ungroupSelectedItems();
	void setItemType( int id );
	void setMode( Mode mode );

protected:
	virtual void mousePressEvent( QGraphicsSceneMouseEvent * event );
	virtual void mouseMoveEvent( QGraphicsSceneMouseEvent * event );
	virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event );

private:
	class Private;
	std::shared_ptr< Private > p_;
};

#endif
