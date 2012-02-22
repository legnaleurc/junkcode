#ifndef WINDOWSGRABBERPRIVATE_HPP
#define WINDOWSGRABBERPRIVATE_HPP

#include "windowgrabber.hpp"

#include <QtCore/QRect>

#include <vector>

namespace qsnapshot {
	namespace widget {

		class WindowGrabber::Private : public QObject {
		public:
			Private( WindowGrabber * host );

			int windowIndex( const QPoint & pos ) const;
			void increaseScope( const QPoint & pos );
			void decreaseScope( const QPoint & pos );
			void finishGrab();

			WindowGrabber * host;
			std::vector< QRect > windows;
			int current;
			int yPos;
			QPoint windowPosition;
		};

	}
}

#endif // WINDOWSGRABBERPRIVATE_HPP
