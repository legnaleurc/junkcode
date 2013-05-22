#ifndef MAINWINDOWPRIVATE_HPP
#define MAINWINDOWPRIVATE_HPP

#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "umlscene.hpp"

#include <QtGui/QButtonGroup>

class MainWindow::Private : public QObject {
	Q_OBJECT

public:
	explicit Private( MainWindow * parent );

public slots:
	void onToolGroupClicked( int id );
	void onChangeNameClicked();
	void onGroupClicked();
	void onUngroupClicked();
	void onExportClicked();

public:
	MainWindow * owner;
	Ui::MainWindow ui;
	UMLScene * scene;
	QButtonGroup * toolGroup;
};

#endif
