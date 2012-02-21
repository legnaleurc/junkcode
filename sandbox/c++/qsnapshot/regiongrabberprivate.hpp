#ifndef REGIONGRABBERPRIVATE_HPP
#define REGIONGRABBERPRIVATE_HPP

#include "regiongrabber.hpp"

#include <QtGui/QPixmap>

namespace qsnapshot {
	namespace widget {

		class RegionGrabber::Private : public QObject {
			Q_OBJECT
		public:
			Private();

			void grabRect();
			void limitPointToRect( const QPoint & p , const QRect & r ) const;
			void normalizeSelection( const QRect & r ) const;

		signals:
			void regionGrabbed( const QPixmap & pixmap );

		public:
			QPixmap pixmap;
			QRect selection;
			QRect selectionBeforeDrag;
			QRect helpTextRect;
			QPoint dragStartPoint;
			int mouseOverHandle;
			bool grabbing;
			bool showHelp;
			bool mouseDown;
			bool newSelection;
		};

	}
}

#endif
