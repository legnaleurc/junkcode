#ifndef UIINSPECTOR_HPP
#define UIINSPECTOR_HPP

#include <QtGui/QPixmap>
#include <QtCore/QPoint>

#include <tuple>

namespace qsnapshot {
	namespace utility {

		QPixmap grabWindow( std::vector< QRect > & windows );
		std::tuple< QPixmap, QPoint > grabCurrent( bool includeDecorations );

	}
}

#endif // UIINSPECTOR_HPP
