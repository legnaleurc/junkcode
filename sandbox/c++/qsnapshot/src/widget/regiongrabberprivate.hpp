#ifndef REGIONGRABBERPRIVATE_HPP
#define REGIONGRABBERPRIVATE_HPP

#include "regiongrabber.hpp"

#include <QtGui/QPixmap>

namespace qsnapshot {
	namespace widget {

		class RegionGrabber::Private : public QObject {
			Q_OBJECT
		public:
			enum MaskType {
				StrokeMask,
				FillMask
			};

			explicit Private( RegionGrabber * host );

			void grabRect();
			QPoint limitPointToRect( const QPoint & p , const QRect & r ) const;
			QRect normalizeSelection( const QRect & r ) const;
			QRegion handleMask( MaskType type ) const;
			void updateHandles();
			void finishGrab();

		signals:
			void regionGrabbed( const QPixmap & pixmap );

		public:
			RegionGrabber * host;
			QPixmap pixmap;
			QRect selection;
			QRect selectionBeforeDrag;
			QRect helpTextRect;
			QPoint dragStartPoint;
			QRect * mouseOverHandle;
			bool grabbing;
			bool showHelp;
			bool mouseDown;
			bool newSelection;
			// naming convention for handles
			// T top, B bottom, R Right, L left
			// 2 letters: a corner
			// 1 letter: the handle on the middle of the corresponding side
			QRect TLHandle, TRHandle, BLHandle, BRHandle;
			QRect LHandle, THandle, RHandle, BHandle;
			QList< QRect * > handles;
		};

	}
}

#endif
