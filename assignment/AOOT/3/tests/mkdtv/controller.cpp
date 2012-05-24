#include "controller.hpp"
#include "generator.hpp"

#include <QtCore/QThreadPool>

const int HEADER_LENGTH = 18 + 5 + 2;
const int MIDDLE_LENGTH = 5 + 5 + 9 + 3;

Controller::Controller( int jobs ) :
	QProgressDialog(),
	output_( "input.dtv" ),
	jobs_( jobs ),
	completed_( 0 ),
	mutex_(),
	totalSize_( 0 )
{
	qsrand( reinterpret_cast< long >( static_cast< void * >( this ) ) );
	this->setRange( 0, jobs );
}

void Controller::start() {
	this->show();
	this->output_.open( QIODevice::WriteOnly );
	this->output_.write( "#DTV:copyright1.0#" );
	this->output_.write( QString( "%1" ).arg( this->jobs_, 5, 10, QChar( ' ' ) ).toUtf8() );
	this->output_.write( "@@" );
	for( int i = 0; i < this->jobs_; ++i ) {
		Generator * g = new Generator;
		this->connect( g, SIGNAL( finished( const QByteArray &, const QByteArray & ) ), SLOT( onFinished( const QByteArray &, const QByteArray & ) ) );
		QThreadPool::globalInstance()->start( g );
	}
}

void Controller::onFinished( const QByteArray & data, const QByteArray & type ) {
	QMutexLocker locker( &this->mutex_ );
	Q_UNUSED( locker )

	this->output_.seek( HEADER_LENGTH + MIDDLE_LENGTH * this->completed_ );
	// x-pos
	this->output_.write( QString( "%1" ).arg( qrand() % 9999 + 1, 5, 10, QChar( ' ' ) ).toUtf8() );
	// y-pos
	this->output_.write( QString( "%1" ).arg( qrand() % 9999 + 1, 5, 10, QChar( ' ' ) ).toUtf8() );
	// file size
	this->output_.write( QString( "%1" ).arg( data.size(), 9, 10, QChar( ' ' ) ).toUtf8() );
	// file type
	this->output_.write( type );

	this->output_.seek( HEADER_LENGTH + MIDDLE_LENGTH * this->jobs_ + this->totalSize_ );
	this->output_.write( data );

	this->completed_ += 1;
	this->setValue( this->completed_ );
	this->totalSize_ += data.size();
	if( this->completed_ >= this->jobs_ ) {
		this->output_.close();
		emit this->finished();
	}
}
