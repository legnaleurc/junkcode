#ifndef MAINWINDOWPRIVATE_HPP
#define MAINWINDOWPRIVATE_HPP

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

namespace qsnapshot {
	namespace widget {

		class MainWindow::Private : public QObject {
			Q_OBJECT
		public:
			explicit Private( MainWindow * host );

			MainWindow * host;
			Ui::MainWindow ui;
		};

	}
}

#endif
