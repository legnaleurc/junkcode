#include "robot.h"
#ifdef Q_OS_MAC
#include "macrobot.h"
#endif

#include <QtTest/QTest>
#include <QtCore/QThread>
#include <QtWidgets/QApplication>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

const auto MATCH_METHOD = CV_TM_CCOEFF_NORMED;

// If inImage exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inImage's
// data with the cv::Mat directly
//    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
inline cv::Mat QImageToCvMat( const QImage &inImage, bool inCloneImageData = true )
{
   switch ( inImage.format() )
   {
	  // 8-bit, 4 channel
	  case QImage::Format_RGB32:
	  {
		 cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

		 return (inCloneImageData ? mat.clone() : mat);
	  }

	  // 8-bit, 3 channel
	  case QImage::Format_RGB888:
	  {
		 if ( !inCloneImageData )
			qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

		 QImage   swapped = inImage.rgbSwapped();

		 return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
	  }

	  // 8-bit, 1 channel
	  case QImage::Format_Indexed8:
	  {
		 cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

		 return (inCloneImageData ? mat.clone() : mat);
	  }

	  default:
		 qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
		 break;
   }

   return cv::Mat();
}

// If inPixmap exists for the lifetime of the resulting cv::Mat, pass false to inCloneImageData to share inPixmap's data
// with the cv::Mat directly
//    NOTE: Format_RGB888 is an exception since we need to use a local QImage and thus must clone the data regardless
inline cv::Mat QPixmapToCvMat( const QPixmap &inPixmap, bool inCloneImageData = true )
{
   return QImageToCvMat( inPixmap.toImage(), inCloneImageData );
}

inline QImage  cvMatToQImage( const cv::Mat &inMat )
{
   switch ( inMat.type() )
   {
	  // 8-bit, 4 channel
	  case CV_8UC4:
	  {
		 QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

		 return image;
	  }

	  // 8-bit, 3 channel
	  case CV_8UC3:
	  {
		 QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

		 return image.rgbSwapped();
	  }

	  // 8-bit, 1 channel
	  case CV_8UC1:
	  {
		 static QVector<QRgb>  sColorTable;

		 // only create our color table once
		 if ( sColorTable.isEmpty() )
		 {
			for ( int i = 0; i < 256; ++i )
			   sColorTable.push_back( qRgb( i, i, i ) );
		 }

		 QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

		 image.setColorTable( sColorTable );

		 return image;
	  }

	  default:
		 qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
		 break;
   }

   return QImage();
}

inline QPixmap cvMatToQPixmap( const cv::Mat &inMat )
{
   return QPixmap::fromImage( cvMatToQImage( inMat ) );
}

Robot * Robot::create(QWidget * widget) {
#ifdef Q_OS_MAC
	auto robot = new MacRobot;
#else
	auto robot = new Robot;
#endif
	robot->_widget = widget;
	return robot;
}

Robot::Robot(): QObject(),
	_widget(nullptr)
{
}

Robot::~Robot() {
}

QWidget * Robot::getWidget() const {
	return this->_widget;
}

QPoint Robot::find(const QPixmap &target) const {
	auto screen = this->getWidget()->grab();
	auto s = QPixmapToCvMat(screen, false);
	auto t = QPixmapToCvMat(target, false);
	cv::Mat r;

	int rc = s.cols - t.cols + 1;
	int rr = s.rows - t.rows + 1;
	r.create(rr, rc, CV_32FC1);
	cv::matchTemplate(s, t, r, MATCH_METHOD);
//	cv::normalize( r, r, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
	cv::Point matchLoc;

	cv::minMaxLoc( r, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

	if( MATCH_METHOD  == CV_TM_SQDIFF || MATCH_METHOD == CV_TM_SQDIFF_NORMED ) {
		matchLoc = minLoc;
	} else {
		matchLoc = maxLoc;
	}
	if (MATCH_METHOD == CV_TM_CCOEFF_NORMED) {
		if (maxVal < 0.8) {
			qDebug() << "not matched";
			return QPoint();
		}
	}

	QRect match(matchLoc.x, matchLoc.y, target.width(), target.height());
	qDebug() << match;
	auto center = match.center();
	qDebug() << center;

	return center;
}

void Robot::click(const QPixmap &target, int msDelay) const {
	auto center = this->find(target);
	this->click(center, msDelay);
}

void Robot::click(const QPoint &pos, int msDelay) const {
	assert(qApp->thread() == QThread::currentThread() || !"must run on the main thread");
	this->doClick(pos, msDelay);
	emit this->clickFinished();
}

void Robot::doClick(const QPoint &pos, int msDelay) const {
	QTest::mouseClick(this->getWidget(), Qt::LeftButton, Qt::NoModifier, pos, msDelay);
}
