#ifndef PORT_HPP
#define PORT_HPP

#include <QtCore/QPointF>

#include <memory>

class AbstractUMLItem;
class AbstractUMLLink;

class Port {
public:
	explicit Port( AbstractUMLItem * item );

	void addLink( AbstractUMLLink * link );
	QPointF getScenePosition() const;
	void setPosition( const QPointF & itemPosition );
	void updateLinks();

private:
	class Private;
	std::shared_ptr< Private > p_;
};

#endif
