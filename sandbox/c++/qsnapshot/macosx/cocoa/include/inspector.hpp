#ifndef QSNAPSHOT_UTILITY_INSPECTOR_HPP
#define QSNAPSHOT_UTILITY_INSPECTOR_HPP

#include <QtCore/QSharedPointer>
#include <QtCore/QPoint>
#include <QtGui/QPixmap>
#include <QtCore/QRect>

#include <vector>

class Inspector {
public:
	Inspector();

	QPixmap grabWindow( std::vector< QRect > & windows );

private:
	class Private;
	QSharedPointer< Private > p_;
};

#endif
