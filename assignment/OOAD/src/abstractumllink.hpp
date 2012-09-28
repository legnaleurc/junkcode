#ifndef ABSTRACTUMLLINK_HPP
#define ABSTRACTUMLLINK_HPP

#include <QtGui/QGraphicsLineItem>

#include <functional>
#include <memory>

class Port;

class AbstractUMLLink : public QGraphicsLineItem {
public:
	static AbstractUMLLink * create( int id, Port * begin, Port * end );
	static bool signup( int id, std::function< AbstractUMLLink * ( Port *, Port * ) > creator );

	enum {
		Type = QGraphicsItem::UserType + 256
	};

	AbstractUMLLink( Port * begin, Port * end, QGraphicsItem * parent = 0 );

	virtual int type() const;

	void updatePosition();

private:
	class Private;
	std::shared_ptr< Private > p_;
};

#endif
