#ifndef SNAPSHOTTIMERPRIVATE_HPP
#define SNAPSHOTTIMERPRIVATE_HPP

#include "snapshottimer.hpp"

#include <QtCore/QTimer>

namespace qsnapshot {
	namespace widget {

		class SnapshotTimer::Private : public QObject {
		public:
			explicit Private( SnapshotTimer * host );

			SnapshotTimer * host;
			QTimer * timer;
			QRect textRect;
			int time;
			int length;
			bool toggle;
		};

	}
}

#endif
