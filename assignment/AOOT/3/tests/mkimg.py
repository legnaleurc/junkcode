#! /usr/bin/env python

from PySide import QtGui, QtCore
import random, os, sys

class ImageGenerator( QtCore.QObject, QtCore.QRunnable ):

	finished = QtCore.Signal()

	def __init__( self ):
		QtCore.QObject.__init__( self )
		QtCore.QRunnable.__init__( self )

	def run( self ):
		w = random.randint( 1, 10000 )
		h = random.randint( 1, 10000 )
		image = QtGui.QImage( w, h, QtGui.QImage.Format_ARGB32_Premultiplied )
		pixel = random.randint( 0, 2 ** 32 )
		image.fill( pixel )
		fileName = u'imgs/_{0}.{1}'.format( random.getrandbits( 64 ), random.choice( [ u'jpg', u'png' ] ) )
		image.save( fileName )
		self.finished.emit()

class Controller( QtCore.QObject ):

	finished = QtCore.Signal()

	def __init__( self, count ):
		QtCore.QObject.__init__( self )
		self.__count = count
		self.__completed = 0

	def start( self ):
		for i in xrange( self.__count ):
			ig = ImageGenerator()
			ig.finished.connect( self.__onFinished )
			ig.finished.connect( ig.deleteLater )
			QtCore.QThreadPool.globalInstance().start( ig )

	def __onFinished( self ):
		self.__completed = self.__completed + 1
		print '{0}%'.format( self.__completed / float( self.__count ) * 100 )
		if self.__completed >= self.__count:
			self.finished.emit()

app = QtCore.QCoreApplication( sys.argv )

jobs = Controller( 100 )
jobs.finished.connect( app.quit )
jobs.start()

sys.exit( app.exec_() )
