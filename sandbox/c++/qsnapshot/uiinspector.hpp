#ifndef UIINSPECTOR_HPP
#define UIINSPECTOR_HPP

#include <QtGui/QPixmap>

namespace qsnapshot {
	namespace utility {

		QPixmap grabWindow( std::vector< QRect > & windows );
		QPixmap grabCurrent( bool includeDecorations );

	}
}

#endif // UIINSPECTOR_HPP
