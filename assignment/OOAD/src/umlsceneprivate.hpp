#ifndef UMLSCENEPRIVATE_HPP
#define UMLSCENEPRIVATE_HPP

#include "umlscene.hpp"
#include "abstractumlitem.hpp"
#include "port.hpp"

#include <functional>

class UMLScene::Private {
public:
	typedef std::function< void ( QGraphicsSceneMouseEvent * ) > Command;

	Private( UMLScene * );

	QList< QGraphicsItem * > getSelectedRoots() const;

	void onInsertPressed( QGraphicsSceneMouseEvent * );
	void onLinkPressed( QGraphicsSceneMouseEvent * );
	void onLinkMoved( QGraphicsSceneMouseEvent * );
	void onLinkReleased( QGraphicsSceneMouseEvent * );
	void onMovePressed( QGraphicsSceneMouseEvent * );
	void onMoveMoved( QGraphicsSceneMouseEvent * );
	void onMoveReleased( QGraphicsSceneMouseEvent * );

	UMLScene * host;
	std::map< UMLScene::Mode, Command > pressCommands;
	std::map< UMLScene::Mode, Command > moveCommands;
	std::map< UMLScene::Mode, Command > releaseCommands;
	Command currentPressCommand;
	Command currentMoveCommand;
	Command currentReleaseCommand;
	int itemType;
	QGraphicsLineItem * guideLine;
	Port * beginPort;
	AbstractUMLItem * beginItem;
	QPointF beginAttachPoint;
	QGraphicsRectItem * guideRect;
	QPointF beginDragPoint;
	bool grouping;
};

#endif
