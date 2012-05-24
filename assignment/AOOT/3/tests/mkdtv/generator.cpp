#include "generator.hpp"

#include <QtGui/QImage>
#include <QtCore/QList>
#include <QtCore/QBuffer>

const QList< QByteArray > EXTS = QList< QByteArray >() << "PNG" << "JPG";

Generator::Generator() :
	QObject(),
	QRunnable()
{
	qsrand( reinterpret_cast< long >( static_cast< void * >( this ) ) );
}

void Generator::run() {
	int width = qrand() % 2048 + 1;
	int height = qrand() % 2048 + 1;
	QImage image( width, height, QImage::Format_ARGB32_Premultiplied );
	uint pixel = ( static_cast< uint >( qrand() ) << 16 ) | ( static_cast< uint >( qrand() ) & 0x0000ffff );
	image.fill( pixel );
	QByteArray data;
	QBuffer buffer( &data );
	QByteArray type( EXTS.at( qrand() % 2 ) );
	image.save( &buffer, type );
	emit this->finished( data, type );
}
