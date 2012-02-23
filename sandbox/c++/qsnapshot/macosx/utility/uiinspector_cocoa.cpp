#include "utility/uiinspector.hpp"
#include "inspector.hpp"

QPixmap qsnapshot::utility::grabWindow( std::vector< QRect > & windows ) {
	Inspector inspector;
	QPixmap pm( inspector.grabWindow( windows ) );
	return pm;
}
